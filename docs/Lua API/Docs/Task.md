<!-- markdownlint-disable -->

# Task

Provides functions to run tasks in YLP's thread.

## Methods

## `Run`

```lua
function Task.Run(callback) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `callback` | `function` | The callback to execute |

## `RunDelayed`

```lua
function Task.RunDelayed(delay, callback) end
```

Execute a callback after a delay

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `delay` | `integer` | Delay in milliseconds |
| `callback` | `function` | The function to execute |

## `Sleep`

```lua
function Task.Sleep(ms) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `ms` | `integer` | Sleep time in milliseconds |

## `Yield`

```lua
function Task.Yield(ms) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `ms` | `integer?` | Optional yield time in milliseconds |
