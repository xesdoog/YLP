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
#include "resources/fonts/fonts.hpp"


namespace YLP::LuaJIT
{
	class LuaFontsLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	public:
		void Register(sol::state& L) override
		{
			std::unordered_map<std::string, ImFont*> fonts = {
			    { "Small", Fonts::Small },
			    {"Regular", Fonts::Regular},
			    {"Subtitle", Fonts::Subtitle},
			    {"Title", Fonts::Title}
			};

			L.new_usertype<ImFont>("ImFont", sol::no_constructor);

			/* @ylp.table Fonts
			* description
				A readonly table that stores pointers to YLP's fonts. Used in combination with `ImGui.PushFont`

			* field Regular<userdata> The default font.
			* field Small<userdata>
			* field Subtitle<userdata>
			* field Title<userdata> The biggest available font.
			@*/
			auto fontsTable  = L.create_named_table("Fonts");
			auto mt          = L.create_table();
			mt["__index"]    = fonts;
			mt["__newindex"] = [&]() {
				luaL_error(L, "Attempt to modify a read only table!");
			};

			fontsTable[sol::metatable_key] = mt;
		};
	};

	LuaFontsLib _LuaFontsLib;
}
