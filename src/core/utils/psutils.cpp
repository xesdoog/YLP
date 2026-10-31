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


#include "psutils.hpp"


namespace YLP::PsUtils
{
	ProcessList::ProcessList()
	{
		if (auto maybeData = GetIconFromProcessPath(""); maybeData.has_value())
		{
			auto& data = maybeData.value();
			Renderer::RequestTextureRaw(data.m_Data,
			    data.m_Width,
			    data.m_Height,
			    "Default Process Icon",
			    &m_DefaultProcessIcon,
			    false);
		}
	}

	const ImTextureID ProcessList::GetProcessIconImpl(const fs::path& path)
	{
		if (auto it = m_ProcessIcons.find(path); it != m_ProcessIcons.end() && it->second.m_ImTex)
			return it->second.m_ImTex;

		return m_DefaultProcessIcon;
	}

	void ProcessList::StartUpdatingImpl()
	{
		if (m_Running)
			return;

		m_Running = true;
		ThreadManager::RunDetached([this]() {
			while (m_Running)
			{
				UpdateProcesses();
				std::unique_lock lock(m_CVMutex);
				m_ConVar.wait_for(lock, 2s, [this] {
					return !m_Running;
				});
			}
		});
	}

	void ProcessList::StopUpdatingImpl()
	{
		if (!m_Running)
			return;

		m_Running = false;
		m_ConVar.notify_all();
	}

	const std::vector<ProcessEntry> ProcessList::GetSnapshotImpl()
	{
		std::scoped_lock lock(m_Mutex);
		return m_Processes;
	}

	void ProcessList::UpdateProcessesImpl()
	{
		if (std::chrono::steady_clock::now() - m_LastUpdated < 1s)
			return;

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE)
			return;

		PROCESSENTRY32W entry{.dwSize = sizeof(entry)};
		std::vector<ProcessEntry> tempList{};

