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

#include "core/scoped_resource.hpp"

#include <condition_variable>
#include <system_error>
#include <thread>
#include <shellapi.h>
#include <TlHelp32.h>


namespace YLP::PsUtils
{
	struct ProcessEntry
	{
		std::string m_Name{};
		DWORD m_Pid{};
		fs::path m_ExePath{};
	};

	struct ProcessIconData
	{
		std::vector<uint8_t> m_Data;

		int m_Width  = 0;
		int m_Height = 0;

		ImTextureID m_ImTex{};

		bool m_LoadRequested = false;
	};

	struct EnumWindowData
	{
		DWORD m_Pid{};
		HWND m_HWND{};
	};

	class ProcessList : public Singleton<ProcessList>
	{
		friend class Singleton<ProcessList>;

	public:
		ProcessList();

		~ProcessList()
		{
			StopUpdating();
			m_ProcessIcons.clear();
		}

		static void StartUpdating()
		{
			GetInstance().StartUpdatingImpl();
		}

		static void StopUpdating()
		{
			GetInstance().StopUpdatingImpl();
		}

		static void UpdateProcesses()
		{
			GetInstance().UpdateProcessesImpl();
		}

		static const std::vector<ProcessEntry> GetSnapshot()
		{
			return GetInstance().GetSnapshotImpl();
		}

		static const ImTextureID GetProcessIcon(const fs::path& path)
		{
			return GetInstance().GetProcessIconImpl(path);
		}

	private:
		void StartUpdatingImpl();
		void StopUpdatingImpl();
		void UpdateProcessesImpl();

		const ImTextureID GetProcessIconImpl(const fs::path& path);

		const std::vector<ProcessEntry> GetSnapshotImpl();

		std::vector<ProcessEntry> m_Processes{};
		std::unordered_map<fs::path, ProcessIconData> m_ProcessIcons{};

		std::mutex m_Mutex{};
		std::mutex m_CVMutex{};

		std::thread m_Worker{};

		std::atomic<bool> m_Running{false};

		std::condition_variable m_ConVar{};

		std::chrono::time_point<std::chrono::steady_clock> m_LastUpdated{};

		ImTextureID m_DefaultProcessIcon{};
	};

	std::optional<DWORD> GetProcessId(std::string_view name);

	const bool IsSameArch(HANDLE hTargetProcess);
	const bool IsServiceRunning(const std::wstring& serviceName);

	std::string TranslateError(DWORD exitCode);

	std::optional<DWORD> WaitForProcessExit(HANDLE hProc, DWORD timeoutMs);

	BOOL CALLBACK EnumProcessWindows(HWND hWnd, LPARAM lParam);
	HWND GetHwndFromPid(DWORD pid);
	std::optional<ProcessIconData> GetIconFromProcessPath(const fs::path& exePath);
}
