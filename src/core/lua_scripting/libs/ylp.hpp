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
#include "../../memory/scanner.hpp"
#include "../../utils/psutils.hpp"
#include "../../updater.hpp"


namespace YLP::LuaJIT
{
	class LuaYLPLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	public:
		void Register(sol::state& L) override
		{
			/* @ylp.table YLP
			* description
				### YLP namespace

			* function GetVersion
			* return string version The current YLP version.

			* function RegisterProcessWatcher Registers a callback to be executed once when a process is first seen.~~You can call `Task.Yield` and `Task.Sleep` in your callback function.
			* param processName<string> The name of the process
			* param callback<fun(process: Process)> The function to execute. YLP will pass a Process object to the function as an argument.
			* param delay<integer?> Optional delay in milliseconds
			* return boolean success Whether the registration was successful or not.

			* function InjectDll Injects a dynamic link library into a target process.
			* param dllPath<string> Path to the DLL file.
			* param processName<string> Name of the target process.
			* return boolean status Success or failure.
			* return string? failReason Optional error message if injection fails.

			* function OnShutdown Registers a function to be executed when YLP is shutting down.
			* param callback<function> The function to execute
			@*/
			auto ylpTable = L["YLP"].get_or_create<sol::table>();

			ylpTable["GetVersion"] = []() {
				return YLPUpdater.GetLocalVersion().ToString();
			};

			ylpTable["RegisterProcessWatcher"] = [&](const std::string& processName, sol::protected_function callback, sol::optional<int> delayMs) {
				auto module = GetModuleFromLuaState(L);
				if (!module)
					return false;

				int ms = std::max(0, delayMs.value_or(0));
				module->RegisterProcessWatcher(processName, callback, std::chrono::milliseconds(ms));
				return true;
			};

			ylpTable["OnShutdown"] = [&](sol::protected_function callback) {
				auto module = GetModuleFromLuaState(L);
				if (!module)
					return;

				module->RegisterShutdownCallback(callback);
			};

			ylpTable["InjectDll"] = [&](const fs::path& dllPath, const std::string& processName) {
				InjectResult res = PsUtils::Inject(processName, dllPath);
				return std::make_tuple(res.success, res.message);
			};
		}
	};

	LuaYLPLib _LuaYLPLib;
}
