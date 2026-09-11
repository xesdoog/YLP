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

#include "renderer.hpp"
#include "theme.hpp"
#include "fonts/fonts.hpp"
#include "widgets/info_callout.hpp"
#include "widgets/segmented_ctrl.hpp"
#include "widgets/spinner.hpp"
#include "widgets/theme_preview.hpp"
#include "widgets/wrapped_selectable.hpp"


namespace ImGui
{
	enum ImKVflags // for DrawKeyValue
	{
		KVflagsNone,
		KVflagsHyperlink,
		KVflagsBullet
	};

	struct ImButtonColorScheme
	{
		ImVec4 Base;
		ImVec4 Hover;
		ImVec4 Active;
	};

	ImButtonColorScheme MakeButtonColors(
	    ImVec4 baseColor,
	    float hoverFactor = 1.15f,
	    float activeFactor = 0.9f);

	bool ColoredButton(
	    const char* label,
	    ImVec4 baseColor,
	    float hoverFactor = 1.15f,
	    float activeFactor = 0.9f);

	void ImageRounded(
	    ImTextureID texture_id,
	    float diameter,
	    const ImVec2& uv0 = ImVec2(0, 0),
	    const ImVec2& uv1 = ImVec2(1, 1),
	    const ImVec4& tint_col = ImVec4(1, 1, 1, 1));

	void ToolTip(const char* text,
	    ImFont* font = nullptr,
	    bool delayed = true,
	    float textWrapWidth = -1.0f);

	bool SelectableLabel(const char* label, bool selected);
	void HelpMarker(const char* text, ImFont* font = nullptr);
	void WarningMessage(const char* text);
	void TitleText(const char* text, bool separator = false);
	void TextCentered(const char* text, ImFont* font = nullptr, float availWidth = 0.0f);
	void SameLineIfAvail(float itemwidth, float region = -1.0f);
	void DrawKeyValue(const char* key,
	    const std::string& value,
	    bool copyable = false,
	    ImVec4 valueColor = ImGui::GetStyle().Colors[ImGuiCol_Text],
	    ImKVflags valueDrawFlags = KVflagsNone,
	    std::string optionalUrl = "");

	void DrawRotatingImage(
	    ImDrawList* drawList,
	    ImTextureID texture,
	    ImVec2 center,
	    ImVec2 size,
	    float angle);

	ImFont* GetScaledFont();

}
