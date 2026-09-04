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


namespace YLP::LuaJIT
{
	class LuaModule
	{
	public:
		LuaModule(fs::path root);
		~LuaModule() = default;

		enum eLuaModuleState : uint8_t
		{
			NONE,
			RUNNING,
			BROKEN,
			WANTS_RELOAD,
			WANTS_UNLOAD
		};

		bool Load();

		void RunScript(const std::string& code);

		lua_State* GetLuaState()
		{
			return m_SolState.lua_state();
		}

		const eLuaModuleState GetRunningState() const noexcept
		{
			return m_LoadState;
		}

		std::string_view GetName() const
		{
			return m_Name;
		}

		fs::path GetRoot()
		{
			return m_Root;
		}

	private:
		std::string m_Name;
		fs::path m_Root;

		const std::chrono::time_point<std::chrono::file_clock> last_write_time() const;

		sol::state m_SolState;
		fs::path m_Entry;

		eLuaModuleState m_LoadState{NONE};

		std::mutex m_CallbackLock{};
	};
}
