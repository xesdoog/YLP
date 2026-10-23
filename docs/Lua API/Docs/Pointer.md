<!-- markdownlint-disable -->

# Pointer

Provides basic memory read/write functions.

It is exclusively returned by [Process](./Process.md) and can not be explicitly constructed.

## Operators

### `add`

```lua
---@operator __add(integer): Pointer
```

### `sub`

```lua
---@operator __sub(integer): Pointer
```

## Methods

## `Add`

```lua
function Pointer:Add(offset) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `offset` | `integer` | The offset to add |

### Returns

| Type | Description |
| --- | --- |
| `Pointer` | ptr A new pointer at the new address. |

## `Sub`

```lua
function Pointer:Sub(offset) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `offset` | `integer` | The offset to subtract. |

### Returns

| Type | Description |
| --- | --- |
| `Pointer` | ptr A new pointer at the new address. |

## `Rip`

```lua
function Pointer:Rip() end
```

### Returns

| Type | Description |
| --- | --- |
| `Pointer` | ptr A new pointer at the resolved RIP-relative address. |

## `Dereference`

```lua
function Pointer:Dereference() end
```

### Returns

| Type | Description |
| --- | --- |
| `Pointer` | ptr A new pointer at the dereferenced address. |

## `GetAddress`

```lua
function Pointer:GetAddress() end
```

### Returns

| Type | Description |
| --- | --- |
| `integer` | address The pointer's memory address. |

## `IsNull`

```lua
function Pointer:IsNull() end
```

### Returns

| Type | Description |
| --- | --- |
| `boolean` |  |

## `AddBytePatch`

```lua
function Pointer:AddBytePatch(byteArray, applyImmediately) end
```

Creates a `BytePatch` object.

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `byteArray` | `{[integer]: integer}` | An array of bytes. Example: `{ 0x90, 0x90, 0x90 }` |
| `applyImmediately` | `boolean?` | Optional: Apply the patch immediately. |

### Returns

| Type | Description |
| --- | --- |
| `BytePatch?` | patch A `BytePatch` object or nil |

## `ReadBytes`

```lua
function Pointer:ReadBytes(numOfBytes) end
```

Reads up to `n` number of bytes

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `numOfBytes` | `integer` | Number of bytes to read. |

### Returns

| Type | Description |
| --- | --- |
| `table<integer,integer>` | byteArray An array of bytes |

## `ReadString`

```lua
function Pointer:ReadString() end
```

### Returns

| Type | Description |
| --- | --- |
| `string` |  |

## `ReadInt8`

```lua
function Pointer:ReadInt8() end
```

Reads a signed 8bit integer

### Returns

| Type | Description |
| --- | --- |
| `integer` | s8 value |

## `ReadUint8`

```lua
function Pointer:ReadUint8() end
```

Reads an unsigned 8bit integer

### Returns

| Type | Description |
| --- | --- |
| `integer` | u8 value |

## `ReadInt16`

```lua
function Pointer:ReadInt16() end
```

Reads a signed 16bit integer

### Returns

| Type | Description |
| --- | --- |
| `integer` | s16 value |

## `ReadUint16`

```lua
function Pointer:ReadUint16() end
```

Reads an unsigned 16bit integer

### Returns

| Type | Description |
| --- | --- |
| `integer` | u16 value |

## `ReadInt32`

```lua
function Pointer:ReadInt32() end
```

Reads a signed 32bit integer

### Returns

| Type | Description |
| --- | --- |
| `integer` | s32 value |

## `ReadUint32`

```lua
function Pointer:ReadUint32() end
```

Reads an unsigned 32bit integer

### Returns

| Type | Description |
| --- | --- |
| `integer` | u32 value |

## `ReadInt64`

```lua
function Pointer:ReadInt64() end
```

Reads a signed 64bit integer

### Returns

| Type | Description |
| --- | --- |
| `integer` | s64 value |

## `ReadUint64`

```lua
function Pointer:ReadUint64() end
```

Reads an unsigned 64bit integer

### Returns

| Type | Description |
| --- | --- |
| `integer` | u64 value |

## `ReadFloat`

```lua
function Pointer:ReadFloat() end
```

Reads a float

### Returns

| Type | Description |
| --- | --- |
| `number` |  |

## `ReadDouble`

```lua
function Pointer:ReadDouble() end
```

Reads a double

### Returns

| Type | Description |
| --- | --- |
| `number` |  |

## `WriteBytes`

```lua
function Pointer:WriteBytes(byteArray) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `byteArray` | `{[integer]: integer}` | An array of bytes. |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteString`

```lua
function Pointer:WriteString(str) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `str` | `string` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteInt8`

```lua
function Pointer:WriteInt8(s8) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `s8` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteUint8`

```lua
function Pointer:WriteUint8(u8) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `u8` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteInt16`

```lua
function Pointer:WriteInt16(s16) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `s16` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteUint16`

```lua
function Pointer:WriteUint16(u16) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `u16` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteInt32`

```lua
function Pointer:WriteInt32(s32) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `s32` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteUint32`

```lua
function Pointer:WriteUint32(u32) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `u32` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteInt64`

```lua
function Pointer:WriteInt64(s64) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `s64` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteUint64`

```lua
function Pointer:WriteUint64(u64) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `u64` | `integer` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteFloat`

```lua
function Pointer:WriteFloat(v) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `v` | `number` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` | success |

## `WriteDouble`

```lua
function Pointer:WriteDouble(v) end
```

### Parameters

| Name | Type | Description |
| --- | --- | --- |
| `v` | `number` |  |

### Returns

| Type | Description |
| --- | --- |
| `boolean` |  |
