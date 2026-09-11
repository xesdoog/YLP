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
	class LuaLogLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	private:
		static inline std::string lua_tostr(sol::state& L, sol::variadic_args args)
		{
			auto moduleName = L["whodis"].get<std::string>();
			std::string output;
			for (auto arg : args)
			{
				if (!output.empty())
					output += '\t';

				sol::protected_function tostring  = L["tostring"];
				sol::protected_function_result r  = tostring(arg);

				output += r.valid() ? r.get<std::string>() : "";
			}
			return "[" + moduleName + (moduleName == "CodeExecutor" ? "]: " : "/main.lua]: ") + output;
		}
		static inline std::string lua_fmt(sol::state& L, sol::variadic_args args)
		{
			auto moduleName                  = L["whodis"].get<std::string>();
			sol::table string                = L["string"];
			sol::protected_function fmt      = string["format"];
			sol::protected_function_result r = fmt(args);
			if (!r.valid())
			{
				sol::error err = r;
				LOG_ERROR("{}: {}", moduleName, err.what());
				return "<format error!>";
			}
			return "[" + moduleName + (moduleName == "CodeExecutor" ? "]: " : "/main.lua]: ") + r.get<std::string>();
		}

	public:
		void Register(sol::state& L) override
		{
			L["print"] = [&](sol::variadic_args args) {
				LOG_INFO(lua_tostr(L, args));
			};

			/*@ylp.function printf Prints a formatted message. Arguments are the same as `string.format`
			* param msg<string> Message.
			* param ...<any> Optional format arguments.
			@*/
			L["printf"] = [&](sol::variadic_args args) {
				LOG_INFO(lua_fmt(L, args));
			};

			/*@ylp.table log
			* description
				Provides functions to output text to console and log file.

			* function info Logs an information message.
			* param ...<any> Any number of arguments of any type.
			
			* function warning Logs a warning message.
			* param ...<any> Any number of arguments of any type.
			
			* function debug Logs a debug message.
			* param ...<any> Any number of arguments of any type.
			
			* function error Logs an error message.
			* param ...<any> Any number of arguments of any type.
			
			* function finfo Logs a formatted information message. Arguments are the same as `string.format`
			* param msg<string> Message.
			* param ...<any> Optional format arguments.
			
			* function fwarning Logs a formatted warning message. Arguments are the same as `string.format`
			* param msg<string> Message.
			* param ...<any> Optional format arguments.
			
			* function fdebug Logs a formatted debug message. Arguments are the same as `string.format`
			* param msg<string> Message.
			* param ...<any> Optional format arguments.
			
			* function ferror Logs a formatted error message. Arguments are the same as `string.format`
			* param msg<string> Message.
			* param ...<any> Optional format arguments.
			@*/
			auto log = L["log"].get_or_create<sol::table>();

			log["info"] = [&](sol::variadic_args args) {
				LOG_INFO(lua_tostr(L, args));
			};

			log["warning"] = [&](sol::variadic_args args) {
				LOG_WARN(lua_tostr(L, args));
			};

			log["debug"] = [&](sol::variadic_args args) {
				LOG_DEBUG(lua_tostr(L, args));
			};

			log["error"] = [&](sol::variadic_args args) {
				LOG_ERROR(lua_tostr(L, args));
			};

			log["finfo"] = [&](sol::variadic_args args) {
				LOG_INFO(lua_fmt(L, args));
			};

			log["fwarning"] = [&](sol::variadic_args args) {
				LOG_WARN(lua_fmt(L, args));
			};

			log["fdebug"] = [&](sol::variadic_args args) {
				LOG_DEBUG(lua_fmt(L, args));
			};

			log["ferror"] = [&](sol::variadic_args args) {
				LOG_ERROR(lua_fmt(L, args));
			};
		}
	};

	LuaLogLib _LuaLogLib;
}
