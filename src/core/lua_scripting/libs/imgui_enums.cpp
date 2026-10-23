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
	class LuaImGuiEnumsLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	public:
		void Register(sol::state& L) override
		{
			// clang-format off

			/*@ylp.enum ImGuiSeparatorFlags
			* field None<integer>
			* field Horizontal<integer> Axis default to current layout type, so generally Horizontal unless e.g. in a menu bar
			* field Vertical<integer>
			* field SpanAllColumns<integer> Make separator cover all columns of a legacy Columns() set.
			@*/
			L.new_enum("ImGuiSeparatorFlags",
				"None", ImGuiSeparatorFlags_None,
				"Horizontal", ImGuiSeparatorFlags_Horizontal,
				"Vertical", ImGuiSeparatorFlags_Vertical,
				"SpanAllColumns", ImGuiSeparatorFlags_SpanAllColumns);

			/*@ylp.enum ImGuiMouseCursor
			* field None<integer>
			* field Arrow<integer>
			* field TextInput<integer> When hovering over InputText, etc.
			* field ResizeAll<integer> (Unused by Dear ImGui functions)
			* field ResizeNS<integer> When hovering over a horizontal border
			* field ResizeEW<integer> When hovering over a vertical border or a column
			* field ResizeNESW<integer> When hovering over the bottom-left corner of a window
			* field ResizeNWSE<integer> When hovering over the bottom-right corner of a window
			* field Hand<integer> (Unused by Dear ImGui functions. Use for e.g. hyperlinks)
			* field Wait<integer> When waiting for something to process/load.
			* field Progress<integer> When waiting for something to process/load, but application is still interactive.
			* field NotAllowed<integer> When hovering something with disallowed interaction. Usually a crossed circle.
			@*/
			L.new_enum("ImGuiMouseCursor",
				"None", ImGuiMouseCursor_None,
				"Arrow", ImGuiMouseCursor_Arrow,
				"TextInput", ImGuiMouseCursor_TextInput,
				"ResizeAll", ImGuiMouseCursor_ResizeAll,
				"ResizeNS", ImGuiMouseCursor_ResizeNS,
				"ResizeEW", ImGuiMouseCursor_ResizeEW,
				"ResizeNESW", ImGuiMouseCursor_ResizeNESW,
				"ResizeNWSE", ImGuiMouseCursor_ResizeNWSE,
				"Hand", ImGuiMouseCursor_Hand,
				"Wait", ImGuiMouseCursor_Wait,
				"Progress", ImGuiMouseCursor_Progress,
				"NotAllowed", ImGuiMouseCursor_NotAllowed);

			/*@ylp.enum ImGuiDir
			* field None<integer>
			* field Left<integer>
			* field Right<integer>
			* field Up<integer>
			* field Down<integer>
			@*/
			L.new_enum("ImGuiDir",
			    "None", ImGuiDir_None,
			    "Left", ImGuiDir_Left,
			    "Right", ImGuiDir_Right,
			    "Up", ImGuiDir_Up,
			    "Down", ImGuiDir_Down);

			/*@ylp.enum ImGuiComboFlags
			* field None<integer>
			* field PopupAlignLeft<integer> Align the popup toward the left by default
			* field HeightSmall<integer> Max ~4 items visible. Tip: If you want your combo popup to be a specific size you can use SetNextWindowSizeConstraints() prior to calling BeginCombo()
			* field HeightRegular<integer> Max ~8 items visible (default)
			* field HeightLarge<integer> Max ~20 items visible
			* field HeightLargest<integer> As many fitting items as possible
			* field NoArrowButton<integer> Display on the preview box without the square arrow button
			* field NoPreview<integer> Display only a square arrow button
			* field WidthFitPreview<integer> Width dynamically calculated from preview contents
			* field HeightMask_<integer>
			@*/
			L.new_enum("ImGuiComboFlags",
				"None", ImGuiComboFlags_None,
				"PopupAlignLeft", ImGuiComboFlags_PopupAlignLeft,
				"HeightSmall", ImGuiComboFlags_HeightSmall,
				"HeightRegular", ImGuiComboFlags_HeightRegular,
				"HeightLarge", ImGuiComboFlags_HeightLarge,
				"HeightLargest", ImGuiComboFlags_HeightLargest,
				"NoArrowButton", ImGuiComboFlags_NoArrowButton,
				"NoPreview", ImGuiComboFlags_NoPreview,
				"WidthFitPreview", ImGuiComboFlags_WidthFitPreview,
				"HeightMask_", ImGuiComboFlags_HeightMask_);

			/*@ylp.enum ImGuiChildFlags
			* field None<integer>
			* field Borders<integer> Show an outer border and enable WindowPadding. (IMPORTANT: this is always == 1 == true for legacy reason)
			* field AlwaysUseWindowPadding<integer> Pad with style.WindowPadding even if no border are drawn (no padding by default for non-bordered child windows because it makes more sense)
			* field ResizeX<integer> Allow resize from right border (layout direction). Enable .ini saving (unless ImGuiWindowFlags_NoSavedSettings passed to window flags)
			* field ResizeY<integer> Allow resize from bottom border (layout direction). "
			* field AutoResizeX<integer> Enable auto-resizing width. Read "IMPORTANT: Size measurement" details above.
			* field AutoResizeY<integer> Enable auto-resizing height. Read "IMPORTANT: Size measurement" details above.
			* field AlwaysAutoResize<integer> Combined with AutoResizeX/AutoResizeY. Always measure size even when child is hidden, always return true, always disable clipping optimization! NOT RECOMMENDED.
			* field FrameStyle<integer> Style the child window like a framed item: use FrameBg, FrameRounding, FrameBorderSize, FramePadding instead of ChildBg, ChildRounding, ChildBorderSize, WindowPadding.
			* field NavFlattened<integer> [BETA] Share focus scope, allow keyboard/gamepad navigation to cross over parent border to this child or between sibling child windows.
			@*/
			L.new_enum("ImGuiChildFlags",
			    "None", ImGuiChildFlags_None,
			    "Borders", ImGuiChildFlags_Borders,
			    "AlwaysUseWindowPadding", ImGuiChildFlags_AlwaysUseWindowPadding,
			    "ResizeX", ImGuiChildFlags_ResizeX,
			    "ResizeY", ImGuiChildFlags_ResizeY,
			    "AutoResizeX", ImGuiChildFlags_AutoResizeX,
			    "AutoResizeY", ImGuiChildFlags_AutoResizeY,
			    "AlwaysAutoResize", ImGuiChildFlags_AlwaysAutoResize,
			    "FrameStyle", ImGuiChildFlags_FrameStyle,
			    "NavFlattened", ImGuiChildFlags_NavFlattened);

			/*@ylp.enum ImGuiWindowFlags
			* field None<integer>
			* field NoTitleBar<integer> Disable title-bar
			* field NoResize<integer> Disable user resizing with the lower-right grip
			* field NoMove<integer> Disable user moving the window
			* field NoScrollbar<integer> Disable scrollbars (window can still scroll with mouse or programmatically)
			* field NoScrollWithMouse<integer> Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
			* field NoCollapse<integer> Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
			* field AlwaysAutoResize<integer> Resize every window to its content every frame
			* field NoBackground<integer> Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
			* field NoSavedSettings<integer> Never load/save settings in .ini file
			* field NoMouseInputs<integer> Disable catching mouse, hovering test with pass through.
			* field MenuBar<integer> Has a menu-bar
			* field HorizontalScrollbar<integer> Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
			* field NoFocusOnAppearing<integer> Disable taking focus when transitioning from hidden to visible state
			* field NoBringToFrontOnFocus<integer> Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
			* field AlwaysVerticalScrollbar<integer> Always show vertical scrollbar (even if ContentSize.y < Size.y)
			* field AlwaysHorizontalScrollbar<integer> Always show horizontal scrollbar (even if ContentSize.x < Size.x)
			* field NoNavInputs<integer> No keyboard/gamepad navigation within the window
			* field NoNavFocus<integer> No focusing toward this window with keyboard/gamepad navigation (e.g. skipped by CTRL+TAB)
			* field UnsavedDocument<integer> Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
			* field NoNav<integer>
			* field NoDecoration<integer>
			* field NoInputs<integer>
			@*/
			L.new_enum("ImGuiWindowFlags",
			    "None", ImGuiWindowFlags_None,
			    "NoTitleBar", ImGuiWindowFlags_NoTitleBar,
			    "NoResize", ImGuiWindowFlags_NoResize,
			    "NoMove", ImGuiWindowFlags_NoMove,
			    "NoScrollbar", ImGuiWindowFlags_NoScrollbar,
			    "NoScrollWithMouse", ImGuiWindowFlags_NoScrollWithMouse,
			    "NoCollapse", ImGuiWindowFlags_NoCollapse,
			    "AlwaysAutoResize", ImGuiWindowFlags_AlwaysAutoResize,
			    "NoBackground", ImGuiWindowFlags_NoBackground,
			    "NoSavedSettings", ImGuiWindowFlags_NoSavedSettings,
			    "NoMouseInputs", ImGuiWindowFlags_NoMouseInputs,
			    "MenuBar", ImGuiWindowFlags_MenuBar,
			    "HorizontalScrollbar", ImGuiWindowFlags_HorizontalScrollbar,
			    "NoFocusOnAppearing", ImGuiWindowFlags_NoFocusOnAppearing,
			    "NoBringToFrontOnFocus", ImGuiWindowFlags_NoBringToFrontOnFocus,
			    "AlwaysHorizontalScrollbar", ImGuiWindowFlags_AlwaysHorizontalScrollbar,
			    "NoNavInputs", ImGuiWindowFlags_NoNavInputs,
			    "NoNavFocus", ImGuiWindowFlags_NoNavFocus,
			    "UnsavedDocument", ImGuiWindowFlags_UnsavedDocument,
			    "NoNav", ImGuiWindowFlags_NoNav,
			    "NoInputs", ImGuiWindowFlags_NoInputs);

			/*@ylp.enum ImGuiStyleVar
			* field Alpha<integer> float     Alpha
			* field DisabledAlpha<integer> float     DisabledAlpha
			* field WindowPadding<integer> ImVec2    WindowPadding
			* field WindowRounding<integer> float     WindowRounding
			* field WindowBorderSize<integer> float     WindowBorderSize
			* field WindowMinSize<integer> ImVec2    WindowMinSize
			* field WindowTitleAlign<integer> ImVec2    WindowTitleAlign
			* field ChildRounding<integer> float     ChildRounding
			* field ChildBorderSize<integer> float     ChildBorderSize
			* field PopupRounding<integer> float     PopupRounding
			* field PopupBorderSize<integer> float     PopupBorderSize
			* field FramePadding<integer> ImVec2    FramePadding
			* field FrameRounding<integer> float     FrameRounding
			* field FrameBorderSize<integer> float     FrameBorderSize
			* field ItemSpacing<integer> ImVec2    ItemSpacing
			* field ItemInnerSpacing<integer> ImVec2    ItemInnerSpacing
			* field IndentSpacing<integer> float     IndentSpacing
			* field CellPadding<integer> ImVec2    CellPadding
			* field ScrollbarSize<integer> float     ScrollbarSize
			* field ScrollbarRounding<integer> float     ScrollbarRounding
			* field ScrollbarPadding<integer> float     ScrollbarPadding
			* field GrabMinSize<integer> float     GrabMinSize
			* field GrabRounding<integer> float     GrabRounding
			* field ImageBorderSize<integer> float     ImageBorderSize
			* field TabRounding<integer> float     TabRounding
			* field TabBorderSize<integer> float     TabBorderSize
			* field TabMinWidthBase<integer> float     TabMinWidthBase
			* field TabMinWidthShrink<integer> float     TabMinWidthShrink
			* field TabBarBorderSize<integer> float     TabBarBorderSize
			* field TabBarOverlineSize<integer> float     TabBarOverlineSize
			* field TableAngledHeadersAngle<integer> float     TableAngledHeadersAngle
			* field TableAngledHeadersTextAlign<integer> ImVec2  TableAngledHeadersTextAlign
			* field TreeLinesSize<integer> float     TreeLinesSize
			* field TreeLinesRounding<integer> float     TreeLinesRounding
			* field ButtonTextAlign<integer> ImVec2    ButtonTextAlign
			* field SelectableTextAlign<integer> ImVec2    SelectableTextAlign
			* field SeparatorTextBorderSize<integer> float     SeparatorTextBorderSize
			* field SeparatorTextAlign<integer> ImVec2    SeparatorTextAlign
			* field SeparatorTextPadding<integer> ImVec2    SeparatorTextPadding
			@*/
			L.new_enum("ImGuiStyleVar",
			    "DisabledAlpha", ImGuiStyleVar_DisabledAlpha,
			    "WindowPadding", ImGuiStyleVar_WindowPadding,
			    "WindowRounding", ImGuiStyleVar_WindowRounding,
			    "WindowBorderSize", ImGuiStyleVar_WindowBorderSize,
			    "WindowMinSize", ImGuiStyleVar_WindowMinSize,
			    "WindowTitleAlign", ImGuiStyleVar_WindowTitleAlign,
			    "ChildRounding", ImGuiStyleVar_ChildRounding,
			    "ChildBorderSize", ImGuiStyleVar_ChildBorderSize,
			    "PopupRounding", ImGuiStyleVar_PopupRounding,
			    "PopupBorderSize", ImGuiStyleVar_PopupBorderSize,
			    "FramePadding", ImGuiStyleVar_FramePadding,
			    "FrameRounding", ImGuiStyleVar_FrameRounding,
			    "FrameBorderSize", ImGuiStyleVar_FrameBorderSize,
			    "ItemSpacing", ImGuiStyleVar_ItemSpacing,
			    "ItemInnerSpacing", ImGuiStyleVar_ItemInnerSpacing,
			    "IndentSpacing", ImGuiStyleVar_IndentSpacing,
			    "CellPadding", ImGuiStyleVar_CellPadding,
			    "ScrollbarSize", ImGuiStyleVar_ScrollbarSize,
			    "ScrollbarRounding", ImGuiStyleVar_ScrollbarRounding,
			    "ScrollbarPadding", ImGuiStyleVar_ScrollbarPadding,
			    "GrabMinSize", ImGuiStyleVar_GrabMinSize,
			    "GrabRounding", ImGuiStyleVar_GrabRounding,
			    "ImageBorderSize", ImGuiStyleVar_ImageBorderSize,
			    "TabRounding", ImGuiStyleVar_TabRounding,
			    "TabBorderSize", ImGuiStyleVar_TabBorderSize,
			    "TabMinWidthBase", ImGuiStyleVar_TabMinWidthBase,
			    "TabMinWidthShrink", ImGuiStyleVar_TabMinWidthShrink,
			    "TabBarBorderSize", ImGuiStyleVar_TabBarBorderSize,
			    "TabBarOverlineSize", ImGuiStyleVar_TabBarOverlineSize,
			    "TableAngledHeadersAngle", ImGuiStyleVar_TableAngledHeadersAngle,
			    "TableAngledHeadersTextAlign", ImGuiStyleVar_TableAngledHeadersTextAlign,
			    "TreeLinesSize", ImGuiStyleVar_TreeLinesSize,
			    "TreeLinesRounding", ImGuiStyleVar_TreeLinesRounding,
			    "ButtonTextAlign", ImGuiStyleVar_ButtonTextAlign,
			    "SelectableTextAlign", ImGuiStyleVar_SelectableTextAlign,
			    "SeparatorTextBorderSize", ImGuiStyleVar_SeparatorTextBorderSize,
			    "SeparatorTextAlign", ImGuiStyleVar_SeparatorTextAlign,
			    "SeparatorTextPadding", ImGuiStyleVar_SeparatorTextPadding);

			/*@ylp.enum ImGuiCol
			* field Text<integer>
			* field TextDisabled<integer>
			* field WindowBg<integer> Background of normal windows
			* field ChildBg<integer> Background of child windows
			* field PopupBg<integer> Background of popups, menus, tooltips windows
			* field Border<integer>
			* field BorderShadow<integer>
			* field FrameBg<integer> Background of checkbox, radio button, plot, slider, text input
			* field FrameBgHovered<integer>
			* field FrameBgActive<integer>
			* field TitleBg<integer> Title bar
			* field TitleBgActive<integer> Title bar when focused
			* field TitleBgCollapsed<integer> Title bar when collapsed
			* field MenuBarBg<integer>
			* field ScrollbarBg<integer>
			* field ScrollbarGrab<integer>
			* field ScrollbarGrabHovered<integer>
			* field ScrollbarGrabActive<integer>
			* field CheckMark<integer> Checkbox tick and RadioButton circle
			* field SliderGrab<integer>
			* field SliderGrabActive<integer>
			* field Button<integer>
			* field ButtonHovered<integer>
			* field ButtonActive<integer>
			* field Header<integer> Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
			* field HeaderHovered<integer>
			* field HeaderActive<integer>
			* field Separator<integer>
			* field SeparatorHovered<integer>
			* field SeparatorActive<integer>
			* field ResizeGrip<integer> Resize grip in lower-right and lower-left corners of windows.
			* field ResizeGripHovered<integer>
			* field ResizeGripActive<integer>
			* field InputTextCursor<integer> InputText cursor/caret
			* field TabHovered<integer> Tab background, when hovered
			* field Tab<integer> Tab background, when tab-bar is focused & tab is unselected
			* field TabSelected<integer> Tab background, when tab-bar is focused & tab is selected
			* field TabSelectedOverline<integer> Tab horizontal overline, when tab-bar is focused & tab is selected
			* field TabDimmed<integer> Tab background, when tab-bar is unfocused & tab is unselected
			* field TabDimmedSelected<integer> Tab background, when tab-bar is unfocused & tab is selected
			* field TabDimmedSelectedOverline<integer> horizontal overline, when tab-bar is unfocused & tab is selected
			* field PlotLines<integer>
			* field PlotLinesHovered<integer>
			* field PlotHistogram<integer>
			* field PlotHistogramHovered<integer>
			* field TableHeaderBg<integer> Table header background
			* field TableBorderStrong<integer> Table outer and header borders (prefer using Alpha=1.0 here)
			* field TableBorderLight<integer> Table inner borders (prefer using Alpha=1.0 here)
			* field TableRowBg<integer> Table row background (even rows)
			* field TableRowBgAlt<integer> Table row background (odd rows)
			* field TextLink<integer> Hyperlink color
			* field TextSelectedBg<integer> Selected text inside an InputText
			* field TreeLines<integer> Tree node hierarchy outlines when using ImGuiTreeNodeFlags_DrawLines
			* field DragDropTarget<integer> Rectangle highlighting a drop target
			* field UnsavedMarker<integer> Unsaved Document marker (in window title and tabs)
			* field NavCursor<integer> Color of keyboard/gamepad navigation cursor/rectangle, when visible
			* field NavWindowingHighlight<integer> Highlight window when using CTRL+TAB
			* field NavWindowingDimBg<integer> Darken/colorize entire screen behind the CTRL+TAB window list, when active
			* field ModalWindowDimBg<integer> Darken/colorize entire screen behind a modal window, when one is active
			@*/
			L.new_enum("ImGuiCol",
			    "TextDisabled", ImGuiCol_TextDisabled,
			    "WindowBg", ImGuiCol_WindowBg,
			    "ChildBg", ImGuiCol_ChildBg,
			    "PopupBg", ImGuiCol_PopupBg,
			    "Border", ImGuiCol_Border,
			    "BorderShadow", ImGuiCol_BorderShadow,
			    "FrameBg", ImGuiCol_FrameBg,
			    "FrameBgHovered", ImGuiCol_FrameBgHovered,
			    "FrameBgActive", ImGuiCol_FrameBgActive,
			    "TitleBg", ImGuiCol_TitleBg,
			    "TitleBgActive", ImGuiCol_TitleBgActive,
			    "TitleBgCollapsed", ImGuiCol_TitleBgCollapsed,
			    "MenuBarBg", ImGuiCol_MenuBarBg,
			    "ScrollbarBg", ImGuiCol_ScrollbarBg,
			    "ScrollbarGrab", ImGuiCol_ScrollbarGrab,
			    "ScrollbarGrabHovered", ImGuiCol_ScrollbarGrabHovered,
			    "ScrollbarGrabActive", ImGuiCol_ScrollbarGrabActive,
			    "CheckMark", ImGuiCol_CheckMark,
			    "SliderGrab", ImGuiCol_SliderGrab,
			    "SliderGrabActive", ImGuiCol_SliderGrabActive,
			    "Button", ImGuiCol_Button,
			    "ButtonHovered", ImGuiCol_ButtonHovered,
			    "ButtonActive", ImGuiCol_ButtonActive,
			    "Header", ImGuiCol_Header,
			    "HeaderHovered", ImGuiCol_HeaderHovered,
			    "HeaderActive", ImGuiCol_HeaderActive,
			    "Separator", ImGuiCol_Separator,
			    "SeparatorHovered", ImGuiCol_SeparatorHovered,
			    "SeparatorActive", ImGuiCol_SeparatorActive,
			    "ResizeGrip", ImGuiCol_ResizeGrip,
			    "ResizeGripHovered", ImGuiCol_ResizeGripHovered,
			    "ResizeGripActive", ImGuiCol_ResizeGripActive,
			    "InputTextCursor", ImGuiCol_InputTextCursor,
			    "TabHovered", ImGuiCol_TabHovered,
			    "Tab", ImGuiCol_Tab,
			    "TabSelected", ImGuiCol_TabSelected,
			    "TabSelectedOverline", ImGuiCol_TabSelectedOverline,
			    "TabDimmed", ImGuiCol_TabDimmed,
			    "TabDimmedSelected", ImGuiCol_TabDimmedSelected,
			    "TabDimmedSelectedOverline", ImGuiCol_TabDimmedSelectedOverline,
			    "PlotLines", ImGuiCol_PlotLines,
			    "PlotLinesHovered", ImGuiCol_PlotLinesHovered,
			    "PlotHistogram", ImGuiCol_PlotHistogram,
			    "PlotHistogramHovered", ImGuiCol_PlotHistogramHovered,
			    "TableHeaderBg", ImGuiCol_TableHeaderBg,
			    "TableBorderStrong", ImGuiCol_TableBorderStrong,
			    "TableBorderLight", ImGuiCol_TableBorderLight,
			    "TableRowBg", ImGuiCol_TableRowBg,
			    "TableRowBgAlt", ImGuiCol_TableRowBgAlt,
			    "TextLink", ImGuiCol_TextLink,
			    "TextSelectedBg", ImGuiCol_TextSelectedBg,
			    "TreeLines", ImGuiCol_TreeLines,
			    "DragDropTarget", ImGuiCol_DragDropTarget,
			    "UnsavedMarker", ImGuiCol_UnsavedMarker,
			    "NavCursor", ImGuiCol_NavCursor,
			    "NavWindowingHighlight", ImGuiCol_NavWindowingHighlight,
			    "NavWindowingDimBg", ImGuiCol_NavWindowingDimBg,
			    "ModalWindowDimBg", ImGuiCol_ModalWindowDimBg);

			/*@ylp.enum ImGuiPopupFlags
			* field None<integer>
			* field MouseButtonLeft<integer> For BeginPopupContext*(): open on Left Mouse release. Guaranteed to always be == 0 (same as ImGuiMouseButton_Left)
			* field MouseButtonRight<integer> For BeginPopupContext*(): open on Right Mouse release. Guaranteed to always be == 1 (same as ImGuiMouseButton_Right)
			* field MouseButtonMiddle<integer> For BeginPopupContext*(): open on Middle Mouse release. Guaranteed to always be == 2 (same as ImGuiMouseButton_Middle)
			* field MouseButtonMask_<integer>
			* field MouseButtonDefault_<integer>
			* field NoReopen<integer> For OpenPopup*(), BeginPopupContext*(): don't reopen same popup if already open (won't reposition, won't reinitialize navigation)
			* field NoOpenOverExistingPopup<integer> For OpenPopup*(), BeginPopupContext*(): don't open if there's already a popup at the same level of the popup stack
			* field NoOpenOverItems<integer> For BeginPopupContextWindow(): don't return true when hovering items, only when hovering empty space
			* field AnyPopupId<integer> For IsPopupOpen(): ignore the ImGuiID parameter and test for any popup.
			* field AnyPopupLevel<integer> For IsPopupOpen(): search/test at any level of the popup stack (default test in the current level)
			* field AnyPopup<integer>
			@*/
			L.new_enum("ImGuiPopupFlags",
			    "None", ImGuiPopupFlags_None,
			    "MouseButtonLeft", ImGuiPopupFlags_MouseButtonLeft,
			    "MouseButtonRight", ImGuiPopupFlags_MouseButtonRight,
			    "MouseButtonMiddle", ImGuiPopupFlags_MouseButtonMiddle,
			    "MouseButtonMask_", ImGuiPopupFlags_MouseButtonMask_,
			    "MouseButtonDefault_", ImGuiPopupFlags_MouseButtonDefault_,
			    "NoReopen", ImGuiPopupFlags_NoReopen,
			    "NoOpenOverExistingPopup", ImGuiPopupFlags_NoOpenOverExistingPopup,
			    "NoOpenOverItems", ImGuiPopupFlags_NoOpenOverItems,
			    "AnyPopupId", ImGuiPopupFlags_AnyPopupId,
			    "AnyPopupLevel", ImGuiPopupFlags_AnyPopupLevel,
			    "AnyPopup", ImGuiPopupFlags_AnyPopup);

			/*@ylp.enum ImGuiSelectableFlags
			* field None<integer>
			* field NoAutoClosePopups<integer> Clicking this doesn't close parent popup window (overrides ImGuiItemFlags_AutoClosePopups)
			* field SpanAllColumns<integer> Frame will span all columns of its container table (text will still fit in current column)
			* field AllowDoubleClick<integer> Generate press events on double clicks too
			* field Disabled<integer> Cannot be selected, display grayed out text
			* field AllowOverlap<integer> (WIP) Hit testing to allow subsequent widgets to overlap this one
			* field Highlight<integer> Make the item be displayed as if it is hovered
			* field SelectOnNav<integer> Auto-select when moved into, unless Ctrl is held. Automatic when in a BeginMultiSelect() block.
			@*/
			L.new_enum("ImGuiSelectableFlags",
			    "None", ImGuiSelectableFlags_None,
			    "NoAutoClosePopups", ImGuiSelectableFlags_NoAutoClosePopups,
			    "SpanAllColumns", ImGuiSelectableFlags_SpanAllColumns,
			    "AllowDoubleClick", ImGuiSelectableFlags_AllowDoubleClick,
			    "Disabled", ImGuiSelectableFlags_Disabled,
			    "AllowOverlap", ImGuiSelectableFlags_AllowOverlap,
			    "Highlight", ImGuiSelectableFlags_Highlight,
			    "SelectOnNav", ImGuiSelectableFlags_SelectOnNav);

			/*@ylp.enum ImGuiInputTextFlags
			* field None<integer>
			* field CharsDecimal<integer> Allow 0123456789.+-* /
			* field CharsHexadecimal<integer> Allow 0123456789ABCDEFabcdef
			* field CharsScientific<integer> Allow 0123456789.+-* /eE (Scientific notation input)
			* field CharsUppercase<integer> Turn a..z into A..Z
			* field CharsNoBlank<integer> Filter out spaces, tabs
			* field AllowTabInput<integer> Pressing TAB input a '\t' character into the text field
			* field EnterReturnsTrue<integer> Return 'true' when Enter is pressed (as opposed to every time the value was modified). Consider using IsItemDeactivatedAfterEdit() instead!
			* field EscapeClearsAll<integer> Escape key clears content if not empty, and deactivate otherwise (contrast to default behavior of Escape to revert)
			* field CtrlEnterForNewLine<integer> In multi-line mode, validate with Enter, add new line with Ctrl+Enter (default is opposite: validate with Ctrl+Enter, add line with Enter).
			* field ReadOnly<integer> Read-only mode
			* field Password<integer> Password mode, display all characters as '*', disable copy
			* field AlwaysOverwrite<integer> Overwrite mode
			* field AutoSelectAll<integer> Select entire text when first taking mouse focus
			* field ParseEmptyRefVal<integer> InputFloat(), InputInt(), InputScalar() etc. only: parse empty string as zero value.
			* field DisplayEmptyRefVal<integer> InputFloat(), InputInt(), InputScalar() etc. only: when value is zero, do not display it. Generally used with ImGuiInputTextFlags_ParseEmptyRefVal.
			* field NoHorizontalScroll<integer> Disable following the cursor horizontally
			* field NoUndoRedo<integer> Disable undo/redo. Note that input text owns the text data while active, if you want to provide your own undo/redo stack you need e.g. to call ClearActiveID().
			* field ElideLeft<integer> When text doesn't fit, elide left side to ensure right side stays visible. Useful for path/filenames. Single-line only!
			* field CallbackCompletion<integer> Callback on pressing TAB (for completion handling)
			* field CallbackHistory<integer> Callback on pressing Up/Down arrows (for history handling)
			* field CallbackAlways<integer> Callback on each iteration. User code may query cursor position, modify text buffer.
			* field CallbackCharFilter<integer> Callback on character inputs to replace or discard them. Modify 'EventChar' to replace or discard, or return 1 in callback to discard.
			* field CallbackResize<integer> Callback on buffer capacity changes request (beyond 'buf_size' parameter value), allowing the string to grow. Notify when the string wants to be resized (for string types which hold a cache of their Size). You will be provided a new BufSize in the callback and NEED to honor it. (see misc/cpp/imgui_stdlib.h for an example of using this)
			* field CallbackEdit<integer> Callback on any edit. Note that InputText() already returns true on edit + you can always use IsItemEdited(). The callback is useful to manipulate the underlying buffer while focus is active.
			* field WordWrap<integer> InputTextMultine(): word-wrap lines that are too long.
			@*/
			L.new_enum("ImGuiInputTextFlags",
			    "None", ImGuiInputTextFlags_None,
			    "CharsDecimal", ImGuiInputTextFlags_CharsDecimal,
			    "CharsHexadecimal", ImGuiInputTextFlags_CharsHexadecimal,
			    "CharsScientific", ImGuiInputTextFlags_CharsScientific,
			    "CharsUppercase", ImGuiInputTextFlags_CharsUppercase,
			    "CharsNoBlank", ImGuiInputTextFlags_CharsNoBlank,
			    "AllowTabInput", ImGuiInputTextFlags_AllowTabInput,
			    "EnterReturnsTrue", ImGuiInputTextFlags_EnterReturnsTrue,
			    "EscapeClearsAll", ImGuiInputTextFlags_EscapeClearsAll,
			    "CtrlEnterForNewLine", ImGuiInputTextFlags_CtrlEnterForNewLine,
			    "ReadOnly", ImGuiInputTextFlags_ReadOnly,
			    "Password", ImGuiInputTextFlags_Password,
			    "AlwaysOverwrite", ImGuiInputTextFlags_AlwaysOverwrite,
			    "AutoSelectAll", ImGuiInputTextFlags_AutoSelectAll,
			    "ParseEmptyRefVal", ImGuiInputTextFlags_ParseEmptyRefVal,
			    "DisplayEmptyRefVal", ImGuiInputTextFlags_DisplayEmptyRefVal,
			    "NoHorizontalScroll", ImGuiInputTextFlags_NoHorizontalScroll,
			    "NoUndoRedo", ImGuiInputTextFlags_NoUndoRedo,
			    "ElideLeft", ImGuiInputTextFlags_ElideLeft,
			    "CallbackCompletion", ImGuiInputTextFlags_CallbackCompletion,
			    "CallbackHistory", ImGuiInputTextFlags_CallbackHistory,
			    "CallbackAlways", ImGuiInputTextFlags_CallbackAlways,
			    "CallbackCharFilter", ImGuiInputTextFlags_CallbackCharFilter,
			    "CallbackResize", ImGuiInputTextFlags_CallbackResize,
			    "CallbackEdit", ImGuiInputTextFlags_CallbackEdit,
			    "WordWrap", ImGuiInputTextFlags_WordWrap);

			/*@ylp.enum ImGuiSliderFlags
			* field None<integer>
			* field Logarithmic<integer> Make the widget logarithmic (linear otherwise). Consider using ImGuiSliderFlags_NoRoundToFormat with this if using a format-string with small amount of digits.
			* field NoRoundToFormat<integer> Disable rounding underlying value to match precision of the display format string (e.g. %.3f values are rounded to those 3 digits).
			* field NoInput<integer> Disable CTRL+Click or Enter key allowing to input text directly into the widget.
			* field WrapAround<integer> Enable wrapping around from max to min and from min to max. Only supported by DragXXX() functions for now.
			* field ClampOnInput<integer> Clamp value to min/max bounds when input manually with CTRL+Click. By default CTRL+Click allows going out of bounds.
			* field ClampZeroRange<integer> Clamp even if min==max==0.0f. Otherwise due to legacy reason DragXXX functions don't clamp with those values. When your clamping limits are dynamic you almost always want to use it.
			* field NoSpeedTweaks<integer> Disable keyboard modifiers altering tweak speed. Useful if you want to alter tweak speed yourself based on your own logic.
			* field AlwaysClamp<integer>
			@*/
			L.new_enum("ImGuiSliderFlags",
				"None", ImGuiSliderFlags_None,
				"Logarithmic", ImGuiSliderFlags_Logarithmic,
				"NoRoundToFormat", ImGuiSliderFlags_NoRoundToFormat,
				"NoInput", ImGuiSliderFlags_NoInput,
				"WrapAround", ImGuiSliderFlags_WrapAround,
				"ClampOnInput", ImGuiSliderFlags_ClampOnInput,
				"ClampZeroRange", ImGuiSliderFlags_ClampZeroRange,
				"NoSpeedTweaks", ImGuiSliderFlags_NoSpeedTweaks,
				"AlwaysClamp", ImGuiSliderFlags_AlwaysClamp);

		}
	};

	LuaImGuiEnumsLib _LuaImGuiEnumsLib;
}
