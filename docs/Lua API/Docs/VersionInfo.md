<!-- markdownlint-disable -->

# VersionInfo

A user type that stores version information with support for direct equality comparisons.

__Usage Example:__

```lua
local version_min = VersionInfo(2, 0, 1, 4)
if (YLP.GetVersion() < version_min) then
YLP.UnloadThisModule()
end
```

## Constructors

```lua
VersionInfo(major, minor, patch, build)
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `major` | `integer` |  |
| `minor` | `integer` |  |
| `patch` | `integer` |  |
| `build` | `integer` |  |
## Fields

| Name | Type | Description |
| --- | --- | --- |
| `major` | `integer` |  |
| `minor` | `integer` |  |
| `patch` | `integer` |  |
| `build` | `integer` |  |

## Methods

## `ToString`

```lua
function VersionInfo:ToString() end
```

### Returns

| Type | Description |
| --- | --- |
| `string` | strVer The string representation of the current version. Ex: `"1.2.3.4"` |
