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
#include "../wrappers/imdrawlist.hpp"
#include "../../gui/widgets/ylp_spinner.hpp"


namespace YLP::LuaJIT
{
	class LuaImGuiLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	private:
		struct SolStrArray
		{
			std::vector<std::string> strings;
			std::vector<const char*> chars;
		};

		static inline SolStrArray SolTbl2StrArr(const sol::table& t)
		{
			SolStrArray result;
			result.strings.reserve(t.size());
			for (auto&& [i, v] : t)
			{
				if (!v.is<std::string>())
					continue;

				result.strings.emplace_back(v.as<std::string>());
			}

			result.chars.reserve(result.strings.size());
			for (const auto& str : result.strings)
				result.chars.emplace_back(str.c_str());

			return result;
		}

		static inline std::tuple<std::string, bool> InputTextMultiline(const std::string& label, std::string text, unsigned int buffSize)
		{
			text.resize(buffSize);
			bool selected = ImGui::InputTextMultiline(label.c_str(), text.data(), buffSize);
			return std::make_tuple(text.c_str(), selected);
		}

		static inline std::tuple<std::string, bool> InputTextMultiline1(const std::string& label, std::string text, unsigned int buffSize, ImVec2 size, sol::optional<int> flags)
		{
			text.resize(buffSize);
			auto inputFlags = static_cast<ImGuiInputTextFlags>(flags.value_or(0));
			bool selected   = ImGui::InputTextMultiline(label.c_str(), text.data(), buffSize, size, inputFlags);
			return std::make_tuple(text.c_str(), selected);
		}

		static inline bool BeginChild(const std::string& name)
		{
			return ImGui::BeginChild(name.c_str());
		}

		static inline bool BeginChild1(const std::string& name, ImVec2 size)
		{
			return ImGui::BeginChild(name.c_str(), size);
		}

		static inline bool BeginChild2(const std::string& name, ImVec2 size, int childFlags, sol::optional<int> windowFlags)
		{
			auto winFlags = static_cast<ImGuiWindowFlags>(windowFlags.value_or(0));
			return ImGui::BeginChild(name.c_str(), size, static_cast<ImGuiChildFlags>(childFlags), winFlags);
		}