		if (Process32FirstW(snapshot, &entry))
		{
			do
			{
				char name[260]{};
				WideCharToMultiByte(CP_UTF8, 0, entry.szExeFile, -1, name, sizeof(name), nullptr, nullptr);

				fs::path filePath{};
				DWORD pid = entry.th32ProcessID;

				ScopedHandle hProcess(OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid));
				if (hProcess)
				{
					wchar_t buffer[MAX_PATH]{};
					DWORD size = MAX_PATH;

					if (QueryFullProcessImageNameW(hProcess.Get(), 0, buffer, &size))
						filePath = fs::path(buffer);
				}

				tempList.push_back({.m_Name = name, .m_Pid = pid, .m_ExePath = filePath});

				if (auto it = m_ProcessIcons.find(filePath); it == m_ProcessIcons.end())
				{
					if (auto maybeData = GetIconFromProcessPath(filePath); maybeData.has_value())
					{
						auto& data               = maybeData.value();
						m_ProcessIcons[filePath] = std::move(data);
						auto& cached             = m_ProcessIcons.at(filePath);

						Renderer::RequestTextureRaw(
						    cached.m_Data,
						    cached.m_Width,
						    cached.m_Height,
						    std::format("{} Icon", name),
						    &cached.m_ImTex,
						    false);
					}
					else
						m_ProcessIcons[filePath] = ProcessIconData{.m_LoadRequested = true};
				}
			} while (Process32NextW(snapshot, &entry));
		}

		CloseHandle(snapshot);
		std::scoped_lock lock(m_Mutex);
		m_Processes.swap(tempList);
		m_LastUpdated = std::chrono::steady_clock::now();
	}

	std::optional<DWORD> WaitForProcessExit(HANDLE hProc, DWORD timeoutMs)
	{
		if (!hProc || hProc == INVALID_HANDLE_VALUE)
		{
			LOG_ERROR("[PsUtils]: WaitForProcessExit failed with invalid handle: 0x{:X}", (uintptr_t)hProc);
			return std::nullopt;
		}

		DWORD wait = WaitForSingleObject(hProc, timeoutMs);
		if (wait == WAIT_OBJECT_0)
		{
			DWORD exitCode = STILL_ACTIVE;
			if (!GetExitCodeProcess(hProc, &exitCode))
			{
				DWORD last = GetLastError();
				LOG_ERROR("[PsUtils]: GetExitCodeProcess failed with error 0x{:X} ({})", last, TranslateError(last));
				return std::nullopt;
			}
			return exitCode;
		}
		else if (wait == WAIT_TIMEOUT)
		{
			return std::nullopt;
		}
		else
		{
			DWORD last = GetLastError();
			LOG_ERROR("[PsUtils]: WaitForSingleObject failed with error 0x{:X} ({})", last, TranslateError(last));
			return std::nullopt;
		}
	}
	
	std::optional<DWORD> GetProcessId(std::string_view name)
	{
		ProcessList::UpdateProcesses();
		for (auto& entry : ProcessList::GetSnapshot())
		{
			if (entry.m_Name == name)
				return entry.m_Pid;
		}

		return std::nullopt;
	}

	const bool IsSameArch(HANDLE hTargetProcess)
	{
		BOOL targetIsWow = FALSE;
		if (!IsWow64Process(hTargetProcess, &targetIsWow))
			return false;

		BOOL selfIsWow = FALSE;
		if (!IsWow64Process(GetCurrentProcess(), &selfIsWow))
			return false;

		return (targetIsWow == selfIsWow);
	}

	std::string TranslateError(DWORD exitCode)
	{
		LPWSTR msgBuffer = nullptr;
		DWORD result = FormatMessageW(
		    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		    nullptr,
		    exitCode,
		    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		    reinterpret_cast<LPWSTR>(&msgBuffer),
		    0,
		    nullptr);

		std::string errorMsg;
		if (result != 0 && msgBuffer != nullptr)
		{
			errorMsg = Utils::WideToUTF8(msgBuffer);
			LocalFree(msgBuffer);

			if (auto lastPos = errorMsg.find_last_not_of(" \r\n"); lastPos != std::string::npos)
				errorMsg.erase(lastPos + 1);
		}
		else
		{
			errorMsg = "UNKNOWN_ERROR";
		}

		return errorMsg;
	}

	const bool IsServiceRunning(const std::wstring& serviceName)
	{
		SC_HANDLE scm = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
		if (!scm)
		{
			DWORD err = GetLastError();
			LOG_DEBUG("[PsUtils]: OpenSCManager failed with error {}: {}", err, TranslateError(err));
			return false;
		}

		SC_HANDLE svc = OpenServiceW(scm, serviceName.c_str(), SERVICE_QUERY_STATUS);
		if (!svc)
		{
			CloseServiceHandle(scm);
			return false;
		}

		SERVICE_STATUS_PROCESS ssp{};
		DWORD bytesNeeded = 0;
		BOOL ok = QueryServiceStatusEx(svc, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE>(&ssp), sizeof(ssp), &bytesNeeded);
		if (!ok)
		{
			CloseServiceHandle(svc);
			CloseServiceHandle(scm);
			return false;
		}

		bool running = (ssp.dwCurrentState == SERVICE_RUNNING);
		CloseServiceHandle(svc);
		CloseServiceHandle(scm);
		return running;
	}

	BOOL CALLBACK EnumProcessWindows(HWND hwnd, LPARAM lParam)
	{
		EnumWindowData& ewd = *reinterpret_cast<EnumWindowData*>(lParam);
		DWORD dwPid = 0;

		GetWindowThreadProcessId(hwnd, &dwPid);

		if (dwPid == ewd.m_Pid)
		{
			ewd.m_HWND = hwnd;
			return FALSE;
		}
		return TRUE;
	}

	HWND GetHwndFromPid(DWORD pid)
	{
		EnumWindowData ewd{0};
		ewd.m_Pid = pid;
		EnumWindows(EnumProcessWindows, reinterpret_cast<LPARAM>(&ewd));
		return ewd.m_HWND;
	}

	std::optional<ProcessIconData> GetIconFromProcessPath(const fs::path& exePath)
	{
		HICON hIcon      = nullptr;
		UINT extracted   = 0;
		LPCWSTR lpszFile = exePath.empty() ? L"shell32.dll" : exePath.c_str(); // if we pass an empty path we get the default windows 'missing' icon (idk what it's called)
		int nIconIndex   = exePath.empty() ? 2 : 0;

		if (ExtractIconExW(lpszFile, nIconIndex, &hIcon, nullptr, 1) == 0 || !hIcon)
			return std::nullopt;

		ScopedIconHandle iconGuard(hIcon);
		constexpr int size = 32;

		BITMAPINFO bmi{};
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       = size;
		bmi.bmiHeader.biHeight      = -size;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biBitCount    = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		void* dibPixels = nullptr;

		HDC hdc = CreateCompatibleDC(nullptr);
		if (!hdc)
			return std::nullopt;

		ScopedDC dcGuard(hdc);
		HBITMAP bitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &dibPixels, nullptr, 0);
		if (!bitmap || !dibPixels)
		{
			if (bitmap)
				DeleteObject(bitmap);
			return std::nullopt;
		}

		ScopedBitmap bitmapGuard(bitmap);
		HGDIOBJ oldBitmap = SelectObject(hdc, bitmap);
		const bool drawn  = DrawIconEx(hdc, 0, 0, hIcon, size, size, 0, nullptr, DI_NORMAL);

		SelectObject(hdc, oldBitmap);

		if (!drawn)
			return std::nullopt;

		ProcessIconData data{};
		data.m_Width  = size;
		data.m_Height = size;
		data.m_Data.resize(static_cast<size_t>(size * size * 4));

		const auto* src = static_cast<const uint8_t*>(dibPixels);
		for (size_t i = 0; i < size * size; ++i)
		{
			const size_t offset = i * 4;
			data.m_Data[offset + 0] = src[offset + 2];
			data.m_Data[offset + 1] = src[offset + 1];
			data.m_Data[offset + 2] = src[offset + 0];
			data.m_Data[offset + 3] = src[offset + 3];
		}

		return data;
	}
}
