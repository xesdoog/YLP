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


namespace YLP::LuaJIT
{
	class LuaProcLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	public:
		void Register(sol::state& L) override
		{
			/* @ylp.class Process
			* description
				A process abstraction providing basic process interactions.

			* constructor __call
			* param processName<string>

			* method IsRunning
			* return boolean
			
			* method IsModuleLoaded
			* param moduleName<string> Name of the module. Example: `"kernelbase.dll"`
			* return boolean

			* method GetModuleSize
			* return integer

			* method GetModuleBase
			* return integer -- The module's base address

			* method Update Updates the process object's internal state. This is useful because the `Process` class does not poll for status on its own.

			* method FindPattern
			* param pattern<string> IDA-style byte signature. Example: `"8B 88 C0 ?? ?? ?? 89 8F"`
			* param name<string?> Optional name of the pattern to scan. Purely for logging purposes.
			* param chunkSize<integer?> Optional memory chunk size. Defaults to 4096
			* return Pointer ptr A pointer at the found address or a null pointer if the scan fails. This is guaranteed to always return a Pointer object.
			@*/
			auto processUsertype = L.new_usertype<ProcessScanner>("Process",
			    sol::call_constructor, sol::constructors<ProcessScanner(std::string)>(),
			    "IsRunning", &ProcessScanner::IsProcessRunning,
			    "IsModuleLoaded", &ProcessScanner::IsModuleLoaded,
			    "GetModuleSize", &ProcessScanner::GetModuleSize,
			    "GetModuleBase", &ProcessScanner::GetBaseAddress,
			    "Update", &ProcessScanner::FindProcess);

			processUsertype["FindPattern"] = [&](
				ProcessScanner& self,
				const std::string& pattern,
				sol::optional<std::string> name,
				sol::optional<size_t> chunkSize)
			{
				return self.FindPattern(pattern, name.value_or(pattern), chunkSize.value_or(4096));
			};
		}
	};

	LuaProcLib _LuaProcLib;
}
