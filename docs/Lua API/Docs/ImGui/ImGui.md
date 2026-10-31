<!-- markdownlint-disable -->

# ImGui

## Methods

## `PushID`

```lua
function ImGui.PushID(id) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `id` | `integer` |  |

## `PopID`

```lua
function ImGui.PopID() end
```

## `PushFont`

```lua
function ImGui.PushFont(font) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `font` | `userdata` | Use the global [Fonts](./Fonts.md) table to access fonts. |

## `PopFont`

```lua
function ImGui.PopFont() end
```

## `Bullet`

```lua
function ImGui.Bullet() end
```

## `Text`

```lua
function ImGui.Text(text, ...) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` |  |
| `...` | `any` | Optional string format arguments |

## `TextCentered`

```lua
function ImGui.TextCentered(text, font, customWidth) end
```

Draws a text centered at either the current available region's width or the specified optional width.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` |  |
| `font` | `userdata?` | Optional font. Use the global [Fonts](./Fonts.md) table to access fonts. |
| `customWidth` | `number?` | Optional width to center at. |

## `TextDisabled`

```lua
function ImGui.TextDisabled(text, ...) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` |  |
| `...` | `any` | Optional string format arguments |

## `TextWrapped`

```lua
function ImGui.TextWrapped(text, ...) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` |  |
| `...` | `any` | Optional string format arguments |

## `TextUnformatted`

```lua
function ImGui.TextUnformatted(text) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` |  |

## `TextColored`

```lua
function ImGui.TextColored(r, g, b, a, text, ...) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `r` | `number` | A number between `0.0` and `1.0` representing the `Red` component of the color. |
| `g` | `number` | A number between `0.0` and `1.0` representing the `Green` component of the color. |
| `b` | `number` | A number between `0.0` and `1.0` representing the `Blue` component of the color. |
| `a` | `number` | A number between `0.0` and `1.0` representing the `Alpha` component of the color. |
| `text` | `string` |  |
| `...` | `any` | Optional string format arguments |

## `TitleText`

```lua
function ImGui.TitleText(text, separator) end
```

Draws a large and bold text.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` |  |
| `separator` | `boolean?` | Optional: Draw a SeparatorText instead. |

## `BulletText`

```lua
function ImGui.BulletText(text, ...) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` |  |
| `...` | `any` | Optional string format arguments |

## `Button`

```lua
function ImGui.Button(label, size) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `size` | `ImVec2?` | Optional button size |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | clicked |

## `SmallButton`

```lua
function ImGui.SmallButton(label) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | clicked |

## `ArrowButton`

```lua
function ImGui.ArrowButton(label, direction) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `direction` | `integer` | [ImGuiDir](./ImGuiDir.md) |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | clicked |

## `InvisibleButton`

```lua
function ImGui.InvisibleButton(label, size) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `size` | `ImVec2` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | clicked |

## `Checkbox`

```lua
function ImGui.Checkbox(label, v) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `boolean` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | value |
| `boolean` | clicked |

## `RadioButton`

```lua
function ImGui.RadioButton(label, active) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `active` | `boolean` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | clicked |

## `RadioButton`

```lua
function ImGui.RadioButton(label, v, vButton) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `integer` | Current index |
| `vButton` | `integer` | Index to change to |

### Returns

| Type | Description |
| --- | --- |
| `integer` | newValue |
| `boolean` | clicked |

## `ProgressBar`

```lua
function ImGui.ProgressBar(fraction, size, overlayText) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `fraction` | `number` |  |
| `size` | `ImVec2?` | Optional frame size |
| `overlayText` | `string?` | Optional overlay text. Defaults to precentage. |

## `Combo`

```lua
function ImGui.Combo(label, currentItem, itemsSeparatedByZeros) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `currentItem` | `integer` |  |
| `itemsSeparatedByZeros` | `string` | A string with items separated by null terminators. Ex: `"Item1\0Item2\0Item3"` |

### Returns

| Type | Description |
| --- | --- |
| `integer` | currentItem |
| `boolean` | opened |

## `Combo`

```lua
function ImGui.Combo(label, currentItem, items) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `currentItem` | `integer` |  |
| `items` | `{[integer]: string}` | A table of strings. Must be a contiguous string array, anything else will be ignored. |

