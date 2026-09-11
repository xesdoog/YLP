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


namespace YLP::LuaJIT
{
	// TODO
	class LuaMemLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	public:
		void Register(sol::state& L) override
		{
			// clang-format off

			/* @ylp.class Pointer
			* description
				Provides basic Pointer manipulation methods.
				It is exclusively returned by [Process](lua://Process), it can not be explicitly constructed.

			* operator add
			* type function
			* param offset<integer>
			* return Pointer

			* operator sub
			* type function
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
			@*/
			auto ptrUsertype = L.new_usertype<Pointer>("Pointer", sol::no_constructor,
				sol::meta_function::addition, &Pointer::Add,
				sol::meta_function::subtraction, &Pointer::Sub,
				"Add", &Pointer::Add,
				"Sub", &Pointer::Sub,
				"Rip", &Pointer::Rip,
				"Dereference", &Pointer::Dereference,
				"GetAddress", &Pointer::GetAddress
			);

			// clang-format on

			/* @ylp.method Pointer.IsNull
			* return boolean
			@*/
			ptrUsertype["IsNull"] = [](Pointer& self) {
				return self.GetAddress() == 0;
			};

			/* @ylp.method Pointer.ReadString
			* return string
			@*/
			ptrUsertype["ReadString"] = [](Pointer& self, sol::optional<size_t> maxLength) {
				return self.Read<std::string>(maxLength.value_or(64));
			};

			/* @ylp.method Pointer.ReadInt8
			* return integer
			@*/
			ptrUsertype["ReadInt8"] = [](Pointer& self) {
				return self.Read<int8_t>();
			};

			/* @ylp.method Pointer.ReadUint8
			* return integer
			@*/
			ptrUsertype["ReadUint8"] = [](Pointer& self) {
				return self.Read<uint8_t>();
			};

			/* @ylp.method Pointer.ReadInt16
			* return integer
			@*/
			ptrUsertype["ReadInt16"] = [](Pointer& self) {
				return self.Read<int16_t>();
			};

			/* @ylp.method Pointer.ReadUint16
			* return integer
			@*/
			ptrUsertype["ReadUint16"] = [](Pointer& self) {
				return self.Read<uint16_t>();
			};

			/* @ylp.method Pointer.ReadInt32
			* return integer
			@*/
			ptrUsertype["ReadInt32"] = [](Pointer& self) {
				return self.Read<int32_t>();
			};

			/* @ylp.method Pointer.ReadUint32
			* return integer
			@*/
			ptrUsertype["ReadUint32"] = [](Pointer& self) {
				return self.Read<uint32_t>();
			};

			/* @ylp.method Pointer.ReadInt64
			* return integer
			@*/
			ptrUsertype["ReadInt64"] = [](Pointer& self) {
				return self.Read<int64_t>();
			};

			/* @ylp.method Pointer.ReadUint64
			* return integer
			@*/
			ptrUsertype["ReadUint64"] = [](Pointer& self) {
				return self.Read<uint64_t>();
			};

			/* @ylp.method Pointer.ReadFloat
			* return number
			@*/
			ptrUsertype["ReadFloat"] = [](Pointer& self) {
				return self.Read<float>();
			};

			/* @ylp.method Pointer.ReadDouble
			* return number
			@*/
			ptrUsertype["ReadDouble"] = [](Pointer& self) {
				return self.Read<double>();
			};

			/* @ylp.method Pointer.WriteString
			* param arg<string>
			@*/
			ptrUsertype["WriteString"] = [](Pointer& self, std::string arg, sol::optional<size_t> maxLength) {
				self.Write<std::string>(arg, maxLength.value_or(64));
			};

			/* @ylp.method Pointer.WriteInt8
			* param arg<integer>
			@*/
			ptrUsertype["WriteInt8"] = [](Pointer& self, int8_t arg) {
				self.Write<int8_t>(arg);
			};

			/* @ylp.method Pointer.WriteUint8
			* param arg<integer>
			@*/
			ptrUsertype["WriteUint8"] = [](Pointer& self, uint8_t arg) {
				self.Write<uint8_t>(arg);
			};

			/* @ylp.method Pointer.WriteInt16
			* param arg<integer>
			@*/
			ptrUsertype["WriteInt16"] = [](Pointer& self, int16_t arg) {
				self.Write<int16_t>(arg);
			};

			/* @ylp.method Pointer.WriteUint16
			* param arg<integer>
			@*/
			ptrUsertype["WriteUint16"] = [](Pointer& self, uint16_t arg) {
				self.Write<uint16_t>(arg);
			};

			/* @ylp.method Pointer.WriteInt32
			* param arg<integer>
			@*/
			ptrUsertype["WriteInt32"] = [](Pointer& self, int32_t arg) {
				self.Write<int32_t>(arg);
			};

			/* @ylp.method Pointer.WriteUint32
			* param arg<integer>
			@*/
			ptrUsertype["WriteUint32"] = [](Pointer& self, uint32_t arg) {
				self.Write<uint32_t>(arg);
			};

			/* @ylp.method Pointer.WriteInt64
			* param arg<integer>
			@*/
			ptrUsertype["WriteInt64"] = [](Pointer& self, int64_t arg) {
				self.Write<int64_t>(arg);
			};

			/* @ylp.method Pointer.WriteUint64
			* param arg<integer>
			@*/
			ptrUsertype["WriteUint64"] = [](Pointer& self, uint64_t arg) {
				self.Write<uint64_t>(arg);
			};

			/* @ylp.method Pointer.WriteFloat
			* param arg<number>
			@*/
			ptrUsertype["WriteFloat"] = [](Pointer& self, float arg) {
				self.Write<float>(arg);
			};

			/* @ylp.method Pointer.WriteDouble
			* param arg<number>
			@*/
			ptrUsertype["WriteDouble"] = [](Pointer& self, double arg) {
				self.Write<double>(arg);
			};
		}
	};

	LuaMemLib _LuaMemLib;
}