	public:
		void Register(sol::state& L) override
		{
			if (auto mod = GetModuleFromLuaState(L); mod->GetName() == "CodeExecutor")
				return;

			/*@ylp.table ImGui
			* description: 
				A very basic API that exposes a few ImGui widgets.
			@*/
			auto imguiTable = L["ImGui"].get_or_create<sol::table>();

			/*@ylp.function ImGui.PushID
			* param id<integer>
			@*/
			imguiTable["PushID"] = [](int id) {
				ImGui::PushID(id);
			};

			/*@ylp.function ImGui.PopID
			*
			@*/
			imguiTable["PopID"] = []() {
				ImGui::PopID();
			};

			/*@ylp.function ImGui.PushFont
			* param font<userdata> Use the global [Fonts](lua://Fonts) table to access fonts.
			@*/
			imguiTable["PushFont"] = [](ImFont* font) {
				ImGui::PushFont(font);
			};

			/*@ylp.function ImGui.PopFont
			*
			@*/
			imguiTable["PopFont"] = []() {
				ImGui::PopFont();
			};

			/*@ylp.function ImGui.Bullet
			* 
			@*/
			imguiTable["Bullet"] = []() {
				ImGui::Bullet();
			};

			/*@ylp.function ImGui.Text
			* param text<string> 
			* param ...<any> Optional string format arguments
			@*/
			imguiTable["Text"] = [&](const std::string& fmt, sol::variadic_args args) {
				ImGui::Text(LuaStringFormat(L, fmt, args).c_str());
			};

			/*@ylp.function ImGui.TextCentered Draws a text centered at either the current available region's width or the specified optional width.
			* param text<string> 
			* param font<userdata?> Optional font. Use the global [Fonts](lua://Fonts) table to access fonts.
			* param customWidth<number?> Optional width to center at.
			@*/
			imguiTable["TextCentered"] = sol::overload(
			    [](const std::string& text) {
				    ImGui::TextCentered(text.c_str());
			    },
			    [](const std::string& text, ImFont* font, sol::optional<float> customWidth) {
				    ImGui::TextCentered(text.c_str(), font, customWidth.value_or(0.0f));
			    });

			/*@ylp.function ImGui.TextDisabled
			* param text<string>
			* param ...<any> Optional string format arguments
			@*/
			imguiTable["TextDisabled"] = [&](const std::string& fmt, sol::variadic_args args) {
				ImGui::TextDisabled(LuaStringFormat(L, fmt, args).c_str());
			};

			/*@ylp.function ImGui.TextWrapped
			* param text<string>
			* param ...<any> Optional string format arguments
			@*/
			imguiTable["TextWrapped"] = [&](const std::string& fmt, sol::variadic_args args) {
				ImGui::TextWrapped(LuaStringFormat(L, fmt, args).c_str());
			};

			/*@ylp.function ImGui.TextUnformatted
			* param text<string> 
			@*/
			imguiTable["TextUnformatted"] = [](const std::string& text) {
				ImGui::TextUnformatted(text.c_str());
			};

			/*@ylp.function ImGui.TextColored
			* param r<number> A number between `0.0` and `1.0` representing the `Red` component of the color.
			* param g<number> A number between `0.0` and `1.0` representing the `Green` component of the color.
			* param b<number> A number between `0.0` and `1.0` representing the `Blue` component of the color.
			* param a<number> A number between `0.0` and `1.0` representing the `Alpha` component of the color.
			* param text<string>
			* param ...<any> Optional string format arguments
			@*/
			imguiTable["TextColored"] = [&](float r, float g, float b, float a, const std::string& fmt, sol::variadic_args args) {
				ImGui::TextColored(ImVec4(r, g, b, a), LuaStringFormat(L, fmt, args).c_str());
			};

			/*@ylp.function ImGui.TitleText Draws a large and bold text.
			* param text<string>
			* param separator<boolean?> Optional: Draw a SeparatorText instead.
			@*/
			imguiTable["TitleText"] = [&](const std::string& text, sol::optional<bool> separator) {
				ImGui::TitleText(text.c_str(), separator.value_or(false));
			};

			/*@ylp.function ImGui.BulletText
			* param text<string>
			* param ...<any> Optional string format arguments
			@*/
			imguiTable["BulletText"] = [&](const std::string& fmt, sol::variadic_args args) {
				ImGui::BulletText(LuaStringFormat(L, fmt, args).c_str());
			};

			/*@ylp.function ImGui.Button
			* param label<string> 
			* param size<ImVec2?> Optional button size
			* return boolean clicked
			@*/
			imguiTable["Button"] = [](const std::string& label, sol::optional<ImVec2> size) {
				return ImGui::Button(label.c_str(), size.value_or(ImVec2()));
			};

			/*@ylp.function ImGui.SmallButton
			* param label<string>
			* return boolean clicked
			@*/
			imguiTable["SmallButton"] = [](const std::string& label) {
				return ImGui::SmallButton(label.c_str());
			};

			/*@ylp.function ImGui.ArrowButton
			* param label<string> 
			* param direction<integer> [ImGuiDir](lua://ImGuiDir)
			* return boolean clicked
			@*/
			imguiTable["ArrowButton"] = [](const std::string& label, int direction) {
				return ImGui::ArrowButton(label.c_str(), static_cast<ImGuiDir>(direction));
			};

			/*@ylp.function ImGui.InvisibleButton
			* param label<string> 
			* param size<ImVec2>
			* return boolean clicked
			@*/
			imguiTable["InvisibleButton"] = [](const std::string& label, const ImVec2& size) {
				return ImGui::InvisibleButton(label.c_str(), size);
			};

			/*@ylp.function ImGui.Checkbox
			* param label<string> 
			* param v<boolean>
			* return boolean value
			* return boolean clicked
			@*/
			imguiTable["Checkbox"] = [](const std::string& label, bool v) {
				bool value{v};
				bool clicked = ImGui::Checkbox(label.c_str(), &value);
				return std::make_tuple(value, clicked);
			};

			/*@ylp.function ImGui.RadioButton
			* param label<string> 
			* param active<boolean>
			* return boolean clicked
			@*/

			/*@ylp.function ImGui.RadioButton
			* param label<string> 
			* param v<integer> Current index
			* param vButton<integer> Index to change to
			* return integer newValue
			* return boolean clicked
			@*/
			imguiTable["RadioButton"] = sol::overload(
			    [](const std::string& label, bool active) {
				    return ImGui::RadioButton(label.c_str(), active);
			    },
			    [](const std::string& label, int v, int vButton) {
				    bool ret = ImGui::RadioButton(label.c_str(), &v, vButton);
				    return std::make_tuple(v, ret);
			    });

			/*@ylp.function ImGui.ProgressBar
			* param fraction<number> 
			* param size<ImVec2?> Optional frame size
			* param overlayText<string?> Optional overlay text. Defaults to precentage.
			@*/
			imguiTable["ProgressBar"] = sol::overload(
			    [](float fraction) {
				    ImGui::ProgressBar(fraction);
			    },
			    [](float fraction, ImVec2 size, sol::optional<std::string> overlay) {
				    ImGui::ProgressBar(fraction, size, overlay.value_or("").c_str());
			    });

			/*@ylp.function ImGui.Combo
			* param label<string>
			* param currentItem<integer>
			* param itemsSeparatedByZeros<string> A string with items separated by null terminators. Ex: `"Item1\0Item2\0Item3"`
			* return integer currentItem
			* return boolean opened
			@*/

			/*@ylp.function ImGui.Combo
			* param label<string>
			* param currentItem<integer>
			* param items<{[integer]: string}> A table of strings. Must be a contiguous string array, anything else will be ignored.
			* return integer currentItem
			* return boolean opened
			@*/
			imguiTable["Combo"] = sol::overload(
			    [](const std::string& label, int currentItem, const std::string& itemsSeparatedByZeros) {
				    bool clicked = ImGui::Combo(label.c_str(), &currentItem, itemsSeparatedByZeros.c_str());
				    return std::make_tuple(currentItem, clicked);
			    },
			    [](const std::string& label, int currentItem, const sol::table& items) {
				    auto res     = SolTbl2StrArr(items);
				    bool clicked = ImGui::Combo(label.c_str(), &currentItem, res.chars.data(), static_cast<int>(res.chars.size()));
				    return std::make_tuple(currentItem, clicked);
			    });

			/*@ylp.function ImGui.BeginCombo
			* param label<string> 
			* param preview<string> 
			* param flags<integer?> Optional [ImGuiComboFlags](lua://ImGuiComboFlags)
			* return boolean opened
			@*/
			imguiTable["BeginCombo"] = [](const std::string& label, const std::string& previewValue, sol::optional<int> flags) {
				return ImGui::BeginCombo(label.c_str(), previewValue.c_str(), static_cast<ImGuiComboFlags>(flags.value_or(0)));
			};

			/*@ylp.function ImGui.EndCombo
			* 
			@*/
			imguiTable["EndCombo"] = []() {
				ImGui::EndCombo();
			};

			/*@ylp.function ImGui.ListBox
			* param label<string>
			* param currentItem<integer>
			* param items<{[integer]: string}> A table of strings. Must be a contiguous string array, anything else will be ignored.
			* param heightInItems<integer?> Optional max items to show. Defaults to -1
			* return integer currentItem
			* return boolean visible
			@*/
			imguiTable["ListBox"] = [](const std::string& label, int currentItem, const sol::table& items, sol::optional<int> heightInItems) {
				auto res     = SolTbl2StrArr(items);
				bool visible = ImGui::ListBox(label.c_str(),
				    &currentItem,
				    res.chars.data(),
				    static_cast<int>(res.chars.size()),
				    heightInItems.value_or(-1));

				return std::make_tuple(currentItem, visible);
			};

			/*@ylp.function ImGui.BeginListBox
			* param label<string> 
			* param size<ImVec2?> Optional listbox size
			* return boolean visible
			@*/
			imguiTable["BeginListBox"] = [](const std::string& label, sol::optional<ImVec2> size) {
				return ImGui::BeginListBox(label.c_str(), size.value_or(ImVec2()));
			};

			/*@ylp.function ImGui.EndListBox
			* 
			@*/
			imguiTable["EndListBox"] = []() {
				ImGui::EndListBox();
			};

			/*@ylp.function ImGui.Selectable
			* param label<string> 
			* param selected<boolean>
			* param flags<integer?> Optional [ImGuiSelectableFlags](lua://ImGuiSelectableFlags)
			* param size<ImVec2?> Optional selectable size
			* return boolean clicked
			@*/
			imguiTable["Selectable"] = sol::overload(
			    [](const std::string& label, bool selected) {
				    return ImGui::Selectable(label.c_str(), selected);
			    },
			    [](const std::string& label, bool selected, int flags, sol::optional<ImVec2> size) {
				    auto selectableFlags = static_cast<ImGuiSelectableFlags>(flags);
				    return ImGui::Selectable(label.c_str(), selected, selectableFlags, size.value_or(ImVec2()));
			    });

			/*@ylp.function ImGui.MenuItem
			* param label<string> 
			* param selected<boolean>
			* return boolean selected
			* return boolean clicked
			@*/
			imguiTable["MenuItem"] = [](const std::string& label, bool selected) {
				bool clicked = ImGui::MenuItem(label.c_str(), nullptr, &selected);
				return std::make_tuple(selected, clicked);
			};

			/*@ylp.function ImGui.BeginDisabled Disables all ImGui widgets created between this call and EndDisabled
			* param cond<boolean?> Optional condition
			@*/
			imguiTable["BeginDisabled"] = [](sol::optional<bool> bCond) {
				ImGui::BeginDisabled(bCond.value_or(true));
			};

			/*@ylp.function ImGui.EndDisabled
			*
			@*/
			imguiTable["EndDisabled"] = []() {
				ImGui::EndDisabled();
			};

			/*@ylp.function ImGui.BeginChild
			* param name<string>
			* return boolean visible
			@*/

			/*@ylp.function ImGui.BeginChild
			* param name<string>
			* param size<ImVec2> child size.
			* return boolean visible
			@*/

			/*@ylp.function ImGui.BeginChild
			* param name<string>
			* param size<ImVec2> child size.
			* param childFlags<integer> [ImGuiChildFlags](lua://ImGuiChildFlags)
			* param windowFlags<integer?> Optional [ImGuiWindowFlags](lua://ImGuiWindowFlags)
			* return boolean visible
			@*/
			imguiTable["BeginChild"] = sol::overload(&BeginChild, &BeginChild1, &BeginChild2);

			/*@ylp.function ImGui.EndChild
			* 
			@*/
			imguiTable["EndChild"] = []() {
				ImGui::EndChild();
			};

			/*@ylp.function ImGui.OpenPopup
			* param name<string> Popup name
			* param flags<integer?> Optional [ImGuiPopupFlags](lua://ImGuiPopupFlags)
			@*/
			imguiTable["OpenPopup"] = [](const std::string& name, sol::optional<int> popupFlags) {
				ImGui::OpenPopup(name.c_str(), static_cast<ImGuiPopupFlags>(popupFlags.value_or(0)));
			};

			/*@ylp.function ImGui.IsPopupOpen
			* param name<string> Popup name
			* param flags<integer?> Optional [ImGuiPopupFlags](lua://ImGuiPopupFlags)
			* return boolean
			@*/
			imguiTable["IsPopupOpen"] = [](const std::string& name, sol::optional<int> popupFlags) {
				return ImGui::IsPopupOpen(name.c_str(), static_cast<ImGuiPopupFlags>(popupFlags.value_or(0)));
			};

			/*@ylp.function ImGui.CloseCurrentPopup
			* 
			@*/
			imguiTable["CloseCurrentPopup"] = []() {
				ImGui::CloseCurrentPopup();
			};

			/*@ylp.function ImGui.BeginPopup
			* param name<string>
			* param windowFlags<integer?> Optional [ImGuiWindowFlags](lua://ImGuiWindowFlags)
			* return boolean isOpen
			@*/
			imguiTable["BeginPopup"] = [](const std::string& name, sol::optional<int> windowFlags) {
				return ImGui::BeginPopup(name.c_str(), static_cast<ImGuiWindowFlags>(windowFlags.value_or(0)));
			};

			/*@ylp.function ImGui.BeginPopupModal
			* param name<string>
			* param windowFlags<integer?> Optional [ImGuiWindowFlags](lua://ImGuiWindowFlags)
			* return boolean visible
			@*/

			/*@ylp.function ImGui.BeginPopupModal
			* param name<string>
			* param open<boolean> When true, the popup will have a `(x)` close button in the title bar.
			* param windowFlags<integer?> Optional [ImGuiWindowFlags](lua://ImGuiWindowFlags)
			* return boolean visible
			* return boolean open
			@*/
			imguiTable["BeginPopupModal"] = sol::overload(
			    [](const std::string& name, sol::optional<int> windowFlags) {
				    return ImGui::BeginPopupModal(name.c_str(), nullptr, static_cast<ImGuiWindowFlags>(windowFlags.value_or(0)));
			    },
			    [](const std::string& name, bool open, sol::optional<int> windowFlags) {
				    bool visible = ImGui::BeginPopupModal(name.c_str(), &open, static_cast<ImGuiWindowFlags>(windowFlags.value_or(0)));
				    return std::make_tuple(visible, open);
			    });

			/*@ylp.function ImGui.EndPopup
			* 
			@*/
			imguiTable["EndPopup"] = []() {
				ImGui::EndPopup();
			};

			/*@ylp.function ImGui.SliderInt
			* param label<string>
			* param v<integer>
			* param vMin<integer> Minimum value
			* param vMax<integer> Maximum value
			* return integer v
			* return boolean changed
			@*/

			/*@ylp.function ImGui.SliderInt
			* param label<string>
			* param v<integer>
			* param vMin<integer> Minimum value
			* param vMax<integer> Maximum value
			* param fmt<string> format string
			* param flags<integer?> Optional [ImGuiSliderFlags](lua://ImGuiSliderFlags)
			* return integer v
			* return boolean changed
			@*/
			imguiTable["SliderInt"] = sol::overload(
			    [](const std::string& label, int v, int vMin, int vMax) {
				    bool changed = ImGui::SliderInt(label.c_str(), &v, vMin, vMax);
				    return std::make_tuple(v, changed);
			    },
			    [](const std::string& label, int v, int vMin, int vMax, const std::string& fmt, sol::optional<int> flags) {
				    auto sliderFlags = static_cast<ImGuiSliderFlags>(flags.value_or(0));
				    bool changed     = ImGui::SliderInt(label.c_str(), &v, vMin, vMax, fmt.c_str(), sliderFlags);
				    return std::make_tuple(v, changed);
			    });

			/*@ylp.function ImGui.SliderFloat
			* param label<string>
			* param v<number>
			* param vMin<number> Minimum value
			* param vMax<number> Maximum value
			* return number v
			* return boolean changed
			@*/

			/*@ylp.function ImGui.SliderFloat
			* param label<string>
			* param v<number>
			* param vMin<number> Minimum value
			* param vMax<number> Maximum value
			* param fmt<string> format string
			* param flags<integer?> Optional [ImGuiSliderFlags](lua://ImGuiSliderFlags)
			* return number v
			* return boolean changed
			@*/
			imguiTable["SliderFloat"] = sol::overload(
			    [](const std::string& label, float v, float vMin, float vMax) {
				    bool changed = ImGui::SliderFloat(label.c_str(), &v, vMin, vMax);
				    return std::make_tuple(v, changed);
			    },
			    [](const std::string& label, float v, float vMin, float vMax, const std::string& fmt, sol::optional<int> flags) {
				    auto sliderFlags = static_cast<ImGuiSliderFlags>(flags.value_or(0));
				    bool changed     = ImGui::SliderFloat(label.c_str(), &v, vMin, vMax, fmt.c_str(), sliderFlags);
				    return std::make_tuple(v, changed);
			    });

			/*@ylp.function ImGui.InputInt
			* param label<string>
			* param v<integer>
			* return integer v
			* return boolean changed
			@*/

			/*@ylp.function ImGui.InputInt
			* param label<string>
			* param v<integer>
			* param step<integer> step
			* param stepFast<integer> fast step
			* param flags<integer?> Optional [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
			* return integer v
			* return boolean changed
			@*/
			imguiTable["InputInt"] = sol::overload(
			    [](const std::string& label, int v, sol::optional<int> step) {
				    bool changed = ImGui::InputInt(label.c_str(), &v);
				    return std::make_tuple(v, changed);
			    },
			    [](const std::string& label, int v, int step, int stepFast, sol::optional<int> flags) {
				    bool changed = ImGui::InputInt(label.c_str(), &v, step, stepFast, static_cast<ImGuiInputTextFlags>(flags.value_or(0)));
				    return std::make_tuple(v, changed);
			    });

			/*@ylp.function ImGui.InputFloat
			* param label<string>
			* param v<number>
			* return number v
			* return boolean changed
			@*/

			/*@ylp.function ImGui.InputFloat
			* param label<string>
			* param v<number>
			* param step<number> step
			* param stepFast<number> fast step
			* param fmt<string?> Optional format string
			* return number v
			* return boolean changed
			@*/

			/*@ylp.function ImGui.InputFloat
			* param label<string>
			* param v<number>
			* param step<number> step
			* param stepFast<number> fast step
			* param fmt<string> format string
			* param flags<integer?> Optional [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
			* return number v
			* return boolean changed
			@*/
			imguiTable["InputFloat"] = sol::overload(
			    [](const std::string& label, float v) {
				    bool changed = ImGui::InputFloat(label.c_str(), &v);
				    return std::make_tuple(v, changed);
			    },
			    [](const std::string& label, float v, int step, int stepFast, sol::optional<std::string> fmt) {
				    bool changed = ImGui::InputFloat(label.c_str(), &v, step, stepFast, fmt.value_or("%.3f").c_str());
				    return std::make_tuple(v, changed);
			    },
			    [](const std::string& label, float v, int step, int stepFast, const std::string& fmt, sol::optional<int> flags) {
				    auto inputFlags = static_cast<ImGuiInputTextFlags>(flags.value_or(0));
				    bool changed    = ImGui::InputFloat(label.c_str(), &v, step, stepFast, fmt.c_str(), inputFlags);
				    return std::make_tuple(v, changed);
			    });

			/*@ylp.function ImGui.InputText
			* param label<string>
			* param text<string>
			* param bufferSize<integer>
			* param flags<integer?> Optional [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
			* return string text
			* return boolean changed
			@*/
			imguiTable["InputText"] = [](const std::string& label, std::string text, uint32_t bufferSize, sol::optional<int> flags) {
				text.resize(bufferSize);
				bool changed = ImGui::InputText(label.c_str(), text.data(), bufferSize, static_cast<ImGuiInputTextFlags>(flags.value_or(0)));
				return std::make_tuple(text.c_str(), changed);
			};

			/*@ylp.function ImGui.InputTextWithHint
			* param label<string>
			* param hint<string>
			* param text<string>
			* param bufferSize<integer>
			* param flags<integer?> Optional [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
			* return string text
			* return boolean changed
			@*/
			imguiTable["InputTextWithHint"] = [](const std::string& label,
			                                      const std::string& hint,
			                                      std::string text,
			                                      uint32_t bufferSize,
			                                      sol::optional<int>
			                                          flags) {
				text.resize(bufferSize);
				bool changed = ImGui::InputTextWithHint(label.c_str(),
				    hint.c_str(),
				    text.data(),
				    bufferSize,
				    static_cast<ImGuiInputTextFlags>(flags.value_or(0)));

				return std::make_tuple(text.c_str(), changed);
			};

			/*@ylp.function ImGui.InputTextMultiline
			* param label<string>
			* param text<string>
			* param bufferSize<integer>
			* return string text
			* return boolean changed
			@*/

			/*@ylp.function ImGui.InputTextMultiline
			* param label<string>
			* param text<string>
			* param bufferSize<integer>
			* param flags<integer> [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
			* param size<ImVec2?> Optional widget size
			* return string text
			* return boolean changed
			@*/
			imguiTable["InputTextMultiline"] = sol::overload(&InputTextMultiline, &InputTextMultiline1);

			// layout stuff

			/*@ylp.function ImGui.SameLine
			*
			@*/

			/*@ylp.function ImGui.SameLine
			* param offsetX<number> X position to start at. Defaults to `0`.
			* param spacing<number?> Optional spacing after the previous item. Defaults to the current item spacing.
			@*/
			imguiTable["SameLine"] = sol::overload(
			    []() { ImGui::SameLine(); },
			    [](sol::optional<float> offsetX) {
				    ImGui::SameLine(offsetX.value_or(0.0f));
			    },
			    [](float offsetX, float spacing) {
				    ImGui::SameLine(offsetX, spacing);
			    });

			/*@ylp.function ImGui.NewLine
			* 
			@*/
			imguiTable["NewLine"] = []() {
				ImGui::NewLine();
			};

			/*@ylp.function ImGui.Spacing
			* 
			@*/
			imguiTable["Spacing"] = []() {
				ImGui::Spacing();
			};

			/*@ylp.function ImGui.Separator
			* 
			@*/
			imguiTable["Separator"] = []() {
				ImGui::Separator();
			};

			/*@ylp.function ImGui.SeparatorEx
			* param flags<integer> [ImGuiSeparatorFlags](lua://ImGuiSeparatorFlags)
			* param thickness<number?> Optional separator thickness. Defaults to 1.0
			@*/
			imguiTable["SeparatorEx"] = [](int flags, sol::optional<float> thickness) {
				ImGui::SeparatorEx(static_cast<ImGuiSeparatorFlags>(flags), thickness.value_or(1.0f));
			};

			/*@ylp.function ImGui.SeparatorText
			* param text<string>
			@*/
			imguiTable["SeparatorText"] = [](const std::string& text) {
				ImGui::SeparatorText(text.c_str());
			};

			/*@ylp.function ImGui.Indent
			* param indentX<number?> Optional indentation width. Defaults to the current item spacing.
			@*/
			imguiTable["Indent"] = [](sol::optional<float> indentX) {
				ImGui::Indent(indentX.value_or(0.f));
			};

			/*@ylp.function ImGui.Unindent
			* param indentX<number?> Optional indentation width. Defaults to the current item spacing.
			@*/
			imguiTable["Unindent"] = [](sol::optional<float> indentX) {
				ImGui::Unindent(indentX.value_or(0.f));
			};

			/*@ylp.function ImGui.Dummy
			* param sizeX<number> Width of the dummy item.
			* param sizeY<number> Height of the dummy item.
			@*/
			imguiTable["Dummy"] = [](float sizeX, float sizeY) {
				ImGui::Dummy(ImVec2(sizeX, sizeY));
			};

			/*@ylp.function ImGui.SetCursorPos
			* param x<number> X position relative to the current window.
			* param y<number> Y position relative to the current window.
			@*/
			imguiTable["SetCursorPos"] = [](float x, float y) {
				ImGui::SetCursorPos(ImVec2(x, y));
			};

			/*@ylp.function ImGui.SetCursorPosX
			* param x<number> X position relative to the current window.
			@*/
			imguiTable["SetCursorPosX"] = [](float x) {
				ImGui::SetCursorPosX(x);
			};

			/*@ylp.function ImGui.SetCursorPosY
			* param y<number> Y position relative to the current window.
			@*/
			imguiTable["SetCursorPosY"] = [](float y) {
				ImGui::SetCursorPosY(y);
			};

			/*@ylp.function ImGui.GetCursorPos
			* return ImVec2 cursorPos
			@*/
			imguiTable["GetCursorPos"] = []() {
				return ImGui::GetCursorPos();
			};

			/*@ylp.function ImGui.GetCursorPosX
			* return number x Current X position relative to the current window.
			@*/
			imguiTable["GetCursorPosX"] = []() {
				return ImGui::GetCursorPosX();
			};

			/*@ylp.function ImGui.GetCursorPosY
			* return number y Current Y position relative to the current window.
			@*/
			imguiTable["GetCursorPosY"] = []() {
				return ImGui::GetCursorPosY();
			};

			/*@ylp.function ImGui.GetContentRegionAvail
			* return ImVec2 avail Available content region.
			@*/
			imguiTable["GetContentRegionAvail"] = []() {
				return ImGui::GetContentRegionAvail();
			};

			/*@ylp.function ImGui.GetWindowSize
			* return ImVec2 windowSize
			@*/
			imguiTable["GetWindowSize"] = []() {
				return ImGui::GetWindowSize();
			};

			/*@ylp.function ImGui.GetWindowPos
			* return ImVec2 windowPos Current window position.
			@*/
			imguiTable["GetWindowPos"] = []() {
				return ImGui::GetWindowPos();
			};

			/*@ylp.function ImGui.GetWindowWidth
			* return number width Current window width
			@*/
			imguiTable["GetWindowWidth"] = []() {
				return ImGui::GetWindowWidth();
			};

			/*@ylp.function ImGui.GetWindowHeight
			* return number height Current window height
			@*/
			imguiTable["GetWindowHeight"] = []() {
				return ImGui::GetWindowHeight();
			};

			/*@ylp.function ImGui.GetWindowDrawList
			* return ImDrawList
			@*/
			imguiTable["GetWindowDrawList"] = []() {
				return LuaImDrawList(ImGui::GetWindowDrawList());
			};

			/*@ylp.function ImGui.GetBackgroundDrawList
			* return ImDrawList
			@*/
			imguiTable["GetBackgroundDrawList"] = []() {
				return LuaImDrawList(ImGui::GetBackgroundDrawList());
			};

			/*@ylp.function ImGui.GetForegroundDrawList
			* return ImDrawList
			@*/
			imguiTable["GetForegroundDrawList"] = []() {
				return LuaImDrawList(ImGui::GetForegroundDrawList());
			};

			/*@ylp.function ImGui.GetFrameHeight
			* return number frameHeight
			@*/
			imguiTable["GetFrameHeight"] = []() {
				return ImGui::GetFrameHeight();
			};

			/*@ylp.function ImGui.GetFrameHeightWithSpacing
			* return number frameHeight Frame height with spacing.
			@*/
			imguiTable["GetFrameHeightWithSpacing"] = []() {
				return ImGui::GetFrameHeightWithSpacing();
			};

			/*@ylp.function ImGui.GetTime
			* return number frameTime
			@*/
			imguiTable["GetTime"] = []() {
				return ImGui::GetTime();
			};

			/*@ylp.function ImGui.SetWindowSize
			* param size<ImVec2> Window size
			* param cond<integer?> Optional [ImGuiCond](lua://ImGuiCond)
			@*/

			/*@ylp.function ImGui.SetWindowSize
			* param name<string> Window name
			* param size<ImVec2> Window size
			* param cond<integer?> Optional [ImGuiCond](lua://ImGuiCond)
			@*/
			imguiTable["SetWindowSize"] = sol::overload(
				[](const ImVec2& size, sol::optional<int> cond) {
					return ImGui::SetWindowSize(size, static_cast<ImGuiCond>(cond.value_or(0)));
				},
				[](const std::string& name, const ImVec2& size, sol::optional<int> cond) {
				    return ImGui::SetWindowSize(name.c_str(), size, static_cast<ImGuiCond>(cond.value_or(0)));
				}
			);

			/*@ylp.function ImGui.SetWindowPos
			* param pos<ImVec2> New position
			* param cond<integer?> Optional [ImGuiCond](lua://ImGuiCond)
			@*/

			/*@ylp.function ImGui.SetWindowPos
			* param name<string> Window name
			* param pos<ImVec2> New position
			* param cond<integer?> Optional [ImGuiCond](lua://ImGuiCond)
			@*/
			imguiTable["SetWindowPos"] = sol::overload(
			    [](const ImVec2& pos, sol::optional<int> cond) {
				    return ImGui::SetWindowPos(pos, static_cast<ImGuiCond>(cond.value_or(0)));
				},
			    [](const std::string& name, const ImVec2& pos, sol::optional<int> cond) {
				    return ImGui::SetWindowPos(name.c_str(), pos, static_cast<ImGuiCond>(cond.value_or(0)));
				}
			);

			/*@ylp.function ImGui.SetNextWindowSize
			* param pos<ImVec2>
			* param cond<integer?> Optional [ImGuiCond](lua://ImGuiCond)
			@*/
			imguiTable["SetNextWindowPos"] = [](const ImVec2& pos, sol::optional<int> cond) {
				return ImGui::SetNextWindowPos(pos, static_cast<ImGuiCond>(cond.value_or(0)));
			};

			/*@ylp.function ImGui.SetNextWindowSize
			* param size<ImVec2>
			* param cond<integer?> Optional [ImGuiCond](lua://ImGuiCond)
			@*/
			imguiTable["SetNextWindowSize"] = [](const ImVec2& size, sol::optional<int> cond) {
				return ImGui::SetNextWindowSize(size, static_cast<ImGuiCond>(cond.value_or(0)));
			};

			/*@ylp.function ImGui.SetNextWindowSizeConstraints
			* param minSize<ImVec2>
			* param maxSize<ImVec2>
			@*/
			imguiTable["SetNextWindowSizeConstraints"] = [](const ImVec2& minSz, const ImVec2& maxSz) {
				return ImGui::SetNextWindowSizeConstraints(minSz, maxSz);
			};

			/*@ylp.function ImGui.SetNextItemWidth
			* param width<number> Width of the next item.
			@*/
			imguiTable["SetNextItemWidth"] = [](float width) {
				ImGui::SetNextItemWidth(width);
			};

			/*@ylp.function ImGui.PushItemWidth
			* param width<number> Width of subsequent items.
			@*/
			imguiTable["PushItemWidth"] = [](float width) {
				ImGui::PushItemWidth(width);
			};

			/*@ylp.function ImGui.PopItemWidth
			* 
			@*/
			imguiTable["PopItemWidth"] = []() {
				ImGui::PopItemWidth();
			};

			/*@ylp.function ImGui.SetNextWindowSize
			* 
			@*/
			imguiTable["PopItemWidth"] = []() {
				ImGui::PopItemWidth();
			};

			/*@ylp.function ImGui.IsWindowAppearing
			* return boolean
			@*/
			imguiTable["IsWindowAppearing"] = []() {
				return ImGui::IsWindowAppearing();
			};

			/*@ylp.function ImGui.IsWindowCollapsed
			* return boolean
			@*/
			imguiTable["IsWindowCollapsed"] = []() {
				return ImGui::IsWindowCollapsed();
			};

			// style stuff

			/*@ylp.function ImGui.PushStyleVar
			* param styleVar<integer> [ImGuiStyleVar](lua://ImGuiStyleVar) index
			* param v<number> style value
			@*/

			/*@ylp.function ImGui.PushStyleVar
			* param styleVar<integer> [ImGuiStyleVar](lua://ImGuiStyleVar) index
			* param vX<number> x value
			* param vY<number> y value
			@*/
			imguiTable["PushStyleVar"] = sol::overload(
			    [](int styleVar, float v) {
				    ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(styleVar), v);
			    },
			    [](int styleVar, float vX, float vY) {
				    ImGui::PushStyleVar(static_cast<ImGuiStyleVar>(styleVar), ImVec2(vX, vY));
			    });

