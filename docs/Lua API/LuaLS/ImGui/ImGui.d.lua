---@meta

---@class ImGui
ImGui = {}

---@param id integer 
function ImGui.PushID(id) end

function ImGui.PopID() end

---@param font userdata Use the global [Fonts](lua://Fonts) table to access fonts.
function ImGui.PushFont(font) end

function ImGui.PopFont() end

function ImGui.Bullet() end

---@param text string 
---@param ... any Optional string format arguments
function ImGui.Text(text, ...) end

-- Draws a text centered at either the current available region's width or the specified optional width.
---@param text string 
---@param font userdata? Optional font. Use the global [Fonts](lua://Fonts) table to access fonts.
---@param customWidth number? Optional width to center at.
function ImGui.TextCentered(text, font, customWidth) end

---@param text string 
---@param ... any Optional string format arguments
function ImGui.TextDisabled(text, ...) end

---@param text string 
---@param ... any Optional string format arguments
function ImGui.TextWrapped(text, ...) end

---@param text string 
function ImGui.TextUnformatted(text) end

---@param r number A number between `0.0` and `1.0` representing the `Red` component of the color.
---@param g number A number between `0.0` and `1.0` representing the `Green` component of the color.
---@param b number A number between `0.0` and `1.0` representing the `Blue` component of the color.
---@param a number A number between `0.0` and `1.0` representing the `Alpha` component of the color.
---@param text string 
---@param ... any Optional string format arguments
function ImGui.TextColored(r, g, b, a, text, ...) end

-- Draws a large and bold text.
---@param text string 
---@param separator boolean? Optional: Draw a SeparatorText instead.
function ImGui.TitleText(text, separator) end

---@param text string 
---@param ... any Optional string format arguments
function ImGui.BulletText(text, ...) end

---@param label string 
---@param size ImVec2? Optional button size
---@return boolean clicked
function ImGui.Button(label, size) end

---@param label string 
---@return boolean clicked
function ImGui.SmallButton(label) end

---@param label string 
---@param direction integer [ImGuiDir](lua://ImGuiDir)
---@return boolean clicked
function ImGui.ArrowButton(label, direction) end

---@param label string 
---@param size ImVec2 
---@return boolean clicked
function ImGui.InvisibleButton(label, size) end

---@param label string 
---@param v boolean 
---@return boolean value
---@return boolean clicked
function ImGui.Checkbox(label, v) end

---@param label string 
---@param active boolean 
---@return boolean clicked
function ImGui.RadioButton(label, active) end

---@param label string 
---@param v integer Current index
---@param vButton integer Index to change to
---@return integer newValue
---@return boolean clicked
function ImGui.RadioButton(label, v, vButton) end

---@param fraction number 
---@param size ImVec2? Optional frame size
---@param overlayText string? Optional overlay text. Defaults to precentage.
function ImGui.ProgressBar(fraction, size, overlayText) end

---@param label string 
---@param currentItem integer 
---@param itemsSeparatedByZeros string A string with items separated by null terminators. Ex: `"Item1\0Item2\0Item3"`
---@return integer currentItem
---@return boolean opened
function ImGui.Combo(label, currentItem, itemsSeparatedByZeros) end

---@param label string 
---@param currentItem integer 
---@param items {[integer]: string} A table of strings. Must be a contiguous string array, anything else will be ignored.
---@return integer currentItem
---@return boolean opened
function ImGui.Combo(label, currentItem, items) end

---@param label string 
---@param preview string 
---@param flags integer? Optional [ImGuiComboFlags](lua://ImGuiComboFlags)
---@return boolean opened
function ImGui.BeginCombo(label, preview, flags) end

function ImGui.EndCombo() end

---@param label string 
---@param currentItem integer 
---@param items {[integer]: string} A table of strings. Must be a contiguous string array, anything else will be ignored.
---@param heightInItems integer? Optional max items to show. Defaults to -1
---@return integer currentItem
---@return boolean visible
function ImGui.ListBox(label, currentItem, items, heightInItems) end

---@param label string 
---@param size ImVec2? Optional listbox size
---@return boolean visible
function ImGui.BeginListBox(label, size) end

function ImGui.EndListBox() end

---@param label string 
---@param selected boolean 
---@param flags integer? Optional [ImGuiSelectableFlags](lua://ImGuiSelectableFlags)
---@param size ImVec2? Optional selectable size
---@return boolean clicked
function ImGui.Selectable(label, selected, flags, size) end

---@param label string 
---@param selected boolean 
---@return boolean selected
---@return boolean clicked
function ImGui.MenuItem(label, selected) end

-- Disables all ImGui widgets created between this call and EndDisabled
---@param cond boolean? Optional condition
function ImGui.BeginDisabled(cond) end

function ImGui.EndDisabled() end

---@param name string 
---@return boolean visible
function ImGui.BeginChild(name) end

---@param name string 
---@param size ImVec2 child size.
---@return boolean visible
function ImGui.BeginChild(name, size) end

---@param name string 
---@param size ImVec2 child size.
---@param childFlags integer [ImGuiChildFlags](lua://ImGuiChildFlags)
---@param windowFlags integer? Optional [ImGuiWindowFlags](lua://ImGuiWindowFlags)
---@return boolean visible
function ImGui.BeginChild(name, size, childFlags, windowFlags) end

function ImGui.EndChild() end

---@param name string Popup name
---@param flags integer? Optional [ImGuiPopupFlags](lua://ImGuiPopupFlags)
function ImGui.OpenPopup(name, flags) end

---@param name string Popup name
---@param flags integer? Optional [ImGuiPopupFlags](lua://ImGuiPopupFlags)
---@return boolean 
function ImGui.IsPopupOpen(name, flags) end

function ImGui.CloseCurrentPopup() end

---@param name string 
---@param windowFlags integer? Optional [ImGuiWindowFlags](lua://ImGuiWindowFlags)
---@return boolean isOpen
function ImGui.BeginPopup(name, windowFlags) end

---@param name string 
---@param windowFlags integer? Optional [ImGuiWindowFlags](lua://ImGuiWindowFlags)
---@return boolean visible
function ImGui.BeginPopupModal(name, windowFlags) end

---@param name string 
---@param open boolean When true, the popup will have a `(x)` close button in the title bar.
---@param windowFlags integer? Optional [ImGuiWindowFlags](lua://ImGuiWindowFlags)
---@return boolean visible
---@return boolean open
function ImGui.BeginPopupModal(name, open, windowFlags) end

function ImGui.EndPopup() end

---@param label string 
---@param v integer 
---@param vMin integer Minimum value
---@param vMax integer Maximum value
---@return integer v
---@return boolean changed
function ImGui.SliderInt(label, v, vMin, vMax) end

---@param label string 
---@param v integer 
---@param vMin integer Minimum value
---@param vMax integer Maximum value
---@param fmt string format string
---@param flags integer? Optional [ImGuiSliderFlags](lua://ImGuiSliderFlags)
---@return integer v
---@return boolean changed
function ImGui.SliderInt(label, v, vMin, vMax, fmt, flags) end

---@param label string 
---@param v number 
---@param vMin number Minimum value
---@param vMax number Maximum value
---@return number v
---@return boolean changed
function ImGui.SliderFloat(label, v, vMin, vMax) end

---@param label string 
---@param v number 
---@param vMin number Minimum value
---@param vMax number Maximum value
---@param fmt string format string
---@param flags integer? Optional [ImGuiSliderFlags](lua://ImGuiSliderFlags)
---@return number v
---@return boolean changed
function ImGui.SliderFloat(label, v, vMin, vMax, fmt, flags) end

---@param label string 
---@param v integer 
---@return integer v
---@return boolean changed
function ImGui.InputInt(label, v) end

---@param label string 
---@param v integer 
---@param step integer step
---@param stepFast integer fast step
---@param flags integer? Optional [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
---@return integer v
---@return boolean changed
function ImGui.InputInt(label, v, step, stepFast, flags) end

---@param label string 
---@param v number 
---@return number v
---@return boolean changed
function ImGui.InputFloat(label, v) end

---@param label string 
---@param v number 
---@param step number step
---@param stepFast number fast step
---@param fmt string? Optional format string
---@return number v
---@return boolean changed
function ImGui.InputFloat(label, v, step, stepFast, fmt) end

---@param label string 
---@param v number 
---@param step number step
---@param stepFast number fast step
---@param fmt string format string
---@param flags integer? Optional [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
---@return number v
---@return boolean changed
function ImGui.InputFloat(label, v, step, stepFast, fmt, flags) end

---@param label string 
---@param text string 
---@param bufferSize integer 
---@param flags integer? Optional [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
---@return string text
---@return boolean changed
function ImGui.InputText(label, text, bufferSize, flags) end

---@param label string 
---@param hint string 
---@param text string 
---@param bufferSize integer 
---@param flags integer? Optional [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
---@return string text
---@return boolean changed
function ImGui.InputTextWithHint(label, hint, text, bufferSize, flags) end

---@param label string 
---@param text string 
---@param bufferSize integer 
---@return string text
---@return boolean changed
function ImGui.InputTextMultiline(label, text, bufferSize) end

---@param label string 
---@param text string 
---@param bufferSize integer 
---@param flags integer [ImGuiInputTextFlags](lua://ImGuiInputTextFlags)
---@param size ImVec2? Optional widget size
---@return string text
---@return boolean changed
function ImGui.InputTextMultiline(label, text, bufferSize, flags, size) end

function ImGui.SameLine() end

---@param offsetX number X position to start at. Defaults to `0`.
---@param spacing number? Optional spacing after the previous item. Defaults to the current item spacing.
function ImGui.SameLine(offsetX, spacing) end

function ImGui.NewLine() end

function ImGui.Spacing() end

function ImGui.Separator() end

---@param flags integer [ImGuiSeparatorFlags](lua://ImGuiSeparatorFlags)
---@param thickness number? Optional separator thickness. Defaults to 1.0
function ImGui.SeparatorEx(flags, thickness) end

---@param text string 
function ImGui.SeparatorText(text) end

---@param indentX number? Optional indentation width. Defaults to the current item spacing.
function ImGui.Indent(indentX) end

---@param indentX number? Optional indentation width. Defaults to the current item spacing.
function ImGui.Unindent(indentX) end

---@param sizeX number Width of the dummy item.
---@param sizeY number Height of the dummy item.
function ImGui.Dummy(sizeX, sizeY) end

---@param x number X position relative to the current window.
---@param y number Y position relative to the current window.
function ImGui.SetCursorPos(x, y) end

---@param x number X position relative to the current window.
function ImGui.SetCursorPosX(x) end

---@param y number Y position relative to the current window.
function ImGui.SetCursorPosY(y) end

---@return ImVec2 cursorPos
function ImGui.GetCursorPos() end

---@return number x Current X position relative to the current window.
function ImGui.GetCursorPosX() end

---@return number y Current Y position relative to the current window.
function ImGui.GetCursorPosY() end

---@return ImVec2 avail Available content region.
function ImGui.GetContentRegionAvail() end

---@return ImVec2 windowSize
function ImGui.GetWindowSize() end

---@return ImVec2 windowPos Current window position.
function ImGui.GetWindowPos() end

---@return number width Current window width
function ImGui.GetWindowWidth() end

---@return number height Current window height
function ImGui.GetWindowHeight() end

---@return ImDrawList 
function ImGui.GetWindowDrawList() end

---@return ImDrawList 
function ImGui.GetBackgroundDrawList() end

---@return ImDrawList 
function ImGui.GetForegroundDrawList() end

---@return number frameHeight
function ImGui.GetFrameHeight() end

---@return number frameHeight Frame height with spacing.
function ImGui.GetFrameHeightWithSpacing() end

---@return number frameTime
function ImGui.GetTime() end

---@param size ImVec2 Window size
---@param cond integer? Optional [ImGuiCond](lua://ImGuiCond)
function ImGui.SetWindowSize(size, cond) end

---@param name string Window name
---@param size ImVec2 Window size
---@param cond integer? Optional [ImGuiCond](lua://ImGuiCond)
function ImGui.SetWindowSize(name, size, cond) end

---@param pos ImVec2 New position
---@param cond integer? Optional [ImGuiCond](lua://ImGuiCond)
function ImGui.SetWindowPos(pos, cond) end

---@param name string Window name
---@param pos ImVec2 New position
---@param cond integer? Optional [ImGuiCond](lua://ImGuiCond)
function ImGui.SetWindowPos(name, pos, cond) end

---@param pos ImVec2 
---@param cond integer? Optional [ImGuiCond](lua://ImGuiCond)
function ImGui.SetNextWindowSize(pos, cond) end

---@param size ImVec2 
---@param cond integer? Optional [ImGuiCond](lua://ImGuiCond)
function ImGui.SetNextWindowSize(size, cond) end

---@param minSize ImVec2 
---@param maxSize ImVec2 
function ImGui.SetNextWindowSizeConstraints(minSize, maxSize) end

---@param width number Width of the next item.
function ImGui.SetNextItemWidth(width) end

---@param width number Width of subsequent items.
function ImGui.PushItemWidth(width) end

function ImGui.PopItemWidth() end

function ImGui.SetNextWindowSize() end

---@return boolean 
function ImGui.IsWindowAppearing() end

---@return boolean 
function ImGui.IsWindowCollapsed() end

---@param styleVar integer [ImGuiStyleVar](lua://ImGuiStyleVar) index
---@param v number style value
function ImGui.PushStyleVar(styleVar, v) end

---@param styleVar integer [ImGuiStyleVar](lua://ImGuiStyleVar) index
---@param vX number x value
---@param vY number y value
function ImGui.PushStyleVar(styleVar, vX, vY) end

---@param count integer? Count of pushed style variables to pop. Defaults to 1; Must match the number of pushed style vars.
function ImGui.PopStyleVar(count) end

---@param colIdx integer [ImGuiCol](lua://ImGuiCol) index
---@param r number A number between `0.0` and `1.0` representing the `Red` component of the color.
---@param g number A number between `0.0` and `1.0` representing the `Green` component of the color.
---@param b number A number between `0.0` and `1.0` representing the `Blue` component of the color.
---@param a number A number between `0.0` and `1.0` representing the `Alpha` component of the color.
function ImGui.PushStyleColor(colIdx, r, g, b, a) end

---@param count integer? Count of pushed style colors to pop. Defaults to 1; Must match the number of pushed style cols.
function ImGui.PopStyleColor(count) end

---@param cursorIdx integer [ImGuiMouseCursor](lua://ImGuiMouseCursor) index
function ImGui.SetMouseCursor(cursorIdx) end

---@param text string 
---@param ... any Optional format args
function ImGui.SetTooltip(text, ...) end

function ImGui.BeginTooltip() end

function ImGui.EndTooltip() end

function ImGui.BeginGroup() end

function ImGui.EndGroup() end

-- Draws a tooltip when the widget this function is called after is hovered.
---@param text string Tooltip text
function ImGui.Tooltip(text) end

-- Draws a help marker (?) symbol in front of the widget this function is called after. When the symbol is hovered, it draws a tooltip.
---@param text string Tooltip text
function ImGui.HelpMarker(text) end

-- Draws a warning marker [!] symbol in front of the widget this function is called after. When the symbol is hovered, it draws a tooltip.
---@param text string Tooltip text
function ImGui.WarningMarker(text) end

-- Draws a spinner indicating 'busy' state.
function ImGui.Spinner() end

-- Draws a spinner indicating 'busy' state.
---@param text string Text to display next to the spinner. Can be an empty or anonymous string: "##blahblah"
---@param radius number? Optional spinner radius. Defaults to 10.0
function ImGui.Spinner(text, radius) end

-- Draws a spinner indicating 'busy' state.
---@param text string Text to display next to the spinner. Can be an empty or anonymous string: "##blahblah"
---@param radius number spinner radius. Defaults to 10.0
---@param thickness number? Optional spinner line thickness. Defaults to 2.0
function ImGui.Spinner(text, radius, thickness) end

-- Draws a spinner made out of YLP's logo with a rotating gear and a fixed label.
function ImGui.YLPSpinner() end

-- Draws a spinner made out of YLP's logo with a rotating gear and a fixed label.
---@param text string Text to display below the spinner. Can be an empty or anonymous string: "##blahblah"
---@param radius number? Optional spinner radius. Defaults to frame height.
function ImGui.YLPSpinner(text, radius) end

-- Draws a spinner made out of YLP's logo with a rotating gear and a fixed label.
---@param text string Text to display below the spinner. Can be an empty or anonymous string: "##blahblah"
---@param radius number spinner radius. Defaults to frame height.
---@param speed number? Optional spinning speed. Defaults to 2.8
function ImGui.YLPSpinner(text, radius, speed) end

---@param currentItem integer The current item
---@param items {[integer]: string} A table of strings. Must be a contiguous string array, anything else will be ignored.
---@param anchorPos (0|1|2)? Anchor position. 0: left | 1: center | 2: right. Defaults to 0: left.
---@return integer selectedItem
---@return boolean clicked
function ImGui.SegmentedControl(currentItem, items, anchorPos) end
