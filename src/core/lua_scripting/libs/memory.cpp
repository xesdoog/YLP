// Copyright (C) 2025 SAMURAI (xesdoog) & Contributors
// This file is part of YLP.
//
// YLP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// YLP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with YLP.  If not, see <https://www.gnu.org/licenses/>.


#pragma once

#include "../lua_library.hpp"
#include "../lua_module.hpp"
#include "../../memory/scanner.hpp"
#include "../../memory/pointer.hpp"
#include "../../memory/byte_patch.hpp"


namespace YLP::LuaJIT
{
	using namespace Memory;

	class LuaMemLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	public:
		void Register(sol::state& L) override
		{
			// clang-format off

			/*@ylp.class BytePatch
			* description
				Provides an abstraction around byte patching.

				It is exclusively returned by [Pointer](lua://Pointer) and can not be explicitly constructed.

			* method Apply Applies the patch to the target pointer's memory.
			* return boolean success Success or failure.

			* method Restore Restores the target pointer's original bytes.
			* return boolean success Success or failure.

			* method IsApplied Whether this patch is currently applied.
			* return boolean
			@*/
			auto bytePatchUt = L.new_usertype<BytePatch>("BytePatch", sol::no_constructor,
				"Apply",     &BytePatch::Apply,
				"Restore",   &BytePatch::Restore,
				"IsApplied", &BytePatch::IsApplied
			);

			/* @ylp.class Pointer
			* description
				Provides basic memory read/write functions.

				It is exclusively returned by [Process](lua://Process) and can not be explicitly constructed.

			* operator add
			* param offset<integer>
			* return Pointer

			* operator sub
			* param offset<integer>
			* return Pointer

			* method Add
			* param offset<integer> The offset to add
			* return Pointer ptr A new pointer at the new address.

			* method Sub
			* param offset<integer> The offset to subtract.
			* return Pointer ptr A new pointer at the new address.

			* method Rip
			* return Pointer ptr A new pointer at the resolved RIP-relative address.

			* method Dereference
			* return Pointer ptr A new pointer at the dereferenced address.

			* method GetAddress
			* return integer address The pointer's memory address.

			* method IsNull
			* return boolean

			* method AddBytePatch Creates a `BytePatch` object.
			* param byteArray<{[integer]: integer}> An array of bytes. Example: `{ 0x90, 0x90, 0x90 }`
			* param applyImmediately<boolean?> Optional: Apply the patch immediately.
			* return BytePatch? patch A `BytePatch` object or nil 
			
			* method ReadBytes Reads up to `n` number of bytes
			* param numOfBytes<integer> Number of bytes to read.
			* return table<integer,integer> byteArray An array of bytes

			* method ReadString
			* return string

			* method ReadInt8 Reads a signed 8bit integer
			* return integer s8 value

			* method ReadUint8 Reads an unsigned 8bit integer
			* return integer u8 value

			* method ReadInt16 Reads a signed 16bit integer
			* return integer s16 value

			* method ReadUint16 Reads an unsigned 16bit integer
			* return integer u16 value

			* method ReadInt32 Reads a signed 32bit integer
			* return integer s32 value

			* method ReadUint32 Reads an unsigned 32bit integer
			* return integer u32 value

			* method ReadInt64 Reads a signed 64bit integer
			* return integer s64 value

			* method ReadUint64 Reads an unsigned 64bit integer
			* return integer u64 value

			* method ReadFloat Reads a float
			* return number

			* method ReadDouble Reads a double
			* return number

			* method WriteBytes
			* param byteArray<{[integer]: integer}> An array of bytes.
			* return boolean success

			* method WriteString
			* param str<string>
			* return boolean success

			* method WriteInt8
			* param s8<integer>
			* return boolean success
			
			* method WriteUint8
			* param u8<integer>
			* return boolean success

			* method WriteInt16
			* param s16<integer>
			* return boolean success

			* method WriteUint16
			* param u16<integer>
			* return boolean success

			* method WriteInt32
			* param s32<integer>
			* return boolean success

			* method WriteUint32
			* param u32<integer>
			* return boolean success

			* method WriteInt64
			* param s64<integer>
			* return boolean success

			* method WriteUint64
			* param u64<integer>
			* return boolean success

			* method WriteFloat
			* param v<number>
			* return boolean success

			* method WriteDouble
			* param v<number>
			* return boolean 
			@*/
			auto ptrUt = L.new_usertype<Pointer>("Pointer", sol::no_constructor,
				sol::meta_function::addition,    &Pointer::Add,
				sol::meta_function::subtraction, &Pointer::Sub,

				"Add",         &Pointer::Add,
				"Sub",         &Pointer::Sub,
				"Rip",         &Pointer::Rip,
				"Dereference", &Pointer::Dereference,
				"GetAddress",  &Pointer::GetAddress
			);

			// clang-format on

			ptrUt["IsNull"] = [](Pointer& self) {
				return self.GetAddress() == 0;
			};

			ptrUt["ReadBytes"] = [](Pointer& self, size_t numOfBytes) {
				return self.ReadBytes(numOfBytes > 0 ? numOfBytes : 4);
			};

			ptrUt["ReadString"] = [](Pointer& self, sol::optional<size_t> maxLength) {
				return self.Read<std::string>(maxLength.value_or(64));
			};

			ptrUt["ReadInt8"] = [](Pointer& self) {
				return self.Read<int8_t>();
			};

			ptrUt["ReadUint8"] = [](Pointer& self) {
				return self.Read<uint8_t>();
			};

			ptrUt["ReadInt16"] = [](Pointer& self) {
				return self.Read<int16_t>();
			};

			ptrUt["ReadUint16"] = [](Pointer& self) {
				return self.Read<uint16_t>();
			};

			ptrUt["ReadInt32"] = [](Pointer& self) {
				return self.Read<int32_t>();
			};

			ptrUt["ReadUint32"] = [](Pointer& self) {
				return self.Read<uint32_t>();
			};

			ptrUt["ReadInt64"] = [](Pointer& self) {
				return self.Read<int64_t>();
			};

			ptrUt["ReadUint64"] = [](Pointer& self) {
				return self.Read<uint64_t>();
			};

			ptrUt["ReadFloat"] = [](Pointer& self) {
				return self.Read<float>();
			};

			ptrUt["ReadDouble"] = [](Pointer& self) {
				return self.Read<double>();
			};

			ptrUt["WriteBytes"] = [](Pointer& self, sol::table tbl) {
				std::vector<uint8_t> temp;
				for (std::size_t i = 1; i <= tbl.size(); ++i)
				{
					uint8_t val = tbl[i].get_or<uint8_t>(0);
					temp.push_back(val);
				}
				return self.WriteBytes(std::span<uint8_t>(temp));
			};

			ptrUt["WriteString"] = [](Pointer& self, std::string arg, sol::optional<size_t> maxLength) {
				return self.Write<std::string>(arg, maxLength.value_or(64));
			};

			ptrUt["WriteInt8"] = [](Pointer& self, int8_t arg) {
				return self.Write<int8_t>(arg);
			};

			ptrUt["WriteUint8"] = [](Pointer& self, uint8_t arg) {
				return self.Write<uint8_t>(arg);
			};

			ptrUt["WriteInt16"] = [](Pointer& self, int16_t arg) {
				return self.Write<int16_t>(arg);
			};

			ptrUt["WriteUint16"] = [](Pointer& self, uint16_t arg) {
				return self.Write<uint16_t>(arg);
			};

			ptrUt["WriteInt32"] = [](Pointer& self, int32_t arg) {
				return self.Write<int32_t>(arg);
			};

			ptrUt["WriteUint32"] = [](Pointer& self, uint32_t arg) {
				return self.Write<uint32_t>(arg);
			};

			ptrUt["WriteInt64"] = [](Pointer& self, int64_t arg) {
				return self.Write<int64_t>(arg);
			};

			ptrUt["WriteUint64"] = [](Pointer& self, uint64_t arg) {
				return self.Write<uint64_t>(arg);
			};

			ptrUt["WriteFloat"] = [](Pointer& self, float arg) {
				return self.Write<float>(arg);
			};

			ptrUt["WriteDouble"] = [](Pointer& self, double arg) {
				return self.Write<double>(arg);
			};

			ptrUt["AddBytePatch"] = [&](Pointer& self, sol::table values, sol::optional<bool> apply) {
				auto module = GetModuleFromLuaState(L);
				if (!module)
					return sol::make_object(L, sol::nil);

				std::vector<uint8_t> temp;
				for (std::size_t i = 1; i <= values.size(); ++i)
				{
					uint8_t val = values[i].get_or<uint8_t>(0);
					temp.push_back(val);
				}

				auto patch = std::make_shared<BytePatch>(self, std::span<uint8_t>(temp));
				module->AddBytePatch(patch);

				if (apply.value_or(false))
					patch->Apply();

				return sol::make_object(L, patch);
			};
		}
	};

	LuaMemLib _LuaMemLib;
}
