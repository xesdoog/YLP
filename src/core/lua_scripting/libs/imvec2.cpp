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
	class LuaImVec2Lib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	public:
		void Register(sol::state& L) override
		{
			// clang-format off

			/* @ylp.class ImVec2
			* description
				A 2D Vector user type to be used in ImGui functions.

			* constructor __call
			* param x<number>
			* param y<number>

			* operator add
			* param other<ImVec2>
			* return ImVec2

			* operator sub
			* param other<ImVec2>
			* return ImVec2

			* operator mul
			* param other<number>
			* return ImVec2

			* operator div
			* param other<ImVec2>
			* return ImVec2
			
			* field x<number> The x componend of the vector.
			
			* field y<number> The y componend of the vector.
			@*/
			L.new_usertype<ImVec2>("ImVec2",
			    sol::call_constructor, sol::constructors<ImVec2(float, float)>(),

			    "x", &ImVec2::x,
			    "y", &ImVec2::y,

			    sol::meta_function::addition, [](const ImVec2& self, const ImVec2& other) {
				    return ImVec2(self.x + other.x, self.y + other.y);
			    },

			    sol::meta_function::subtraction, [](const ImVec2& self, const ImVec2& other) {
				    return ImVec2(self.x - other.x, self.y - other.y);
			    },

			    sol::meta_function::multiplication, sol::overload(
					[](const ImVec2& self, float v) {
						return ImVec2(self.x * v, self.y * v);
					},
			        [](float v, const ImVec2& self) {
				        return ImVec2(self.x * v, self.y * v);
			        }
				),

			    sol::meta_function::division, [](const ImVec2& self, float v) {
				    return ImVec2(self.x / v, self.y / v);
			    },

			    sol::meta_function::equal_to, [](const ImVec2& self, const ImVec2& other) {
				    return self.x == other.x && self.y == other.y;
			    });

			    sol::meta_function::to_string, [](const ImVec2& self) {
				    return std::format("({:.3f}, {:.3f})", self.x, self.y);
			    };
		};
	};

	LuaImVec2Lib _LuaImVec2Lib;
}
