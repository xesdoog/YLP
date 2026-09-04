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


#include "lua_module.hpp"
#include "lua_mgr.hpp"


namespace YLP::LuaJIT
{
	LuaModule::LuaModule(fs::path root) :
		m_SolState(sol::state{}),
		m_Root(root),
		m_Entry(root / "main.lua"),
		m_Name(root.filename().string())
	{
		m_SolState.open_libraries(
		    sol::lib::base,
		    sol::lib::bit32,
		    sol::lib::coroutine,
		    sol::lib::ffi,
		    sol::lib::jit,
		    sol::lib::math,
		    sol::lib::string,
		    sol::lib::table,
		    sol::lib::utf8);

		m_SolState["this*"] = reinterpret_cast<void*>(this);
		m_SolState["whodis"] = m_Name;
		LuaManager::RegisterLibraries(m_SolState);
	}

	bool LuaModule::Load()
	{
		if (!Config().enableScripting)
			return false;

		auto result = m_SolState.safe_script_file(m_Entry.string(), &sol::script_pass_on_error, sol::load_mode::text);
		if (!result.valid())
		{
			sol::error e = result;
			LOG_ERROR("Failed to load module! {}", m_Name, e.what());
			m_LoadState = BROKEN;
			return false;
		}

		LOG_INFO("Loaded module '{}'", m_Name);
		m_LoadState = RUNNING;
		return true;
	}

	void LuaModule::RunScript(const std::string& code)
	{
		if (!Config().enableScripting)
			return;

		if (auto result = m_SolState.safe_script(code, &sol::script_pass_on_error); !result.valid())
		{
			sol::error error = result;
			LOG_ERROR(error.what());
		}
	}
}
