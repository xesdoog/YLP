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
	InjectResult InjectResult::Ok() noexcept
	{
		InjectResult r;
		r.success = true;
		r.message = "Success";
		r.win_error = 0;
		return r;
	}

	InjectResult InjectResult::Err(std::string msg, DWORD err) noexcept
	{
		InjectResult r;
		r.success = false;
		r.message = std::move(msg);
		r.win_error = err;
		return r;
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
		std::vector<ProcessEntry> tempList;

		if (Process32FirstW(snapshot, &entry))
		{
			do
			{
				char name[260];
				WideCharToMultiByte(CP_UTF8, 0, entry.szExeFile, -1, name, sizeof(name), nullptr, nullptr);
				tempList.push_back({.m_Name = name, .m_Pid = entry.th32ProcessID});
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

	DllInfo ValidateDLL(const std::filesystem::path& file)
	{
		if (!IO::Exists(file))
			return {.error = "File not found"};

		HANDLE hFile = CreateFileW(file.wstring().c_str(),
		    GENERIC_READ,
		    FILE_SHARE_READ,
		    nullptr,
		    OPEN_EXISTING,
		    FILE_ATTRIBUTE_NORMAL,
		    nullptr);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			CloseHandle(hFile);
			return {.error = "CreateFile failed"};
		}

		HANDLE hMap = CreateFileMappingW(hFile, nullptr, PAGE_READONLY, 0, 0, nullptr);
		if (!hMap)
		{
			CloseHandle(hFile);
			return {.error = "CreateFileMapping failed"};
		}

		LPVOID base = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
		if (!base)
		{
			CloseHandle(hMap);
			CloseHandle(hFile);
			return {.error = "MapViewOfFile failed"};
		}

		auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
		if (dos->e_magic != IMAGE_DOS_SIGNATURE)
		{
			UnmapViewOfFile(base);
			CloseHandle(hMap);
			CloseHandle(hFile);
			return {.error = "Invalid DOS signature"};
		}

		auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>((BYTE*)base + dos->e_lfanew);
		if (nt->Signature != IMAGE_NT_SIGNATURE)
		{
			UnmapViewOfFile(base);
			CloseHandle(hMap);
			CloseHandle(hFile);
			return {.error = "Invalid NT signature"};
		}

		DllInfo info{};
		info.is64bit	= (nt->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64);
		auto entryExport = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		info.hasExports  = entryExport.Size > 0 && entryExport.VirtualAddress != 0;
		info.ok			= true;

		UnmapViewOfFile(base);
		CloseHandle(hMap);
		CloseHandle(hFile);
		return info;
	}

	DllInfo AddDLL()
	{
		auto dllPath = IO::OpenFileDialog({{L"Dynamic Link Library", L"*.dll"}}, L"Select a DLL");
		if (dllPath.empty())
			return {.error = "Canceled by user"};

		DllInfo info = ValidateDLL(dllPath);
		info.checksum = Utils::CalcSha256(dllPath);
		info.filepath = dllPath;
		info.name = dllPath.filename().string();
		return info;
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

	HANDLE RemoteLoadLibraryW(HANDLE hProcess, LPVOID lpRemoteWstr)
	{
		HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");
		if (!hKernel32)
			return nullptr;

		FARPROC proc = GetProcAddress(hKernel32, "LoadLibraryW");
		if (!proc)
			return nullptr;

		return CreateRemoteThread(hProcess, nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(proc), lpRemoteWstr, 0, nullptr);
	}

	InjectResult Inject(std::string_view processName, std::filesystem::path dllPath, bool manualMap)
	{
		auto dllInfo = ValidateDLL(dllPath);
		if (!dllInfo.ok)
			return InjectResult::Err(std::string("PE validation failed: ") + dllInfo.error);

		if (!dllPath.is_absolute())
			dllPath = std::filesystem::absolute(dllPath);

		auto maybepid = GetProcessId(processName);
		if (!maybepid.has_value())
			return InjectResult::Err(std::string("Process not found: ") + std::string(processName));

		DWORD pid = maybepid.value();
		const DWORD acc = PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE;
		ScopedHandle hProcess(OpenProcess(acc, FALSE, pid));
		if (!hProcess)
			return InjectResult::Err("OpenProcess failed", GetLastError());

		if (!IsSameArch(hProcess.Get()))
			return InjectResult::Err("Process mismatch (YLP and target process must be the same architecture).");

		//return manualMap ? ManualMapInject(hProcess, dllPath) : NativeInject(hProcess, dllPath);

		std::wstring dllW  = dllPath.wstring();
		const SIZE_T bytes = (dllW.size() + 1) * sizeof(wchar_t);

		LPVOID remoteMem = VirtualAllocEx(hProcess.Get(), nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!remoteMem)
			return InjectResult::Err("VirtualAllocEx failed", GetLastError());

		SIZE_T written = 0;
		if (!WriteProcessMemory(hProcess.Get(), remoteMem, dllW.c_str(), bytes, &written) || written != bytes)
		{
			const DWORD err = GetLastError();
			VirtualFreeEx(hProcess.Get(), remoteMem, 0, MEM_RELEASE);
			return InjectResult::Err("WriteProcessMemory failed", err);
		}

		ScopedHandle hThread(RemoteLoadLibraryW(hProcess.Get(), remoteMem));
		if (!hThread)
		{
			const DWORD err = GetLastError();
			VirtualFreeEx(hProcess.Get(), remoteMem, 0, MEM_RELEASE);
			return InjectResult::Err("CreateRemoteThread (LoadLibraryW) failed", err);
		}

		const DWORD wait = WaitForSingleObject(hThread.Get(), 10'000);
		if (wait == WAIT_FAILED)
		{
			const DWORD err = GetLastError();
			LOG_WARN("[PsUtils]: WaitForSingleObject failed with error {}", std::system_category().message(err));
		}
		else if (wait == WAIT_TIMEOUT)
		{
			LOG_WARN("[PsUtils]: Remote thread timed out after 10 seconds.");
		}

		DWORD exitCode = 0;
		if (!GetExitCodeThread(hThread.Get(), &exitCode))
		{
			const DWORD err = GetLastError();
			VirtualFreeEx(hProcess.Get(), remoteMem, 0, MEM_RELEASE);
			return InjectResult::Err("GetExitCodeThread failed", err);
		}

		if (exitCode == 0)
		{
			VirtualFreeEx(hProcess.Get(), remoteMem, 0, MEM_RELEASE);
			return InjectResult::Err("Remote LoadLibraryW returned NULL (load failed inside target).");
		}

		if (!VirtualFreeEx(hProcess.Get(), remoteMem, 0, MEM_RELEASE))
		{
			LOG_WARN("[PsUtils]: VirtualFreeEx failed during cleanup with error {}", std::system_category().message(GetLastError()));
		}

		char buf[265];
		sprintf_s(buf, "Successfully injected %s into %s. Remote module handle: 0x%08X", dllPath.filename().string().c_str(), processName.data(), static_cast<unsigned int>(exitCode));
		LOG_DEBUG("[PsUtils]: {}", buf);
		return InjectResult::Ok();
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
}
