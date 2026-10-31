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

#include <combaseapi.h>
#include <shellapi.h>
#include <shobjidl.h>


namespace YLP::IO
{
	struct DllInfo
	{
		bool m_Ok         = false;
		bool m_Is64bit    = false;
		bool m_HasExports = false;

		fs::path m_Path{};

		std::string m_Error{};
		std::string m_Checksum{};
		std::string m_Name{};
		std::string m_LastKnownProcess{};

		bool operator==(const DllInfo& other) const noexcept
		{
			return m_Checksum == other.m_Checksum && m_Name == other.m_Name;
		}
	};

	void HandleFsErr(const std::error_code& ec);

	bool Exists(const fs::path& _path);
	bool IsDir(const fs::path& _path);
	bool IsFile(const fs::path& _path);
	bool IsEmpty(const fs::path& _path);
	bool CreateFolder(const fs::path& _path);
	bool CreateFolders(const fs::path& _path);
	bool Remove(const fs::path& _path);
	bool RemoveAll(const fs::path& target);
	bool HasLuaFiles(const fs::path& root);
	bool FilterLuaFiles(const fs::path& root);

	void Open(const std::string& path);
	void Open(const std::wstring& path);
	void HighlightFile(const std::string& filePath);
	void HighlightFile(const std::wstring& filePath);
	bool Rename(const fs::path& src, const fs::path& dest);

	DllInfo AddDLL();
	void ValidateDLL(DllInfo& inf);

	std::wstring ReadRegistryKey(HKEY rootPath, const wchar_t* subkeyPath, const wchar_t* subkeyValue);

	fs::path _OpenFileDialog(
	    const std::vector<COMDLG_FILTERSPEC>& filters,
	    const wchar_t* title,
	    FILEOPENDIALOGOPTIONS flags,
	    const std::filesystem::path& defaultFolder = {},
	    const wchar_t* defaultName = nullptr,
	    const wchar_t* defaultExtension = nullptr);

	fs::path OpenFileDialog(
	    const std::vector<COMDLG_FILTERSPEC>& filters = {{L"All Files", L"*.*"}},
	    const wchar_t* title = L"Open File",
	    bool multiselect = false,
	    const std::filesystem::path& defaultFolder = {});

	fs::path OpenFolderDialog(
	    const wchar_t* title = L"Select Folder",
		bool multiselect = false,
	    const fs::path& defaultFolder = {});

	fs::path SaveFileDialog(
	    const std::vector<COMDLG_FILTERSPEC>& filters = {{L"All Files", L"*.*"}},
		const wchar_t* title = L"Save As",
		const wchar_t* defaultName = L"",
		const wchar_t* defaultExtension = L"",
	    const fs::path& defaultFolder = {});

	std::optional<std::filesystem::path> MakeAbsPath(const fs::path& root, const fs::path& other);
}
