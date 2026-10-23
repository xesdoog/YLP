---@meta

---@class VersionInfo
---@overload fun(major: integer, minor: integer, patch: integer, build: integer): VersionInfo
---@field major integer 
---@field minor integer 
---@field patch integer 
---@field build integer 
VersionInfo = {}

---@return string strVer The string representation of the current version. Ex: `"1.2.3.4"`
function VersionInfo:ToString() end