### Returns

| Type | Description |
| --- | --- |
| `integer` | currentItem |
| `boolean` | opened |

## `BeginCombo`

```lua
function ImGui.BeginCombo(label, preview, flags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `preview` | `string` |  |
| `flags` | `integer?` | Optional [ImGuiComboFlags](./ImGuiComboFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | opened |

## `EndCombo`

```lua
function ImGui.EndCombo() end
```

## `ListBox`

```lua
function ImGui.ListBox(label, currentItem, items, heightInItems) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `currentItem` | `integer` |  |
| `items` | `{[integer]: string}` | A table of strings. Must be a contiguous string array, anything else will be ignored. |
| `heightInItems` | `integer?` | Optional max items to show. Defaults to -1 |

### Returns

| Type | Description |
| --- | --- |
| `integer` | currentItem |
| `boolean` | visible |

## `BeginListBox`

```lua
function ImGui.BeginListBox(label, size) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `size` | `ImVec2?` | Optional listbox size |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | visible |

## `EndListBox`

```lua
function ImGui.EndListBox() end
```

## `Selectable`

```lua
function ImGui.Selectable(label, selected, flags, size) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `selected` | `boolean` |  |
| `flags` | `integer?` | Optional [ImGuiSelectableFlags](./ImGuiSelectableFlags.md) |
| `size` | `ImVec2?` | Optional selectable size |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | clicked |

## `MenuItem`

```lua
function ImGui.MenuItem(label, selected) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `selected` | `boolean` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | selected |
| `boolean` | clicked |

## `BeginDisabled`

```lua
function ImGui.BeginDisabled(cond) end
```

Disables all ImGui widgets created between this call and EndDisabled

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `cond` | `boolean?` | Optional condition |

## `EndDisabled`

```lua
function ImGui.EndDisabled() end
```

## `BeginChild`

```lua
function ImGui.BeginChild(name) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | visible |

## `BeginChild`

```lua
function ImGui.BeginChild(name, size) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` |  |
| `size` | `ImVec2` | child size. |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | visible |

## `BeginChild`

