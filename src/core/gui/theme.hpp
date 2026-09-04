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

#include <variant>
#include <type_traits>


namespace YLP::Frontend
{
	using namespace nlohmann;
	static inline json NLOHMANN_JSON_SERIALIZE_VEC2(const ImVec2& v)
	{
		return {
		    {"x", v.x},
		    {"y", v.y},
		};
	}

	static inline ImVec2 NLOHMANN_JSON_DESERIALIZE_VEC2(const json& j)
	{
		float x = j.value("x", 0.f);
		float y = j.value("y", 0.f);
		return ImVec2(x, y);
	}

	static inline json NLOHMANN_JSON_SERIALIZE_VEC4(const ImVec4& v)
	{
		return {
		    {"x", v.x},
		    {"y", v.y},
		    {"z", v.z},
		    {"w", v.w},
		};
	}

	static inline ImVec4 NLOHMANN_JSON_DESERIALIZE_VEC4(const json& j)
	{
		float x = j.value("x", 0.f);
		float y = j.value("y", 0.f);
		float z = j.value("z", 0.f);
		float w = j.value("w", 0.f);
		return ImVec4(x, y, z, w);
	}

	enum class eThemeStyleType : uint8_t
	{
		Float,
		Vec2
	};

	struct ThemeStyleVar
	{
		eThemeStyleType m_Type;
		std::function<void*(ImGuiStyle&)> GetStylePtr;
		const std::variant<float, ImVec2> FromJson(const json& value) const
		{
			switch (m_Type)
			{
			case eThemeStyleType::Float:
				return value.get<float>();

			case eThemeStyleType::Vec2:
				return NLOHMANN_JSON_DESERIALIZE_VEC2(value);
			}
			throw std::runtime_error("ThemeError: Unknown style type!");
		}
	};

	struct Theme
	{
		std::string m_Name;
		std::string m_AuthorName = "YLP";
		std::filesystem::path m_FilePath{}; // assigned each time a theme is loaded from file (only non default themes)
		std::unordered_map<std::string, ImVec4> m_Colors{};
		std::unordered_map<std::string, std::variant<float, ImVec2>> m_StyleVars{};
		bool m_IsDefault{true};

		static json SerializeStyle(const std::variant<float, ImVec2>& value)
		{
			return std::visit(
			    [](const auto& v) -> json {
				    using T = std::decay_t<decltype(v)>;
				    if constexpr (std::is_same_v<T, float>)
					    return v;
				    else
					    return NLOHMANN_JSON_SERIALIZE_VEC2(v);
			    },
			    value);
		}

		static void ApplyStyle(void* ptr, const std::variant<float, ImVec2>& value)
		{
			std::visit(
			    [ptr](const auto& v) {
				    using T = std::decay_t<decltype(v)>;
				    if constexpr (std::is_same_v<T, float>)
					    *static_cast<float*>(ptr) = v;
				    else if constexpr (std::is_same_v<T, ImVec2>)
					    *static_cast<ImVec2*>(ptr) = v;
			    },
			    value);
		}