			/*@ylp.function ImGui.PopStyleVar
			* param count<integer?> Count of pushed style variables to pop. Defaults to 1; Must match the number of pushed style vars.
			@*/
			imguiTable["PopStyleVar"] = [](sol::optional<int> count) {
				ImGui::PopStyleVar(std::max(count.value_or(1), 1));
			};

			/*@ylp.function ImGui.PushStyleColor
			* param colIdx<integer> [ImGuiCol](lua://ImGuiCol) index
			* param r<number> A number between `0.0` and `1.0` representing the `Red` component of the color.
			* param g<number> A number between `0.0` and `1.0` representing the `Green` component of the color.
			* param b<number> A number between `0.0` and `1.0` representing the `Blue` component of the color.
			* param a<number> A number between `0.0` and `1.0` representing the `Alpha` component of the color.
			@*/
			imguiTable["PushStyleColor"] = [](int colIdx, float r, float g, float b, float a) {
				ImGui::PushStyleColor(static_cast<ImGuiCol>(colIdx), ImVec4(r, g, b, a));
			};

			/*@ylp.function ImGui.PopStyleColor
			* param count<integer?> Count of pushed style colors to pop. Defaults to 1; Must match the number of pushed style cols.
			@*/
			imguiTable["PopStyleColor"] = [](sol::optional<int> count) {
				ImGui::PopStyleColor(std::max(count.value_or(1), 1));
			};

