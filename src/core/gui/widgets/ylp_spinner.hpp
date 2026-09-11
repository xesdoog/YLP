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

#include "../renderer.hpp"
#include "../../../resources/logos/ylp_gear.hpp"
#include "../../../resources/logos/ylp_brand.hpp"


namespace ImGui
{
	using namespace YLP;
	inline ImTextureID __gearTex  = NULL;
	inline ImTextureID __brandTex = NULL;

	inline void YLPSpinner(const char* text = "", ImVec2 size = ImVec2(ImGui::GetFrameHeight(), ImGui::GetFrameHeight()), float speed = 2.8f)
	{
		if (!__gearTex)
			__gearTex = Renderer::LoadTextureFromMemory(logo_gear_data, logo_gear_size, "YLP_GEAR");

		if (!__brandTex)
			__brandTex = Renderer::LoadTextureFromMemory(logo_brand_data, logo_brand_size, "YLP_BRND");

		static float angle = 0.0f;
		angle += ImGui::GetIO().DeltaTime * speed;
		
		const ImVec2 pos     = ImGui::GetCursorScreenPos();
		const ImVec2 center  = pos + size * 0.5f;
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		ImGui::SetCursorScreenPos(pos);
		ImGui::Image(__brandTex, size);
		DrawRotatingImage(drawList, __gearTex, center, size, angle);

		if (text && strncmp(text, "##", 2) != 0)
		{
			drawList->AddText(
			    ImVec2(pos.x + size.x + ImGui::GetStyle().ItemSpacing.x, center.y - ImGui::CalcTextSize(text).y * 0.5f),
			    ImGui::GetColorU32(ImGuiCol_Text),
			    text);
		}
	}
}
