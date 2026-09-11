// Copyright (C) 2025 SAMURAI (xesdoog) & Contributors
// This file is part of YLP.
//
// YLP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// YLP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with YLP.  If not, see <https://www.gnu.org/licenses/>.
//
// Credit goes to https://github.com/YimMenu/YimMenuV2 for most of this code.


#include "lua_mgr.hpp"
#include "lua_module.hpp"
#include "lib_loader.hpp"


namespace YLP::LuaJIT
{
	void LuaManager::InitImpl(const fs::path& projectRoot)
	{
		if (m_Initialized)
			return;

		if (!Config().enableScripting)
			return;

		if (!IO::Exists(projectRoot))
			return;

		m_PluginsDir = projectRoot / "Plugins";

		if (!IO::Exists(m_PluginsDir))
			IO::CreateFolder(m_PluginsDir);

		if (!IO::Exists(m_PluginsDir / "shared"))
			IO::CreateFolder(m_PluginsDir / "shared");

		if (!IO::Exists(m_PluginsDir / "disabled"))
			IO::CreateFolder(m_PluginsDir / "disabled");

		LoadModulesImpl();
		m_Initialized.store(true);
		LOG_INFO("Lua manager initialized.");

		ThreadManager::RunDelayed([this] {
			Update();
		}, 1s);
	}

	void LuaManager::DestroyImpl()
	{
		if (!m_Initialized)
			return;

		m_Initialized.store(false);
		std::unique_lock lock(m_LoadedModulesMutex);
		m_Modules.clear();
		m_DisabledModules.clear();
		LOG_DEBUG("LuaManager destroyed.");
	}

	void LuaManager::RegisterLibraryImpl(LuaLibrary* library)
	{
		m_Libraries.push_back(library);
	}

	void LuaManager::RegisterLibrariesImpl(sol::state& L)
	{
		int count = 0;
		for (auto library : m_Libraries)
		{
			count += 1;
			library->Register(L);
		}
	}

	void LuaManager::LoadModuleImpl(const fs::path& root)
	{
		if (!IO::IsDir(root))
			return;

		for (auto& entry : fs::directory_iterator(root))
		{
			if (!IO::IsFile(entry))
				continue;

			if (Utils::StringToLower(entry.path().filename().string()) == "main.lua")
			{
				m_LoadQueue.push(root);
				break;
			}
		}
	}

	void LuaManager::EnableModuleImpl(const fs::path& root)
	{
		std::scoped_lock lock(m_DisabledModulesMutex);
		std::erase_if(m_DisabledModules, [this, root](const DisabledModule& m) {
			if (m.m_Path == root)
			{
				if (root.parent_path().filename().string() == "disabled")
				{
					auto newRoot = m_PluginsDir / root.filename().string();
					if (!IO::Rename(root, newRoot))
						return false;
					
					LoadModuleImpl(newRoot);
					return true;
				}

				LoadModuleImpl(root);
				return true;
			}
			return false;
		});
	}

	void LuaManager::LoadDisabledModulesImpl(const fs::path& root)
	{
		if (!IO::Exists(root) || !IO::IsDir(root))
			return;

		for (auto& entry : fs::directory_iterator(root))
		{
			if (!IO::IsDir(entry))
				continue;

			auto rootname = entry.path().filename().string();
			if (rootname.empty())
				continue;

			m_DisabledModules.push_back({rootname, entry.path()});
		}
	}

	void LuaManager::LoadDisabledModuleImpl(const fs::path& root)
	{
		auto newRoot = m_PluginsDir / "disabled" / root.filename().string();
		if (IO::Rename(root, newRoot))
		{
			std::scoped_lock lock(m_DisabledModulesMutex);
			m_DisabledModules.push_back({newRoot.filename().string(), newRoot});
		}
	}

	void LuaManager::LoadModulesImpl()
	{
		if (!m_CodeExecutor)
			m_CodeExecutor = std::make_unique<LuaModule>("/CodeExecutor");

		if (!IO::Exists(m_PluginsDir) || !IO::IsDir(m_PluginsDir) || IO::IsEmpty(m_PluginsDir))
			return;

		for (auto& entry : fs::directory_iterator(m_PluginsDir))
		{
			if (!IO::IsDir(entry))
				continue;

			auto rootname = entry.path().filename().string();
			if (rootname.empty() || rootname == "shared")
				continue;

			if (rootname == "disabled")
				LoadDisabledModulesImpl(entry.path());
			else
				LoadModuleImpl(entry.path());
		}
	}

	// enabled modules only
	void LuaManager::ReloadAllModulesImpl()
	{
		{
			std::scoped_lock lock1(m_LoadedModulesMutex);
			std::scoped_lock lock2(m_DisabledModulesMutex);
			m_Modules.clear();
			m_DisabledModules.clear();
		}
		LoadModulesImpl();
	}

	void LuaManager::ExecuteCodeImpl(const std::string& code)
	{
		if (!m_Initialized || !Config().enableScripting || code.empty())
			return;

		auto& executor = m_CodeExecutor;
		if (!executor)
		{
			LOG_ERROR("CodeExecutor has not been initialized!");
			return;
		}
		m_CodeExecutor->Execute(code);
	}

	void LuaManager::UpdateImpl()
	{
		while (m_Initialized && Config().enableScripting && g_Running)
		{
			m_CodeExecutor->Tick();

			{
				std::scoped_lock lock(m_LoadedModulesMutex);
				while (!m_LoadQueue.empty())
				{
					auto m = std::make_shared<LuaModule>(m_LoadQueue.front());
					if (m->Load())
						m_Modules.push_back(m);
					else
						m_DisabledModules.push_back({m->GetName().data(), m->GetRoot()});

					m_LoadQueue.pop();
				}

				std::erase_if(m_Modules, [this](auto& m) {
					if (!m->IsSafeToUnload())
						return false;

					if (m_ShouldReload)
						return true;

					auto state = m->GetLoadState();
					if (state == LuaModule::WANTS_UNLOAD)
					{
						LoadDisabledModuleImpl(m->GetRoot());
						return true;
					}
					else if (state == LuaModule::WANTS_RELOAD)
					{
						m_LoadQueue.push(m->GetRoot());
						return true;
					}

					return false;
				});

				for (auto& m : m_Modules)
				{
					if (m->GetLoadState() == LuaModule::RUNNING)
						m->Tick();
				}
			}

			std::this_thread::sleep_for(1ms);

			auto now = std::chrono::steady_clock::now();
			if (m_ShouldReload && m_Modules.empty() && now - m_LastReload >= 1200ms)
			{
				m_ShouldReload = false;
				m_LastReload   = now;
				ReloadAllModulesImpl();
			}
		}
	}
}