			/*@ylp.function ImGui.SetMouseCursor
			* param cursorIdx<integer> [ImGuiMouseCursor](lua://ImGuiMouseCursor) index
			@*/
			imguiTable["SetMouseCursor"] = [](int cursorIdx) {
				ImGui::SetMouseCursor(static_cast<ImGuiMouseCursor>(cursorIdx));
			};

			// misc

			/*@ylp.function ImGui.SetTooltip
			* param text<string>
			* param ...<any> Optional format args
			@*/
			imguiTable["SetTooltip"] = [&](const std::string& text, sol::variadic_args args) {
				ImGui::SetTooltip(LuaStringFormat(L, text, args).c_str());
			};

			/*@ylp.function ImGui.BeginTooltip
			*
			@*/
			imguiTable["BeginTooltip"] = []() {
				ImGui::BeginTooltip();
			};

			/*@ylp.function ImGui.EndTooltip
			*
			@*/
			imguiTable["EndTooltip"] = []() {
				ImGui::EndTooltip();
			};

			/*@ylp.function ImGui.BeginGroup
			*
			@*/
			imguiTable["BeginGroup"] = []() {
				ImGui::BeginGroup();
			};

			/*@ylp.function ImGui.EndGroup
			*
			@*/
			imguiTable["EndGroup"] = []() {
				ImGui::EndGroup();
			};

