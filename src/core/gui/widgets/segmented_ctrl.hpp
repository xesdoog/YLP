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


namespace ImGui
{
	enum class ImSegmentedControlAnchorPos : uint8_t
	{
		LEFT,
		CENTER,
		RIGHT,
	};

	inline bool SegmentedControl(const char* id,
		int* current,
		std::initializer_list<const char*> items,
		ImSegmentedControlAnchorPos anchorPos = ImSegmentedControlAnchorPos::LEFT)
	{
		ImGui::PushID(id);
		ImGuiStyle& style	 = ImGui::GetStyle();
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const float height	 = ImGui::GetFrameHeight();
		const float rounding = style.FrameRounding;

		float width = 0.0f;
		for (const char* item : items)
			width += ImGui::CalcTextSize(item).x + style.FramePadding.x * 2.0f;

		ImVec2 cursorPos = ImGui::GetCursorScreenPos();
		ImVec2 size(width, height);
		ImVec2 startPos{};
		switch (anchorPos)
		{
			case ImSegmentedControlAnchorPos::LEFT:
				startPos = cursorPos;
				break;
			case ImSegmentedControlAnchorPos::CENTER:
			    startPos = cursorPos + ImVec2((ImGui::GetContentRegionAvail().x - size.x) * 0.5f, 0);
				break;
			case ImSegmentedControlAnchorPos::RIGHT:
			    startPos = ImGui::GetCursorScreenPos() + ImVec2(ImGui::GetContentRegionAvail().x - size.x, 0);
			    break;
		    default:
			    startPos = cursorPos;
			    break;
		}

		drawList->AddRectFilled(
		    startPos,
		    startPos + size,
		    ImGui::GetColorU32(ImGuiCol_FrameBg),
		    rounding);

		float x   = startPos.x;
		int index = 0;

		for (const char* item : items)
		{
			float itemWidth = ImGui::CalcTextSize(item).x + style.FramePadding.x * 2.0f;
			ImVec2 itemPos(x, startPos.y);
			ImVec2 itemSize(itemWidth, height);
			ImGui::SetCursorScreenPos(itemPos);

			bool pressed = ImGui::InvisibleButton(item, itemSize);
			bool hovered = ImGui::IsItemHovered();
			bool selected = (*current == index);
			if (selected)
			{
				drawList->AddRectFilled(
				    itemPos,
				    itemPos + itemSize,
				    ImGui::GetColorU32(ImGuiCol_ButtonActive),
				    rounding);
			}

			ImVec2 textSize = ImGui::CalcTextSize(item);
			drawList->AddText(
			    itemPos + ImVec2((itemWidth - textSize.x) * 0.5f, (height - textSize.y) * 0.5f),
			    ImGui::GetColorU32(selected || hovered ? ImGuiCol_Text : ImGuiCol_TextDisabled), 
				item);

			if (hovered)
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			if (pressed)
				*current = index;

			x += itemWidth;
			index++;
		}

		ImGui::SetCursorScreenPos(startPos);
		ImGui::Dummy(size);
		ImGui::PopID();

		return true;
	}
}
