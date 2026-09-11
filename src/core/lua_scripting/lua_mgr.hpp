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


#pragma once

#include <sol/sol.hpp>

#include "lua_module.hpp"
#include "lua_library.hpp"


namespace YLP::LuaJIT
{
	class LuaManager : public Singleton<LuaManager>
	{
		friend class Singleton<LuaManager>;

	private:
		LuaManager() noexcept = default;
		~LuaManager() = default;

		LuaManager(const LuaManager&) = delete;
		LuaManager(LuaManager&&) = delete;
		LuaManager& operator=(const LuaManager&) = delete;
		LuaManager& operator=(LuaManager&&) = delete;

		void LoadModuleImpl(const fs::path& path);
		void LoadDisabledModuleImpl(const fs::path& path);
		void LoadDisabledModulesImpl(const fs::path& path);
		void EnableModuleImpl(const fs::path& path);
		void LoadModulesImpl();
		void ReloadAllModulesImpl();
		void RegisterLibraryImpl(LuaLibrary* library);
		void RegisterLibrariesImpl(sol::state& L);
		void UpdateImpl();

	public:
		struct DisabledModule
		{
			std::string m_Name;
			fs::path m_Path;
		};

		static void Init(const fs::path& projectRoot)
		{
			GetInstance().InitImpl(projectRoot);
		}

		static void Destroy()
		{
			GetInstance().DestroyImpl();
		}

		static void Update()
		{
			GetInstance().UpdateImpl();
		}

		static void RegisterLibrary(LuaLibrary* library)
		{
			GetInstance().RegisterLibraryImpl(library);
		}

		static void RegisterLibraries(sol::state& L)
		{
			GetInstance().RegisterLibrariesImpl(L);
		}

		static void ExecuteCode(const std::string& code)
		{
			GetInstance().ExecuteCodeImpl(code);
		}

		static void LoadModule(const fs::path& modulePath)
		{
			GetInstance().LoadModuleImpl(modulePath);
		}

		static void EnableModule(const fs::path& modulePath)
		{
			GetInstance().EnableModuleImpl(modulePath);
		}

		static void ReloadAllModules()
		{
			GetInstance().m_ShouldReload.store(true);
		}

		static std::vector<std::shared_ptr<LuaModule>>& GetModules()
		{
			return GetInstance().m_Modules;
		}

		static std::vector<DisabledModule>& GetDisabledModules()
		{
			return GetInstance().m_DisabledModules;
		}
	private:
		void InitImpl(const fs::path& pluginsPath);
		void ExecuteCodeImpl(const std::string& code);
		void DestroyImpl();

		fs::path m_PluginsDir{};

		std::atomic_bool m_Initialized{false};
		std::atomic_bool m_ShouldReload{false};

		std::mutex m_LoadedModulesMutex{};
		std::mutex m_DisabledModulesMutex{};

		std::vector<std::shared_ptr<LuaModule>> m_Modules{};
		std::queue<fs::path> m_LoadQueue{};
		std::vector<DisabledModule> m_DisabledModules{};
		std::vector<LuaLibrary*> m_Libraries{};

		std::unique_ptr<LuaModule> m_CodeExecutor{nullptr};

		std::chrono::time_point<std::chrono::steady_clock> m_LastReload{};
	};
}