			// custom widgets

			/*@ylp.function ImGui.Tooltip Draws a tooltip when the widget this function is called after is hovered.
			* param text<string> Tooltip text
			@*/
			imguiTable["Tooltip"] = [](const std::string& text) {
				ImGui::ToolTip(text.c_str());
			};

			/*@ylp.function ImGui.HelpMarker Draws a help marker (?) symbol in front of the widget this function is called after. When the symbol is hovered, it draws a tooltip.
			* param text<string> Tooltip text
			@*/
			imguiTable["HelpMarker"] = [](const std::string& text) {
				ImGui::HelpMarker(text.c_str());
			};

			/*@ylp.function ImGui.WarningMarker Draws a warning marker [!] symbol in front of the widget this function is called after. When the symbol is hovered, it draws a tooltip.
			* param text<string> Tooltip text
			@*/
			imguiTable["WarningMarker"] = [](const std::string& text) {
				ImGui::WarningMarker(text.c_str());
			};

			/*@ylp.function ImGui.Spinner Draws a spinner indicating 'busy' state.
			*
			@*/

			/*@ylp.function ImGui.Spinner Draws a spinner indicating 'busy' state.
			* param text<string> Text to display next to the spinner. Can be an empty or anonymous string: "##blahblah"
			* param radius<number?> Optional spinner radius. Defaults to 10.0
			@*/