```lua
function ImGui.BeginChild(name, size, childFlags, windowFlags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` |  |
| `size` | `ImVec2` | child size. |
| `childFlags` | `integer` | [ImGuiChildFlags](./ImGuiChildFlags.md) |
| `windowFlags` | `integer?` | Optional [ImGuiWindowFlags](./ImGuiWindowFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | visible |

## `EndChild`

```lua
function ImGui.EndChild() end
```

## `OpenPopup`

```lua
function ImGui.OpenPopup(name, flags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` | Popup name |
| `flags` | `integer?` | Optional [ImGuiPopupFlags](./ImGuiPopupFlags.md) |

## `IsPopupOpen`

```lua
function ImGui.IsPopupOpen(name, flags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` | Popup name |
| `flags` | `integer?` | Optional [ImGuiPopupFlags](./ImGuiPopupFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `boolean` |  |

## `CloseCurrentPopup`

```lua
function ImGui.CloseCurrentPopup() end
```

## `BeginPopup`

```lua
function ImGui.BeginPopup(name, windowFlags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` |  |
| `windowFlags` | `integer?` | Optional [ImGuiWindowFlags](./ImGuiWindowFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | isOpen |

## `BeginPopupModal`

```lua
function ImGui.BeginPopupModal(name, windowFlags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` |  |
| `windowFlags` | `integer?` | Optional [ImGuiWindowFlags](./ImGuiWindowFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | visible |

## `BeginPopupModal`

```lua
function ImGui.BeginPopupModal(name, open, windowFlags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` |  |
| `open` | `boolean` | When true, the popup will have a `(x)` close button in the title bar. |
| `windowFlags` | `integer?` | Optional [ImGuiWindowFlags](./ImGuiWindowFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | visible |
| `boolean` | open |

## `EndPopup`

```lua
function ImGui.EndPopup() end
```

## `SliderInt`

```lua
function ImGui.SliderInt(label, v, vMin, vMax) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `integer` |  |
| `vMin` | `integer` | Minimum value |
| `vMax` | `integer` | Maximum value |

### Returns

| Type | Description |
| --- | --- |
| `integer` | v |
| `boolean` | changed |

## `SliderInt`

```lua
function ImGui.SliderInt(label, v, vMin, vMax, fmt, flags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `integer` |  |
| `vMin` | `integer` | Minimum value |
| `vMax` | `integer` | Maximum value |
| `fmt` | `string` | format string |
| `flags` | `integer?` | Optional [ImGuiSliderFlags](./ImGuiSliderFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `integer` | v |
| `boolean` | changed |

## `SliderFloat`

```lua
function ImGui.SliderFloat(label, v, vMin, vMax) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `number` |  |
| `vMin` | `number` | Minimum value |
| `vMax` | `number` | Maximum value |

### Returns

| Type | Description |
| --- | --- |
| `number` | v |
| `boolean` | changed |

## `SliderFloat`

```lua
function ImGui.SliderFloat(label, v, vMin, vMax, fmt, flags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `number` |  |
| `vMin` | `number` | Minimum value |
| `vMax` | `number` | Maximum value |
| `fmt` | `string` | format string |
| `flags` | `integer?` | Optional [ImGuiSliderFlags](./ImGuiSliderFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `number` | v |
| `boolean` | changed |

## `InputInt`

```lua
function ImGui.InputInt(label, v) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `integer` | v |
| `boolean` | changed |

## `InputInt`

```lua
function ImGui.InputInt(label, v, step, stepFast, flags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `integer` |  |
| `step` | `integer` | step |
| `stepFast` | `integer` | fast step |
| `flags` | `integer?` | Optional [ImGuiInputTextFlags](./ImGuiInputTextFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `integer` | v |
| `boolean` | changed |

## `InputFloat`

```lua
function ImGui.InputFloat(label, v) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `number` |  |

### Returns

| Type | Description |
| --- | --- |
| `number` | v |
| `boolean` | changed |

## `InputFloat`

```lua
function ImGui.InputFloat(label, v, step, stepFast, fmt) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `number` |  |
| `step` | `number` | step |
| `stepFast` | `number` | fast step |
| `fmt` | `string?` | Optional format string |

### Returns

| Type | Description |
| --- | --- |
| `number` | v |
| `boolean` | changed |

## `InputFloat`

```lua
function ImGui.InputFloat(label, v, step, stepFast, fmt, flags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `v` | `number` |  |
| `step` | `number` | step |
| `stepFast` | `number` | fast step |
| `fmt` | `string` | format string |
| `flags` | `integer?` | Optional [ImGuiInputTextFlags](./ImGuiInputTextFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `number` | v |
| `boolean` | changed |

## `InputText`

```lua
function ImGui.InputText(label, text, bufferSize, flags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `text` | `string` |  |
| `bufferSize` | `integer` |  |
| `flags` | `integer?` | Optional [ImGuiInputTextFlags](./ImGuiInputTextFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `string` | text |
| `boolean` | changed |

## `InputTextWithHint`

```lua
function ImGui.InputTextWithHint(label, hint, text, bufferSize, flags) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `hint` | `string` |  |
| `text` | `string` |  |
| `bufferSize` | `integer` |  |
| `flags` | `integer?` | Optional [ImGuiInputTextFlags](./ImGuiInputTextFlags.md) |

### Returns

| Type | Description |
| --- | --- |
| `string` | text |
| `boolean` | changed |

## `InputTextMultiline`

```lua
function ImGui.InputTextMultiline(label, text, bufferSize) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `text` | `string` |  |
| `bufferSize` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `string` | text |
| `boolean` | changed |

## `InputTextMultiline`

```lua
function ImGui.InputTextMultiline(label, text, bufferSize, flags, size) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `label` | `string` |  |
| `text` | `string` |  |
| `bufferSize` | `integer` |  |
| `flags` | `integer` | [ImGuiInputTextFlags](./ImGuiInputTextFlags.md) |
| `size` | `ImVec2?` | Optional widget size |

### Returns

| Type | Description |
| --- | --- |
| `string` | text |
| `boolean` | changed |

## `SameLine`

```lua
function ImGui.SameLine() end
```

## `SameLine`

```lua
function ImGui.SameLine(offsetX, spacing) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `offsetX` | `number` | X position to start at. Defaults to `0`. |
| `spacing` | `number?` | Optional spacing after the previous item. Defaults to the current item spacing. |

## `NewLine`

```lua
function ImGui.NewLine() end
```

## `Spacing`

```lua
function ImGui.Spacing() end
```

## `Separator`

```lua
function ImGui.Separator() end
```

## `SeparatorEx`

```lua
function ImGui.SeparatorEx(flags, thickness) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `flags` | `integer` | [ImGuiSeparatorFlags](./ImGuiSeparatorFlags.md) |
| `thickness` | `number?` | Optional separator thickness. Defaults to 1.0 |

## `SeparatorText`

```lua
function ImGui.SeparatorText(text) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` |  |

## `Indent`

```lua
function ImGui.Indent(indentX) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `indentX` | `number?` | Optional indentation width. Defaults to the current item spacing. |

## `Unindent`

```lua
function ImGui.Unindent(indentX) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `indentX` | `number?` | Optional indentation width. Defaults to the current item spacing. |

## `Dummy`

```lua
function ImGui.Dummy(sizeX, sizeY) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `sizeX` | `number` | Width of the dummy item. |
| `sizeY` | `number` | Height of the dummy item. |

## `SetCursorPos`

```lua
function ImGui.SetCursorPos(x, y) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `x` | `number` | X position relative to the current window. |
| `y` | `number` | Y position relative to the current window. |

## `SetCursorPosX`

```lua
function ImGui.SetCursorPosX(x) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `x` | `number` | X position relative to the current window. |

## `SetCursorPosY`

```lua
function ImGui.SetCursorPosY(y) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `y` | `number` | Y position relative to the current window. |

## `GetCursorPos`

```lua
function ImGui.GetCursorPos() end
```

### Returns

| Type | Description |
| --- | --- |
| `ImVec2` | cursorPos |

## `GetCursorPosX`

```lua
function ImGui.GetCursorPosX() end
```

### Returns

| Type | Description |
| --- | --- |
| `number` | x Current X position relative to the current window. |

## `GetCursorPosY`

```lua
function ImGui.GetCursorPosY() end
```

### Returns

| Type | Description |
| --- | --- |
| `number` | y Current Y position relative to the current window. |

## `GetContentRegionAvail`

```lua
function ImGui.GetContentRegionAvail() end
```

### Returns

| Type | Description |
| --- | --- |
| `ImVec2` | avail Available content region. |

## `GetWindowSize`

```lua
function ImGui.GetWindowSize() end
```

### Returns

| Type | Description |
| --- | --- |
| `ImVec2` | windowSize |

## `GetWindowPos`

```lua
function ImGui.GetWindowPos() end
```

### Returns

| Type | Description |
| --- | --- |
| `ImVec2` | windowPos Current window position. |

## `GetWindowWidth`

```lua
function ImGui.GetWindowWidth() end
```

### Returns

| Type | Description |
| --- | --- |
| `number` | width Current window width |

## `GetWindowHeight`

```lua
function ImGui.GetWindowHeight() end
```

### Returns

| Type | Description |
| --- | --- |
| `number` | height Current window height |

## `GetWindowDrawList`

```lua
function ImGui.GetWindowDrawList() end
```

### Returns

| Type | Description |
| --- | --- |
| `ImDrawList` |  |

## `GetBackgroundDrawList`

```lua
function ImGui.GetBackgroundDrawList() end
```

### Returns

| Type | Description |
| --- | --- |
| `ImDrawList` |  |

## `GetForegroundDrawList`

```lua
function ImGui.GetForegroundDrawList() end
```

### Returns

| Type | Description |
| --- | --- |
| `ImDrawList` |  |

## `GetFrameHeight`

```lua
function ImGui.GetFrameHeight() end
```

### Returns

| Type | Description |
| --- | --- |
| `number` | frameHeight |

## `GetFrameHeightWithSpacing`

```lua
function ImGui.GetFrameHeightWithSpacing() end
```

### Returns

| Type | Description |
| --- | --- |
| `number` | frameHeight Frame height with spacing. |

## `GetTime`

```lua
function ImGui.GetTime() end
```

### Returns

| Type | Description |
| --- | --- |
| `number` | frameTime |

## `SetWindowSize`

```lua
function ImGui.SetWindowSize(size, cond) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `size` | `ImVec2` | Window size |
| `cond` | `integer?` | Optional [ImGuiCond](./ImGuiCond.md) |

## `SetWindowSize`

```lua
function ImGui.SetWindowSize(name, size, cond) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` | Window name |
| `size` | `ImVec2` | Window size |
| `cond` | `integer?` | Optional [ImGuiCond](./ImGuiCond.md) |

## `SetWindowPos`

```lua
function ImGui.SetWindowPos(pos, cond) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `pos` | `ImVec2` | New position |
| `cond` | `integer?` | Optional [ImGuiCond](./ImGuiCond.md) |

## `SetWindowPos`

```lua
function ImGui.SetWindowPos(name, pos, cond) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `name` | `string` | Window name |
| `pos` | `ImVec2` | New position |
| `cond` | `integer?` | Optional [ImGuiCond](./ImGuiCond.md) |

## `SetNextWindowSize`

```lua
function ImGui.SetNextWindowSize(pos, cond) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `pos` | `ImVec2` |  |
| `cond` | `integer?` | Optional [ImGuiCond](./ImGuiCond.md) |

## `SetNextWindowSize`

```lua
function ImGui.SetNextWindowSize(size, cond) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `size` | `ImVec2` |  |
| `cond` | `integer?` | Optional [ImGuiCond](./ImGuiCond.md) |

## `SetNextWindowSizeConstraints`

```lua
function ImGui.SetNextWindowSizeConstraints(minSize, maxSize) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `minSize` | `ImVec2` |  |
| `maxSize` | `ImVec2` |  |

## `SetNextItemWidth`

```lua
function ImGui.SetNextItemWidth(width) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `width` | `number` | Width of the next item. |

## `PushItemWidth`

```lua
function ImGui.PushItemWidth(width) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `width` | `number` | Width of subsequent items. |

## `PopItemWidth`

```lua
function ImGui.PopItemWidth() end
```

## `SetNextWindowSize`

```lua
function ImGui.SetNextWindowSize() end
```

## `IsWindowAppearing`

```lua
function ImGui.IsWindowAppearing() end
```

### Returns

| Type | Description |
| --- | --- |
| `boolean` |  |

## `IsWindowCollapsed`

```lua
function ImGui.IsWindowCollapsed() end
```

### Returns

| Type | Description |
| --- | --- |
| `boolean` |  |

## `PushStyleVar`

```lua
function ImGui.PushStyleVar(styleVar, v) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `styleVar` | `integer` | [ImGuiStyleVar](./ImGuiStyleVar.md) index |
| `v` | `number` | style value |

## `PushStyleVar`

```lua
function ImGui.PushStyleVar(styleVar, vX, vY) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `styleVar` | `integer` | [ImGuiStyleVar](./ImGuiStyleVar.md) index |
| `vX` | `number` | x value |
| `vY` | `number` | y value |

## `PopStyleVar`

```lua
function ImGui.PopStyleVar(count) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `count` | `integer?` | Count of pushed style variables to pop. Defaults to 1; Must match the number of pushed style vars. |

## `PushStyleColor`

```lua
function ImGui.PushStyleColor(colIdx, r, g, b, a) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `colIdx` | `integer` | [ImGuiCol](./ImGuiCol.md) index |
| `r` | `number` | A number between `0.0` and `1.0` representing the `Red` component of the color. |
| `g` | `number` | A number between `0.0` and `1.0` representing the `Green` component of the color. |
| `b` | `number` | A number between `0.0` and `1.0` representing the `Blue` component of the color. |
| `a` | `number` | A number between `0.0` and `1.0` representing the `Alpha` component of the color. |

## `PopStyleColor`

```lua
function ImGui.PopStyleColor(count) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `count` | `integer?` | Count of pushed style colors to pop. Defaults to 1; Must match the number of pushed style cols. |

## `SetMouseCursor`

```lua
function ImGui.SetMouseCursor(cursorIdx) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `cursorIdx` | `integer` | [ImGuiMouseCursor](./ImGuiMouseCursor.md) index |

## `SetTooltip`

```lua
function ImGui.SetTooltip(text, ...) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` |  |
| `...` | `any` | Optional format args |

## `BeginTooltip`

```lua
function ImGui.BeginTooltip() end
```

## `EndTooltip`

```lua
function ImGui.EndTooltip() end
```

## `BeginGroup`

```lua
function ImGui.BeginGroup() end
```

## `EndGroup`

```lua
function ImGui.EndGroup() end
```

## `Tooltip`

```lua
function ImGui.Tooltip(text) end
```

Draws a tooltip when the widget this function is called after is hovered.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` | Tooltip text |

## `HelpMarker`

```lua
function ImGui.HelpMarker(text) end
```

Draws a help marker (?) symbol in front of the widget this function is called after. When the symbol is hovered, it draws a tooltip.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` | Tooltip text |

## `WarningMarker`

```lua
function ImGui.WarningMarker(text) end
```

Draws a warning marker [!] symbol in front of the widget this function is called after. When the symbol is hovered, it draws a tooltip.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` | Tooltip text |

## `Spinner`

```lua
function ImGui.Spinner() end
```

Draws a spinner indicating 'busy' state.

## `Spinner`

```lua
function ImGui.Spinner(text, radius) end
```

Draws a spinner indicating 'busy' state.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` | Text to display next to the spinner. Can be an empty or anonymous string: "##blahblah" |
| `radius` | `number?` | Optional spinner radius. Defaults to 10.0 |

## `Spinner`

```lua
function ImGui.Spinner(text, radius, thickness) end
```

Draws a spinner indicating 'busy' state.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` | Text to display next to the spinner. Can be an empty or anonymous string: "##blahblah" |
| `radius` | `number` | spinner radius. Defaults to 10.0 |
| `thickness` | `number?` | Optional spinner line thickness. Defaults to 2.0 |

## `YLPSpinner`

```lua
function ImGui.YLPSpinner() end
```

Draws a spinner made out of YLP's logo with a rotating gear and a fixed label.

## `YLPSpinner`

```lua
function ImGui.YLPSpinner(text, radius) end
```

Draws a spinner made out of YLP's logo with a rotating gear and a fixed label.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` | Text to display below the spinner. Can be an empty or anonymous string: "##blahblah" |
| `radius` | `number?` | Optional spinner radius. Defaults to frame height. |

## `YLPSpinner`

```lua
function ImGui.YLPSpinner(text, radius, speed) end
```

Draws a spinner made out of YLP's logo with a rotating gear and a fixed label.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `text` | `string` | Text to display below the spinner. Can be an empty or anonymous string: "##blahblah" |
| `radius` | `number` | spinner radius. Defaults to frame height. |
| `speed` | `number?` | Optional spinning speed. Defaults to 2.8 |

## `SegmentedControl`

```lua
function ImGui.SegmentedControl(currentItem, items, anchorPos) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `currentItem` | `integer` | The current item |
| `items` | `{[integer]: string}` | A table of strings. Must be a contiguous string array, anything else will be ignored. |
| `anchorPos` | `(0|1|2)?` | Anchor position. 0: left \| 1: center \| 2: right. Defaults to 0: left. |

### Returns

| Type | Description |
| --- | --- |
| `integer` | selectedItem |
| `boolean` | clicked |
