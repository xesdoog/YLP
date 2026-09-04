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

		void LoadModuleImpl(fs::path path);
		void LoadDisabledModuleImpl(fs::path path);
		void LoadModulesImpl();
		void RegisterLibraryImpl(LuaLibrary* library);
		void RegisterLibrariesImpl(sol::state& L);
		void UpdateImpl();

	public:
		struct DisabledModule
		{
			std::string m_Name;
			fs::path m_Path;
		};

		static void Init(const fs::path& pluginsPath)
		{
			GetInstance().InitImpl(pluginsPath);
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

		static void LoadModule(fs::path path)
		{
			GetInstance().LoadModuleImpl(path);
		}

		static void LoadDisabledModule(fs::path path)
		{
			GetInstance().LoadDisabledModuleImpl(path);
		}

		static void LoadModules()
		{
			GetInstance().LoadModulesImpl();
		}

		static std::vector<std::shared_ptr<LuaModule>>& GetModules()
		{
			return GetInstance().m_Modules;
		}

		static std::vector<DisabledModule>& GetDisabledModules()
		{
			return GetInstance().m_DisabledModules;
		}

		static void ExecuteCode(const std::string& code)
		{
			if (!Config().enableScripting)
				return;

			auto& executor = GetInstance().m_CodeExecutor;
			if (!executor)
			{
				LOG_ERROR("CodeExecutor has not been initialized!");
				return;
			}
			executor->RunScript(code);
		}
	private:
		void InitImpl(const fs::path& pluginsPath);
		void DestroyImpl();

		std::mutex m_Mutex{};
		std::vector<std::shared_ptr<LuaModule>> m_Modules{};
		std::queue<fs::path> m_LoadQueue{};
		std::vector<DisabledModule> m_DisabledModules{};
		std::vector<LuaLibrary*> m_Libraries{};

		fs::path m_PluginsDir{};

		std::unique_ptr<LuaModule> m_CodeExecutor{nullptr};
	};
}