			/*@ylp.function ImGui.Spinner Draws a spinner indicating 'busy' state.
			* param text<string> Text to display next to the spinner. Can be an empty or anonymous string: "##blahblah"
			* param radius<number> spinner radius. Defaults to 10.0
			* param thickness<number?> Optional spinner line thickness. Defaults to 2.0
			@*/
			imguiTable["Spinner"] = sol::overload(
			    []() { ImGui::Spinner(); },
			    [](const std::string& text, sol::optional<float> radius) {
				    ImGui::Spinner(text.c_str(), radius.value_or(10.0f));
			    },
			    [](const std::string& text, float radius, sol::optional<float> thickness) {
				    ImGui::Spinner(text.c_str(), radius, thickness.value_or(2.0f));
			    });

			/*@ylp.function ImGui.YLPSpinner Draws a spinner made out of YLP's logo with a rotating gear and a fixed label.
			*
			@*/

			/*@ylp.function ImGui.YLPSpinner Draws a spinner made out of YLP's logo with a rotating gear and a fixed label.
			* param text<string> Text to display below the spinner. Can be an empty or anonymous string: "##blahblah"
			* param radius<number?> Optional spinner radius. Defaults to frame height.
			@*/

			/*@ylp.function ImGui.YLPSpinner Draws a spinner made out of YLP's logo with a rotating gear and a fixed label.
			* param text<string> Text to display below the spinner. Can be an empty or anonymous string: "##blahblah"
			* param radius<number> spinner radius. Defaults to frame height.
			* param speed<number?> Optional spinning speed. Defaults to 2.8
			@*/
			imguiTable["YLPSpinner"] = sol::overload(
			    []() {
				    ImGui::YLPSpinner();
			    },
			    [](const std::string& text, sol::optional<float> radius) {
				    float rectW = radius.value_or(ImGui::GetFrameHeight());
				    ImGui::YLPSpinner(text.c_str(), ImVec2(rectW, rectW));
			    },
			    [](const std::string& text, float radius, sol::optional<float> speed) {
				    ImGui::YLPSpinner(text.c_str(), ImVec2(radius, radius), speed.value_or(2.8f));
			    });

			/*@ylp.function ImGui.SegmentedControl
			* param currentItem<integer> The current item
			* param items<{[integer]: string}> A table of strings. Must be a contiguous string array, anything else will be ignored.
			* param anchorPos<(0|1|2)?> Anchor position. 0: left | 1: center | 2: right. Defaults to 0: left.
			* return integer selectedItem
			* return boolean clicked
			@*/
			imguiTable["SegmentedControl"] = [](int currentItem, const sol::table& items, sol::optional<int> anchorPos) {
				auto res     = SolTbl2StrArr(items);
				auto pos     = static_cast<ImGui::ImSegmentedCtrlPos>(anchorPos.value_or(0));
				bool clicked = ImGui::SegmentedControl(&currentItem, res.chars, pos);
				return std::make_tuple(currentItem, clicked);
			};
		}
	};

	LuaImGuiLib _LuaImGuiLib;
}
