---@meta

---@class ImGuiCond : integer
---@field None integer No condition (always set the variable), same as _Always
---@field Always integer No condition (always set the variable), same as _None
---@field Once integer Set the variable once per runtime session (only the first call will succeed)
---@field FirstUseEver integer Set the variable if the object/window has no persistently saved data (no entry in .ini file)
---@field Appearing integer Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
ImGuiCond = {}
