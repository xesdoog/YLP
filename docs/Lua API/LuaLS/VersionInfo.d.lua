---@meta

-- A user type that stores version information with support for direct equality comparisons.
-- 
-- __Usage Example:__
-- 
-- ```lua
-- local version_min = VersionInfo(2, 0, 1, 4)
-- if (YLP.GetVersion() < version_min) then
-- YLP.UnloadThisModule()
-- end
-- ```
---@class VersionInfo
---@overload fun(major: integer, minor: integer, patch: integer, build: integer): VersionInfo
---@field major integer 
---@field minor integer 
---@field patch integer 
---@field build integer 
VersionInfo = {}

---@return string strVer The string representation of the current version. Ex: `"1.2.3.4"`
function VersionInfo:ToString() end
