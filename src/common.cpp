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


#include "common.hpp"

#ifndef STB_IMAGE_IMPLEMENTATION
	#define STB_IMAGE_IMPLEMENTATION
#endif
#include <thirdparty/stb_image.h>


namespace YLP
{
	extern HINSTANCE g_Instance{nullptr};
	extern HWND g_Hwnd{0};
	extern fs::path g_ProjectPath{};
	extern fs::path g_YimPath{};
	extern fs::path g_YimV2Path{};
	extern std::atomic<bool> g_Running{false};
}
