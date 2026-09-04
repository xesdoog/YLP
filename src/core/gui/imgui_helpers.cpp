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


#include "imgui_helpers.hpp"

namespace ImGui
{
	void SameLineIfAvail(float itemwidth, float region)
	{
		if (region <= 0.f)
			region = ImGui::GetContentRegionAvail().x;

		ImGui::SameLine();
		if (itemwidth >= ImGui::GetContentRegionAvail().x)
			ImGui::NewLine();
	}

	void ToolTip(const char* text, ImFont* font, bool delayed, float textWrapWidth)
	{
		ImGuiHoveredFlags flags = ImGuiHoveredFlags_AllowWhenDisabled;

		if (delayed)
			flags |= ImGuiHoveredFlags_DelayNormal;

		if (!ImGui::IsItemHovered(flags))
			return;

		if (!font)
			font = Fonts::Regular;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.f);
		ImGui::SetNextWindowBgAlpha(0.848f);
		ImGui::PushFont(font);
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(textWrapWidth >= 0.f ? textWrapWidth : ImGui::GetFontSize() * 25);
		ImGui::Text(text);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
		ImGui::PopFont();
		ImGui::PopStyleVar();
	}

	void HelpMarker(const char* text, ImFont* font)
	{
		ImGui::SameLine();
		ImGui::TextDisabled(ICON_MD_HELP);
		ToolTip(text, font, false);
	}

	void WarningMessage(const char* text)
	{
		ImGui::PushFont(Fonts::Title);
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35);
		ImGui::TextColored(ImVec4(1.0f, 0.7568, 0.027f, 1.0f), ICON_MD_WARNING);
		ImGui::SameLine();
		ImGui::Text("Warning");
		ImGui::PopFont();
		ImGui::PopTextWrapPos();

		ImGui::Dummy(ImVec2(0, 2.f));
		ImGui::TextWrapped(text);
		ImGui::Spacing();
	}

	void TitleText(const char* text, bool separator)
	{
		ImGui::PushFont(Fonts::Title);
		if (separator)
			ImGui::SeparatorText(text);
		else
			ImGui::Text(text);
		ImGui::PopFont();
	}

	ImButtonColorScheme MakeButtonColors(ImVec4 baseColor, float hoverFactor, float activeFactor)
	{
		auto Mul = [](ImVec4 c, float f) {
			return ImVec4(c.x * f, c.y * f, c.z * f, c.w);
		};

		ImButtonColorScheme s;
		s.Base = baseColor;
		s.Hover = Mul(baseColor, hoverFactor);
		s.Active = Mul(baseColor, activeFactor);
		return s;
	}

	bool ColoredButton(const char* label, ImVec4 baseColor, float hoverFactor, float activeFactor)
	{
		bool ret = false;
		ImButtonColorScheme scheme = MakeButtonColors(baseColor);

		ImGui::PushStyleColor(ImGuiCol_Button, scheme.Base);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, scheme.Hover);
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, scheme.Active);
		ret = ImGui::Button(label);
		ImGui::PopStyleColor(3);

		return ret;
	}

	void ImageRounded(
	    ImTextureID texture_id,
	    float diameter,
	    const ImVec2& uv0,
	    const ImVec2& uv1,
	    const ImVec4& tint_col)
	{
		ImVec2 p_min = ImGui::GetCursorScreenPos();
		ImVec2 p_max = ImVec2(p_min.x + diameter, p_min.y + diameter);
		ImGui::GetWindowDrawList()->AddImageRounded(texture_id, p_min, p_max, uv0, uv1, ImGui::GetColorU32(tint_col), diameter * 0.5f);
		ImGui::Dummy(ImVec2(diameter, diameter));
	}

	void TextCentered(const char* text, ImFont* font, float availWidth)
	{
		if (!font)
			font = Fonts::Regular;

		ImGui::PushFont(font);
		float windowWidth = availWidth ? availWidth : ImGui::GetWindowSize().x;
		float textWidth = ImGui::CalcTextSize(text).x;
		float textPosX = (windowWidth - textWidth) * 0.5f;
		ImGui::SetCursorPosX(textPosX > 0 ? textPosX : 0);
		ImGui::Text(text);
		ImGui::PopFont();
	}

	bool SelectableLabel(const char* icon, bool selected)
	{
		ImGui::BeginGroup();
		const ImVec2 framePadding = ImGui::GetStyle().FramePadding;
		const ImVec2 cursorPos = ImGui::GetCursorPos();
		float frameHeight = ImGui::GetFrameHeight();
		bool clicked = ImGui::InvisibleButton(icon, ImVec2(frameHeight, frameHeight));
		bool hovered = ImGui::IsItemHovered();
		auto colIdx = selected ? ImGuiCol_ButtonActive : (hovered ? ImGuiCol_ButtonHovered : (clicked ? ImGuiCol_Button : ImGuiCol_Text));
		if (hovered and ImGui::IsMouseDown(0))
			colIdx = ImGuiCol_Button;

		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(cursorPos.x + framePadding.x, cursorPos.y + framePadding.y));
		ImGui::TextColored(ImGui::GetStyleColorVec4(colIdx), icon);
		ImGui::EndGroup();
		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		return clicked;
	}

	void DrawKeyValue(const char* key,
	    const std::string& value,
	    bool copyable,
	    ImVec4 valueColor,
	    ImKVflags valueDrawFlags,
	    std::string optionalUrl)
	{
		ImGui::TextUnformatted(key);
		auto valsize = ImGui::CalcTextSize(value.c_str());
		ImGui::SameLine(ImGui::GetContentRegionAvail().x - valsize.x - (copyable ? 40 : 5));
		ImGui::PushStyleColor(ImGuiCol_Text, valueColor);

		switch (valueDrawFlags)
		{
		case KVflagsHyperlink:
			ImGui::TextLinkOpenURL(value.c_str(), optionalUrl.c_str());
			break;
		case KVflagsBullet:
			ImGui::BulletText(value.c_str());
			break;
		default:
			ImGui::TextUnformatted(value.c_str());
			break;
		}

		ImGui::PopStyleColor();
		if (copyable)
		{
			ImGui::SameLine();
			if (ImGui::SmallButton(ICON_MD_FILE_COPY))
				ImGui::SetClipboardText(value.c_str());
			ImGui::ToolTip("Copy");
		}
	};

	ImFont* GetScaledFont()
	{
		return YLP::Renderer::GetWindowSize().x >= 1200 ? Fonts::Regular : Fonts::Small;
	}
}