		static inline const std::unordered_map<std::string, ImGuiCol> ImColMap =
		{
		    {"Text", ImGuiCol_Text},
		    {"TextDisabled", ImGuiCol_TextDisabled},
		    {"WindowBg", ImGuiCol_WindowBg},
		    {"ChildBg", ImGuiCol_ChildBg},
		    {"PopupBg", ImGuiCol_PopupBg},
		    {"Border", ImGuiCol_Border},
		    {"BorderShadow", ImGuiCol_BorderShadow},
		    {"FrameBg", ImGuiCol_FrameBg},
		    {"FrameBgHovered", ImGuiCol_FrameBgHovered},
		    {"FrameBgActive", ImGuiCol_FrameBgActive},
		    {"TitleBg", ImGuiCol_TitleBg},
		    {"TitleBgActive", ImGuiCol_TitleBgActive},
		    {"TitleBgCollapsed", ImGuiCol_TitleBgCollapsed},
		    {"MenuBarBg", ImGuiCol_MenuBarBg},
		    {"ScrollbarBg", ImGuiCol_ScrollbarBg},
		    {"ScrollbarGrab", ImGuiCol_ScrollbarGrab},
		    {"ScrollbarGrabHovered", ImGuiCol_ScrollbarGrabHovered},
		    {"ScrollbarGrabActive", ImGuiCol_ScrollbarGrabActive},
		    {"CheckMark", ImGuiCol_CheckMark},
		    {"SliderGrab", ImGuiCol_SliderGrab},
		    {"SliderGrabActive", ImGuiCol_SliderGrabActive},
		    {"Button", ImGuiCol_Button},
		    {"ButtonHovered", ImGuiCol_ButtonHovered},
		    {"ButtonActive", ImGuiCol_ButtonActive},
		    {"Header", ImGuiCol_Header},
		    {"HeaderHovered", ImGuiCol_HeaderHovered},
		    {"HeaderActive", ImGuiCol_HeaderActive},
		    {"Separator", ImGuiCol_Separator},
		    {"SeparatorHovered", ImGuiCol_SeparatorHovered},
		    {"SeparatorActive", ImGuiCol_SeparatorActive},
		    {"ResizeGrip", ImGuiCol_ResizeGrip},
		    {"ResizeGripHovered", ImGuiCol_ResizeGripHovered},
		    {"ResizeGripActive", ImGuiCol_ResizeGripActive},
		    {"InputTextCursor", ImGuiCol_InputTextCursor},
		    {"TabHovered", ImGuiCol_TabHovered},
		    {"Tab", ImGuiCol_Tab},
		    {"TabSelected", ImGuiCol_TabSelected},
		    {"TabSelectedOverline", ImGuiCol_TabSelectedOverline},
		    {"TabDimmed", ImGuiCol_TabDimmed},
		    {"TabDimmedSelected", ImGuiCol_TabDimmedSelected},
		    {"TabDimmedSelectedOverline", ImGuiCol_TabDimmedSelectedOverline},
		    {"PlotLines", ImGuiCol_PlotLines},
		    {"PlotLinesHovered", ImGuiCol_PlotLinesHovered},
		    {"PlotHistogram", ImGuiCol_PlotHistogram},
		    {"PlotHistogramHovered", ImGuiCol_PlotHistogramHovered},
		    {"TableHeaderBg", ImGuiCol_TableHeaderBg},
		    {"TableBorderStrong", ImGuiCol_TableBorderStrong},
		    {"TableBorderLight", ImGuiCol_TableBorderLight},
		    {"TableRowBg", ImGuiCol_TableRowBg},
		    {"TableRowBgAlt", ImGuiCol_TableRowBgAlt},
		    {"TextLink", ImGuiCol_TextLink},
		    {"TextSelectedBg", ImGuiCol_TextSelectedBg},
		    {"TreeLines", ImGuiCol_TreeLines},
		    {"DragDropTarget", ImGuiCol_DragDropTarget},
		    {"UnsavedMarker", ImGuiCol_UnsavedMarker},
		    {"NavCursor", ImGuiCol_NavCursor},
		    {"NavWindowingHighlight", ImGuiCol_NavWindowingHighlight},
		    {"NavWindowingDimBg", ImGuiCol_NavWindowingDimBg},
		    {"ModalWindowDimBg", ImGuiCol_ModalWindowDimBg},
		};

