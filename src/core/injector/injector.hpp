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

#include "thirdparty/Syscall/syscall.h"


using pLoadLibraryA   = HINSTANCE(WINAPI*)(const char*);
using pGetProcAddress = FARPROC(WINAPI*)(HMODULE, LPCSTR);
using pDllMain        = BOOL(WINAPI*)(void*, DWORD, void*);
using pAddFuncTable   = BOOL(WINAPIV*)(PRUNTIME_FUNCTION, DWORD, DWORD64);


namespace YLP::Injector
{
	struct InjectResult
	{
		std::string m_Message{};

		bool m_Success   = false;
		DWORD m_WinError = 0;

		static InjectResult Ok() noexcept;
		static InjectResult Err(std::string msg, DWORD err = 0) noexcept;
	};

	struct InjectorConfig
	{
		int m_Mode                     = 0;
		int m_MaxRandomizationAttempts = 3;
		bool m_WipePE                  = false;
		bool m_RandomizeAddress        = false;
		bool m_EnableSEH               = true;
	};

	HANDLE RemoteLoadLibraryW(HANDLE hProcess, LPVOID lpRemoteWstr);

	InjectResult NativeInject(HANDLE hProcess, const fs::path& dllPath);
	InjectResult ManualMap(HANDLE hProcess, const BYTE* dllData, size_t dllSize, InjectorConfig config);
	InjectResult Inject(const std::string_view& processName, fs::path dllPath, const InjectorConfig& cfg = {});

	inline void to_json(nlohmann::json& j, const InjectorConfig& cfg)
	{
		j["mode"]                       = cfg.m_Mode;
		j["max_randomization_attempts"] = cfg.m_MaxRandomizationAttempts;
		j["wipe_headers"]               = cfg.m_WipePE;
		j["randomize_address"]          = cfg.m_RandomizeAddress;
		j["enable_seh"]                 = cfg.m_EnableSEH;
	}

	inline void from_json(const nlohmann::json& j, InjectorConfig& cfg)
	{
		cfg.m_Mode                     = j.value("mode", 0);
		cfg.m_MaxRandomizationAttempts = j.value("max_randomization_attempts", 3);
		cfg.m_WipePE                   = j.value("wipe_headers", false);
		cfg.m_RandomizeAddress         = j.value("randomize_address", false);
		cfg.m_EnableSEH                = j.value("enable_seh", true);
	}
}
