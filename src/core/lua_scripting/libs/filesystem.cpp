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
#include "../lua_path.hpp"


namespace YLP::LuaJIT
{
	class LuaFsLib : public LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

	private:
		fs::path GetModuleRoot(sol::state& L)
		{
			if (auto mod = GetModuleFromLuaState(L))
				return mod->GetRoot();

			return {};
		}

		inline auto Iterator(sol::state& L, const LuaPath& path)
		{
			auto iterator = std::make_shared<std::filesystem::recursive_directory_iterator>(path.Get(), fs::directory_options::skip_permission_denied);
			auto end      = std::filesystem::recursive_directory_iterator{};
			auto modRoot  = GetModuleRoot(L);

			sol::state_view lua(L);
			return [iterator, end, modRoot, lua]() mutable -> sol::object {
				if (*iterator == end)
					return sol::lua_nil;

				try
				{
					LuaPath result(modRoot, (*iterator)->path());
					++(*iterator);
					return sol::make_object(lua, result);
				}
				catch (const LuaPathError& e)
				{
					luaL_error(lua.lua_state(), e.what());
					return sol::make_object(lua, sol::nil);
				}
			};
		};

	public:
		void Register(sol::state& L) override
		{
			/*@ylp.table Filesystem
			* description
				Provides file system functions. Paths are limited to the `/Plugins` folder only.
			
			* function MyRoot Returns the module's root path.
			* return Path

			* function IterDir Recursive directory iterator.
			* param path<Path>
			* return fun(): Path

			* function Rename
			* param oldPath<Path>
			* param newPath<Path>
			* return boolean successOrFailure
			* return string failReason

			* function Remove
			* param path<Path>
			* param emptyOnly<boolean?> If true, non-empty folders will not be removed. Defaults to `true`.
			* return boolean successOrFailure
			* return string failReason
			@*/
			auto luaFs = L["Filesystem"].get_or_create<sol::table>();

			luaFs["MyRoot"] = [&]()
			{
				auto modRoot = GetModuleRoot(L);
				return LuaPath(modRoot, modRoot);
			};

			luaFs["IterDir"] = [&](const LuaPath& path)
			{
				return Iterator(L, path);
			};

			luaFs["Rename"] = [&](const LuaPath& oldPath, const LuaPath& newPath)
			{
				if (!oldPath.Exists())
					return std::make_tuple(false, "File not found");

				auto modRoot  = GetModuleRoot(L);
				auto modEntry = modRoot / "main.lua";
				auto& oldFs   = oldPath.Get();
				auto& newFs   = newPath.Get();

				if (oldFs == modRoot || oldFs == modEntry || newFs == modRoot || newFs == modEntry)
					return std::make_tuple(false, "Can not rename module's root path or entry point!");

				bool success = IO::Rename(oldPath.Get(), newPath.Get());
				return std::make_tuple(success, success ? "" : "An error has occured.");
			};

			luaFs["Remove"] = [&](const LuaPath& path, sol::optional<bool> emptyOnly)
			{
				if (!path.Exists())
					return std::make_tuple(false, "File not found");

				auto modRoot  = GetModuleRoot(L);
				auto modEntry = modRoot / "main.lua";
				auto& pathFs  = path.Get();

				if (pathFs == modRoot || pathFs == modEntry)
					return std::make_tuple(false, "Can not remove module's root path or entry point!");

				auto func    = emptyOnly.value_or(true) ? IO::Remove : IO::RemoveAll;
				bool success = func(path.Get());
				return std::make_tuple(success, success ? "" : "An error has occured.");
			};

			/*@ylp.class Path
			* description
				A path object limited to the `/Plugins` folder

			* constructor __call
			* param path<string>
			
			* operator div
			* param other<string>
			* return Path

			* method Exists
			* return boolean

			* method IsFile
			* return boolean

			* method IsDir
			* return boolean

			* method MakeDir Creates a directory if it doesn't exist
			*

			* method MakeDirs Creates a directory and any missing parent directories
			*

			* method GetFilename
			* return string

			* method GetStem
			* return string stem The file name without the extension.

			* method GetExtension
			* return string ext The file extension including the leading dot. Ex: `.json`. Returns empty string for folders.

			* method GetFileSize
			* return integer filesz The file size in bytes.

			* method GetParent
			* return Path parentDir The parent directory. Note: This will throw if the path is outside the module's root directory.

			* method Join Joins the path with a sub-path. Does the same thing as the division operator *(__div)*: `myPath / "somefile.txt"`
			* param subPath<string>
			* return Path

			* method Open
			* param mode<openmode?>
			* return file*? File handle or nil
			* return string failReason An error message if the operation fails.

			* method ToString Returns the string representation of the path. `myPath:ToString()`, `myPath:__tostring()`, and `tostring(myPath)` all achieve the same thing.
			* return string strPath

			* method IterDir Recursive directory iterator.
			* return fun(): Path

			* method CalcSha256 Calculates the file's SHA256 hash. Throws if the path is not a file.
			* return string SHA256
			@*/
			auto pathCls = L.new_usertype<LuaPath>("Path",
			    sol::call_constructor, sol::factories([&](const std::string& path)
				{
				    try
				    {
					    return LuaPath(GetModuleRoot(L), path);
				    }
				    catch (const LuaPathError& e)
				    {
					    throw sol::error(e.what());
				    }
				})
			);

			pathCls["GetExtension"] = &LuaPath::Extension;
			pathCls["GetFilename"]  = &LuaPath::Filename;
			pathCls["GetFileSize"]  = &LuaPath::Size;
			pathCls["GetParent"]    = &LuaPath::Parent;
			pathCls["GetStem"]      = &LuaPath::Stem;
			pathCls["Exists"]       = &LuaPath::Exists;
			pathCls["IsFile"]       = &LuaPath::IsFile;
			pathCls["IsDir"]        = &LuaPath::IsDir;
			pathCls["MakeDir"]      = &LuaPath::MakeDir;
			pathCls["MakeDirs"]     = &LuaPath::MakeDirs;
			pathCls["Join"]         = &LuaPath::Join;

			pathCls["Open"] = [&](const LuaPath& self, std::string_view mode) {
				sol::state_view Lua(L);
				auto io                        = Lua["io"];
				sol::protected_function ioOpen = io["open"];
				return ioOpen(self.Get().filename().u8string().c_str(), mode);
			};

			pathCls["ToString"] = [](const LuaPath& self) {
				return self.Get().string();
			};

			pathCls["IterDir"] = [&](const LuaPath& self) {
				return Iterator(L, self);
			};

			pathCls["CalcSha256"] = [&](const LuaPath& self) {
				if (!self.IsFile())
				{
					LOG_ERROR("Attempt to calculate sha256 hash of a non-file Path object");
					return std::string();
				}

				return Utils::CalcSha256(self.Get());
			};

			pathCls["__tostring"] = [](const LuaPath& self) {
				return self.Get().string();
			};

			pathCls["__eq"] = [](const LuaPath& self, const LuaPath& other) {
				return self == other;
			};

			pathCls["__div"] = [](const LuaPath& self, const std::string& sub) {
				return self.Join(sub);
			};
		}
	};

	LuaFsLib _LuaFsLib;
}
