---@meta

-- Provides basic memory read/write functions.
-- 
-- It is exclusively returned by [Process](lua://Process) and can not be explicitly constructed.
---@class Pointer
---@operator add(integer): Pointer
---@operator sub(integer): Pointer
Pointer = {}

---@param offset integer The offset to add
---@return Pointer ptr A new pointer at the new address.
function Pointer:Add(offset) end

---@param offset integer The offset to subtract.
---@return Pointer ptr A new pointer at the new address.
function Pointer:Sub(offset) end

---@return Pointer ptr A new pointer at the resolved RIP-relative address.
function Pointer:Rip() end

---@return Pointer ptr A new pointer at the dereferenced address.
function Pointer:Dereference() end

---@return integer address The pointer's memory address.
function Pointer:GetAddress() end

---@return boolean 
function Pointer:IsNull() end

-- Creates a `BytePatch` object.
---@param byteArray {[integer]: integer} An array of bytes. Example: `{ 0x90, 0x90, 0x90 }`
---@param applyImmediately boolean? Optional: Apply the patch immediately.
---@return BytePatch? patch A `BytePatch` object or nil
function Pointer:AddBytePatch(byteArray, applyImmediately) end

-- Reads up to `n` number of bytes
---@param numOfBytes integer Number of bytes to read.
---@return table<integer,integer> byteArray An array of bytes
function Pointer:ReadBytes(numOfBytes) end

---@return string 
function Pointer:ReadString() end

-- Reads a signed 8bit integer
---@return integer s8 value
function Pointer:ReadInt8() end

-- Reads an unsigned 8bit integer
---@return integer u8 value
function Pointer:ReadUint8() end

-- Reads a signed 16bit integer
---@return integer s16 value
function Pointer:ReadInt16() end

-- Reads an unsigned 16bit integer
---@return integer u16 value
function Pointer:ReadUint16() end

-- Reads a signed 32bit integer
---@return integer s32 value
function Pointer:ReadInt32() end

-- Reads an unsigned 32bit integer
---@return integer u32 value
function Pointer:ReadUint32() end

-- Reads a signed 64bit integer
---@return integer s64 value
function Pointer:ReadInt64() end

-- Reads an unsigned 64bit integer
---@return integer u64 value
function Pointer:ReadUint64() end

-- Reads a float
---@return number 
function Pointer:ReadFloat() end

-- Reads a double
---@return number 
function Pointer:ReadDouble() end

---@param byteArray {[integer]: integer} An array of bytes.
---@return boolean success
function Pointer:WriteBytes(byteArray) end

---@param str string 
---@return boolean success
function Pointer:WriteString(str) end

---@param s8 integer 
---@return boolean success
function Pointer:WriteInt8(s8) end

---@param u8 integer 
---@return boolean success
function Pointer:WriteUint8(u8) end

---@param s16 integer 
---@return boolean success
function Pointer:WriteInt16(s16) end

---@param u16 integer 
---@return boolean success
function Pointer:WriteUint16(u16) end

---@param s32 integer 
---@return boolean success
function Pointer:WriteInt32(s32) end

---@param u32 integer 
---@return boolean success
function Pointer:WriteUint32(u32) end

---@param s64 integer 
---@return boolean success
function Pointer:WriteInt64(s64) end

---@param u64 integer 
---@return boolean success
function Pointer:WriteUint64(u64) end

---@param v number 
---@return boolean success
function Pointer:WriteFloat(v) end

---@param v number 
---@return boolean 
function Pointer:WriteDouble(v) end
