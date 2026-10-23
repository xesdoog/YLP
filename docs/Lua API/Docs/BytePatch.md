<!-- markdownlint-disable -->

# BytePatch

Provides an abstraction around byte patching.

It is exclusively returned by [Pointer](./Pointer.md) and can not be explicitly constructed.

## Methods

## `Apply`

```lua
function BytePatch:Apply() end
```

Applies the patch to the target pointer's memory.

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success Success or failure. |

## `Restore`

```lua
function BytePatch:Restore() end
```

Restores the target pointer's original bytes.

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success Success or failure. |

## `IsApplied`

```lua
function BytePatch:IsApplied() end
```

Whether this patch is currently applied.

### Returns

| Type | Description |
| --- | --- |
| `boolean` |  |
