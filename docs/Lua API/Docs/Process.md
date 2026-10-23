<!-- markdownlint-disable -->

# Process

A process abstraction providing basic process interactions.

## Constructors

```lua
Process(processName)
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `processName` | `string` |  |
## Methods

## `IsRunning`

```lua
function Process:IsRunning() end
```

### Returns

| Type | Description |
| --- | --- |
| `boolean` |  |

## `IsModuleLoaded`

```lua
function Process:IsModuleLoaded(moduleName) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `moduleName` | `string` | Name of the module. Example: `"kernelbase.dll"` |

### Returns

| Type | Description |
| --- | --- |
| `boolean` |  |

## `GetModuleSize`

```lua
function Process:GetModuleSize() end
```

### Returns

| Type | Description |
| --- | --- |
| `integer` |  |

## `GetModuleBase`

```lua
function Process:GetModuleBase() end
```

### Returns

| Type | Description |
| --- | --- |
| `integer` | -- The module's base address |

## `Update`

```lua
function Process:Update() end
```

Updates the process object's internal state. This is useful because the `Process` class does not poll for status on its own.

## `FindPattern`

```lua
function Process:FindPattern(pattern, name, chunkSize) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `pattern` | `string` | IDA-style byte signature. Example: `"8B 88 C0 ?? ?? ?? 89 8F"` |
| `name` | `string?` | Optional name of the pattern to scan. Purely for logging purposes. |
| `chunkSize` | `integer?` | Optional memory chunk size. Defaults to 4096 |

### Returns

| Type | Description |
| --- | --- |
| `Pointer` | ptr A pointer at the found address or a null pointer if the scan fails. This is guaranteed to always return a Pointer object. |
