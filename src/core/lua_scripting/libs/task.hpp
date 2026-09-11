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


#pragma once

#include "../lua_library.hpp"
#include "../lua_module.hpp"


namespace YLP::LuaJIT
{
	class LuaScriptLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	private:
		static inline void register_task(sol::protected_function func, sol::optional<int> delay, sol::this_state s)
		{
			auto luaModule = GetModuleFromLuaState(s.lua_state(), "Failed to register task! Module pointer is null.");
			if (!luaModule)
				return;

			int millis = std::max(0, delay.value_or(0));
			luaModule->RegisterTask(func, static_cast<std::chrono::milliseconds>(millis));
		}

	public:
		void Register(sol::state& L) override
		{
			/* @ylp.table Task
			* description
				Provides functions to run tasks in YLP's thread.

			* function Run
			* param callback<function> The callback to execute

			* function RunDelayed Execute a callback after a delay
			* param delay<integer> Delay in milliseconds
			* param callback<function> The function to execute
			
			* function Sleep
			* param ms<integer> Sleep time in milliseconds
			
			* function Yield
			* param ms<integer?> Optional yield time in milliseconds
			@*/

			auto taskTable = L["Task"].get_or_create<sol::table>();
			taskTable["Run"] = [](sol::protected_function func, sol::this_state state) {
				register_task(func, 0, state);
			};

			taskTable["RunDelayed"] = [](int delayMs, sol::protected_function func, sol::this_state state) {
				register_task(func, delayMs, state);
			};

			taskTable["Sleep"] = sol::yielding([](int millis) { return millis; });
			taskTable["Yield"] = sol::yielding([](sol::optional<int> millis) { return millis.value_or(0); });
		}
	};

	LuaScriptLib _LuaScriptLib;
}