		static inline const std::unordered_map<std::string, ThemeStyleVar> StyleResolvers =
		{
		    {
		        "WindowRounding",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.WindowRounding;
		            }
				}
		    },
			{
		        "ChildRounding",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.ChildRounding;
		            }
				}
		    },
		    {
		        "FrameRounding",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.FrameRounding;
		            }
				}
		    },
		    {
		        "PopupRounding",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.PopupRounding;
		            }
				}
		    },
		    {
		        "ScrollbarRounding",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.ScrollbarRounding;
		            }
				}
		    },
		    {
		        "GrabRounding",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.GrabRounding;
		            }
				}
		    },
		    {
		        "TabRounding",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.TabRounding;
		            }
				}
		    },
		    {
		        "WindowBorderSize",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.WindowBorderSize;
		            }
				}
		    },
		    {
		        "ChildBorderSize",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.ChildBorderSize;
		            }
				}
		    },
		    {
		        "PopupBorderSize",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.PopupBorderSize;
		            }
				}
		    },
		    {
		        "FrameBorderSize",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.FrameBorderSize;
		            }
				}
		    },
		    {
		        "TabBorderSize",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.TabBorderSize;
		            }
				}
		    },
		    {
		        "IndentSpacing",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.IndentSpacing;
		            }
				}
		    },
		    {
		        "ScrollbarSize",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.ScrollbarSize;
		            }
				}
		    },
		    {
		        "GrabMinSize",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.GrabMinSize;
		            }
				}
		    },
		    {
		        "ScrollbarPadding",
		        {
					eThemeStyleType::Float,
		            [](ImGuiStyle& style) -> void* {
			            return &style.ScrollbarPadding;
		            }
				}
		    },
		    {
		        "WindowPadding",
		        {
					eThemeStyleType::Vec2,
		            [](ImGuiStyle& style) -> void* {
			            return &style.WindowPadding;
		            }
				}
		    },
		    {
		        "FramePadding",
		        {
					eThemeStyleType::Vec2,
		            [](ImGuiStyle& style) -> void* {
			            return &style.FramePadding;
		            }
				}
		    },
		    {
		        "CellPadding",
		        {
					eThemeStyleType::Vec2,
		            [](ImGuiStyle& style) -> void* {
			            return &style.CellPadding;
		            }
				}
		    },
		    {
		        "ItemSpacing",
		        {
					eThemeStyleType::Vec2,
		            [](ImGuiStyle& style) -> void* {
			            return &style.ItemSpacing;
		            }
				}
		    },
		    {
		        "ItemInnerSpacing",
		        {
					eThemeStyleType::Vec2,
		            [](ImGuiStyle& style) -> void* {
			            return &style.ItemInnerSpacing;
		            }
				}
		    },
		};

		[[nodiscard]] const bool Apply() const
		{
			if (!GImGui)
			{
				LOG_ERROR("Attempt to apply themes without a valid ImGui context!");
				return false;
			}

			try
			{
				ImGuiStyle style = ImGuiStyle();
				auto& styleVars = m_StyleVars;
				for (auto& [name, value] : styleVars)
				{
					auto it = StyleResolvers.find(name);
					if (it == StyleResolvers.end())
						continue;

					auto stylePtr = it->second.GetStylePtr(style);
					if (!stylePtr)
						continue;

					ApplyStyle(stylePtr, value);
				}

				for (auto& [name, color] : m_Colors)
				{
					auto it = ImColMap.find(name);
					if (it != ImColMap.end())
						style.Colors[it->second] = color;
				}
				ImGui::GetStyle() = style;
				return true;
			}
			catch (const std::exception& e)
			{
				LOG_ERROR("Failed to apply theme! {}", e.what());
				return false;
			}
		}
	};

	inline void from_json(const json& j, Theme& t)
	{
		t.m_Name = j.value("name", "");
		t.m_AuthorName = j.value("author", "YLP");
		t.m_IsDefault = false;
		auto& styleVars = t.m_StyleVars;
		auto& styleColors = t.m_Colors;
		const json& styleVarsJson = j["style_vars"];
		auto& StyleResolvers = Theme::StyleResolvers;
		if (!styleVarsJson.is_null() && styleVarsJson.is_object())
		{
			for (const auto& [k, v] : styleVarsJson.items())
			{
				if (auto it = StyleResolvers.find(k); it != StyleResolvers.end())
					styleVars[k] = it->second.FromJson(v);
			}
		}

		const json& styleColsJson = j["style_colors"];
		auto& ImColMap = Theme::ImColMap;
		if (!styleColsJson.is_null() && styleColsJson.is_object())
		{
			for (const auto& [k, v] : styleColsJson.items())
			{
				if (auto it = ImColMap.find(k); it == ImColMap.end())
					continue;

				styleColors[k] = NLOHMANN_JSON_DESERIALIZE_VEC4(v);
			}
		}
	}

	inline void to_json(json& j, const Theme& t)
	{
		j["name"] = t.m_Name;
		j["author"] = t.m_AuthorName;
		auto& styleVars = t.m_StyleVars;
		auto& colors = t.m_Colors;
		auto& StyleResolvers = Theme::StyleResolvers;
		for (auto& [k, v] : styleVars)
		{
			if (auto it = StyleResolvers.find(k); it == StyleResolvers.end())
				continue;

			j["style_vars"][k] = t.SerializeStyle(v);
		}

		for (auto& [k, v] : Theme::ImColMap)
		{
			if (auto it = colors.find(k); it != colors.end())
				j["style_colors"][k] = NLOHMANN_JSON_SERIALIZE_VEC4(it->second);
		}
	}
}
