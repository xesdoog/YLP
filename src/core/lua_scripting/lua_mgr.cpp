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
	void LuaManager::InitImpl(const fs::path& pluginsPath)
	{
		if (!Config().enableScripting)
			return;

		m_PluginsDir = pluginsPath;
		std::error_code ec{};
		if (!fs::exists(m_PluginsDir))
			fs::create_directory(m_PluginsDir, ec);

		if (!fs::exists(m_PluginsDir / "shared"))
			fs::create_directory(m_PluginsDir / "shared", ec);

		if (!fs::exists(m_PluginsDir / "disabled"))
			fs::create_directory(m_PluginsDir / "disabled", ec);

		LoadModulesImpl();
		LOG_INFO("Lua manager initialized.");
	}

	void LuaManager::DestroyImpl()
	{
		std::lock_guard lock(m_Mutex);
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

	void LuaManager::LoadModuleImpl(fs::path path)
	{
		if (!fs::exists(path) || !fs::is_directory(path))
			return;

		for (auto& entry : fs::directory_iterator(path))
		{
			if (!fs::is_regular_file(entry))
				continue;

			if (entry.path().filename().string() == "main.lua")
			{
				m_LoadQueue.push(path);
				break;
			}
		}
	}

	void LuaManager::LoadDisabledModuleImpl(fs::path path)
	{
		if (!fs::exists(path) || !fs::is_directory(path) || path.filename().string() != "disabled")
			return;

		for (auto& entry : fs::directory_iterator(path))
		{
			if (!fs::is_directory(entry))
				continue;

			auto rootname = entry.path().filename().string();
			if (rootname.empty())
				continue;

			m_DisabledModules.push_back({rootname, entry.path()});
		}
	}

	void LuaManager::LoadModulesImpl()
	{
		m_CodeExecutor = std::make_unique<LuaModule>("/CodeExecutor");

		std::error_code ec{};
		if (!fs::exists(m_PluginsDir, ec) || !fs::is_directory(m_PluginsDir, ec) || fs::is_empty(m_PluginsDir, ec))
			return;

		for (auto& entry : fs::directory_iterator(m_PluginsDir))
		{
			if (!fs::is_directory(entry))
				continue;

			auto rootname = entry.path().filename().string();
			if (rootname.empty() || rootname == "shared")
				continue;

			if (rootname == "disabled")
				LoadDisabledModule(entry.path());
			else
				LoadModule(entry.path());
		}

		std::lock_guard lock(m_Mutex);
		while (!m_LoadQueue.empty())
		{
			auto m = std::make_shared<LuaModule>(m_LoadQueue.front());
			if (m->Load())
				m_Modules.push_back(m);
			else
				m_DisabledModules.push_back({m->GetName().data(), m->GetRoot()});

			m_LoadQueue.pop();
		}
	}

	void LuaManager::UpdateImpl() // TODO
	{
		//std::scoped_lock lock(m_Mutex);
		//for (auto& module : m_Modules)
		//{
		//	switch (module->GetRunningState())
		//	{
		//	case LuaModule::WANTS_RELOAD:
		//		break;

		//	case LuaModule::WANTS_UNLOAD:
		//		break;

		//	case LuaModule::BROKEN:
		//		break;

		//	default:
		//		break;
		//	}
		//}
	}
}
