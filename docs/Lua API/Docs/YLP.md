<!-- markdownlint-disable -->

# YLP

### YLP namespace

## Methods

## `GetVersion`

```lua
function YLP.GetVersion() end
```

### Returns

| Type | Description |
| --- | --- |
| `VersionInfo` | versionInfo The current YLP version. |

## `IsDebug`

```lua
function YLP.IsDebug() end
```

### Returns

| Type | Description |
| --- | --- |
| `boolean` | isDebug True if the current build type is debug, otherwise false. |

## `RegisterProcessWatcher`

```lua
function YLP.RegisterProcessWatcher(processName, callback, delay) end
```

Registers a callback to be executed once when a process is first seen.

You can call `Task.Yield` and `Task.Sleep` in your callback function.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `processName` | `string` | The name of the process |
| `callback` | `fun(process: Process)` | The function to execute. YLP will pass a Process object to the function as an argument. |
| `delay` | `integer?` | Optional delay in milliseconds |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success Whether the registration was successful or not. |

## `RegisterGui`

```lua
function YLP.RegisterGui(callback) end
```

Registers an ImGui callback to be drawn in the 'Lua Scripting' tab.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `callback` | `function` | The UI to draw. ImGui functions can only be called here. |

## `InjectDll`

```lua
function YLP.InjectDll(dllPath, processName) end
```

Injects a dynamic link library into a target process.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `dllPath` | `Path` | DLL file path. Must be a [Path](./Path.md) object. |
| `processName` | `string` | Name of the target process. |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | status Success or failure. |
| `string?` | failReason Optional error message if injection fails. |

## `InjectDll`

```lua
function YLP.InjectDll(dllPath, processName, manualMap, manualMapArgs) end
```

Injects a dynamic link library into a target process.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `dllPath` | `Path` | DLL file path. Must be a [Path](./Path.md) object. |
| `processName` | `string` | Name of the target process. |
| `manualMap` | `boolean` | Use manual mapping instead of standard `LoadLibrary` |
| `manualMapArgs` | `{ eraseHeaders: boolean?, enableSEH: boolean?, randomizeBaseAddress: boolean?}?` | Optional manual mapping configuration. |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | status Success or failure. |
| `string?` | failReason Optional error message if injection fails. |

## `OnShutdown`

```lua
function YLP.OnShutdown(callback) end
```

Registers a function to be executed when YLP is shutting down.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `callback` | `function` | The function to execute |

## `UnloadThisModule`

```lua
function YLP.UnloadThisModule() end
```

Unloads the caller module.

The module can only be loaded again from the Settings tab in YLP's UI.
