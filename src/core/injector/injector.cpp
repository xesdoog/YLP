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


#include "injector.hpp"
#include "shll.hpp"
#include "../memory/mem_helpers.hpp"


namespace YLP::Injector
{
	using namespace PsUtils;

	InjectResult InjectResult::Ok() noexcept
	{
		return {.m_Success = true};
	}

	InjectResult InjectResult::Err(std::string msg, DWORD err) noexcept
	{
		InjectResult r;
		r.m_Success  = false;
		r.m_Message  = std::move(msg);
		r.m_WinError = err;
		return r;
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

	InjectResult NativeInject(HANDLE hProcess, const fs::path& dllPath)
	{
		const auto& wpath  = dllPath.wstring();
		const SIZE_T bytes = (wpath.size() + 1) * sizeof(wchar_t);
		LPVOID remoteMem   = VirtualAllocEx(hProcess, nullptr, bytes, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!remoteMem)
			return InjectResult::Err("VirtualAllocEx failed", GetLastError());

		SIZE_T written = 0;
		if (!WriteProcessMemory(hProcess, remoteMem, wpath.c_str(), bytes, &written) || written != bytes)
		{
			const DWORD err = GetLastError();
			VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
			return InjectResult::Err("WriteProcessMemory failed", err);
		}

		ScopedHandle hThread(RemoteLoadLibraryW(hProcess, remoteMem));
		if (!hThread)
		{
			const DWORD err = GetLastError();
			VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
			return InjectResult::Err("CreateRemoteThread (LoadLibraryW) failed", err);
		}

		const DWORD wait = WaitForSingleObject(hThread.Get(), 10'000);
		if (wait == WAIT_FAILED)
			LOG_WARN("WaitForSingleObject failed with error {}", PsUtils::TranslateError(GetLastError()));
		else if (wait == WAIT_TIMEOUT)
			LOG_WARN("Remote thread timed out after 10 seconds.");

		DWORD exitCode = 0;
		if (!GetExitCodeThread(hThread.Get(), &exitCode))
		{
			VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE);
			return InjectResult::Err("GetExitCodeThread failed", GetLastError());
		}

		if (!VirtualFreeEx(hProcess, remoteMem, 0, MEM_RELEASE))
			LOG_WARN("VirtualFreeEx failed during cleanup with error {}", PsUtils::TranslateError(GetLastError()));

		if (exitCode == 0)
			return InjectResult::Err("RemoteLoadLibraryW returned NULL (load failed inside target).");

		return InjectResult::Ok();
	}

	InjectResult ManualMap(HANDLE hProcess, const BYTE* dllData, size_t dllSize, InjectorConfig config)
	{
		if (!Syscall::IsInitialized())
		{
			if (!Syscall::Initialize())
				return InjectResult::Err("Failed to initialize syscalls");
		}

		const auto* pDosHeader  = reinterpret_cast<const IMAGE_DOS_HEADER*>(dllData);
		const auto* pNtHeaders  = reinterpret_cast<const IMAGE_NT_HEADERS*>(dllData + pDosHeader->e_lfanew);
		const auto* pOptHeader  = &pNtHeaders->OptionalHeader;
		const auto* pFileHeader = &pNtHeaders->FileHeader;
		const DWORD imageSize   = pOptHeader->SizeOfImage;

		LOG_DEBUG("Image size: {}, Entry RVA: {}", imageSize, pOptHeader->AddressOfEntryPoint);
		LOG_DEBUG("Number of sections: {}", pFileHeader->NumberOfSections);

		BYTE* pTargetBase = nullptr;
		SIZE_T regionSize = imageSize;
		NTSTATUS status   = 0xC0000001;

		if (config.m_RandomizeAddress)
		{
			for (int attempt = 0; attempt < config.m_MaxRandomizationAttempts && !pTargetBase; ++attempt)
			{
				BYTE* randomAddr = Memory::GenerateRandomAddress(imageSize);
				regionSize       = imageSize;
				PVOID baseAddr   = randomAddr;

				status = NtAllocateVirtualMemory(hProcess, &baseAddr, 0, &regionSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
				if (NT_SUCCESS(status))
					pTargetBase = reinterpret_cast<BYTE*>(baseAddr);
			}
		}

		if (!pTargetBase)
		{
			regionSize     = imageSize;
			PVOID baseAddr = nullptr;
			status         = NtAllocateVirtualMemory(hProcess, &baseAddr, 0, &regionSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
			if (NT_SUCCESS(status))
				pTargetBase = reinterpret_cast<BYTE*>(baseAddr);
		}

		if (!pTargetBase)
			return InjectResult::Err("Failed to allocate memory!");

		LOG_DEBUG("Allocated target base at 0x{:X} with PAGE_EXECUTE_READWRITE", reinterpret_cast<uintptr_t>(pTargetBase));
		LOG_DEBUG("Bypassed NtProtectVirtualMemory hook");

		status = NtWriteVirtualMemory(hProcess, pTargetBase, const_cast<BYTE*>(dllData), pOptHeader->SizeOfHeaders, nullptr);
		if (!NT_SUCCESS(status))
		{
			LOG_ERROR("Failed to write PE headers (NTSTATUS: {})", status);
			PVOID base  = pTargetBase;
			SIZE_T size = 0;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			return InjectResult::Err("Failed to write process memory!");
		}

		auto pSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
		for (UINT i = 0; i < pFileHeader->NumberOfSections; ++i, ++pSectionHeader)
		{
			if (pSectionHeader->SizeOfRawData > 0)
			{
				status = NtWriteVirtualMemory(hProcess,
				    pTargetBase + pSectionHeader->VirtualAddress,
				    const_cast<BYTE*>(dllData + pSectionHeader->PointerToRawData),
				    pSectionHeader->SizeOfRawData,
				    nullptr);

				if (!NT_SUCCESS(status))
				{
					LOG_ERROR("Failed to write section {} (NTSTATUS: {})", i, status);
					PVOID base  = pTargetBase;
					SIZE_T size = 0;
					NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
					return InjectResult::Err("Failed to write sections!");
				}
			}
		}

		LOG_DEBUG("Wrote {} sections", pFileHeader->NumberOfSections);

		MMappingData mmapData = {};
		mmapData.fnLoadLib = LoadLibraryA;
		mmapData.fnGetProc = GetProcAddress;

#ifdef _WIN64
		mmapData.fnAddFuncTable = (pAddFuncTable)RtlAddFunctionTable;
#endif

		mmapData.pBase      = pTargetBase;
		mmapData.dwReason   = DLL_PROCESS_ATTACH;
		mmapData.lpReserved = nullptr;
		mmapData.enableSEH  = config.m_EnableSEH;

		BYTE* pMappingCtx = nullptr;
		SIZE_T ctxSize    = sizeof(mmapData);
		PVOID ctxBase     = nullptr;

		status = NtAllocateVirtualMemory(hProcess, &ctxBase, 0, &ctxSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!NT_SUCCESS(status))
		{
			PVOID base  = pTargetBase;
			SIZE_T size = 0;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			return InjectResult::Err("Failed to allocate mapping data!", status);
		}
		pMappingCtx = reinterpret_cast<BYTE*>(ctxBase);

		NtWriteVirtualMemory(hProcess, pMappingCtx, &mmapData, sizeof(mmapData), nullptr);

		void* pShellcode = nullptr;
		SIZE_T codeSize  = 0x1000;

		status = NtAllocateVirtualMemory(hProcess, &pShellcode, 0, &codeSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		if (!NT_SUCCESS(status))
		{
			PVOID base  = pTargetBase;
			SIZE_T size = 0;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			base = pMappingCtx;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			return InjectResult::Err("Failed to allocate mapping data!", status);
		}

		NtWriteVirtualMemory(hProcess, pShellcode, reinterpret_cast<void*>(_shell), codeSize, nullptr);
		LOG_DEBUG("Shellcode written to 0x{:X}", reinterpret_cast<uintptr_t>(pShellcode));

		HANDLE hThread               = nullptr;
		OBJECT_ATTRIBUTES_SYSCALL oa = {sizeof(OBJECT_ATTRIBUTES_SYSCALL), 0};

		status = NtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, &oa, hProcess, pShellcode, pMappingCtx, 0, 0, 0, 0, nullptr);
		if (!NT_SUCCESS(status) || !hThread)
		{
			PVOID base  = pTargetBase;
			SIZE_T size = 0;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			base = pMappingCtx;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			base = pShellcode;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			return InjectResult::Err("Failed to create thread!", status);
		}
		LOG_INFO("Remote thread created. Waiting for completion...");

		NtWaitForSingleObject(hThread, FALSE, nullptr);
		NtClose(hThread);

		MMappingData result{};
		NtReadVirtualMemory(hProcess, pMappingCtx, &result, sizeof(result), nullptr);

		if (result.hModule == INVALID_DATA_POINTER)
		{
			PVOID base  = pTargetBase;
			SIZE_T size = 0;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			base = pMappingCtx;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			base = pShellcode;
			NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
			return InjectResult::Err("Failed to execute shell code!");
		}

		LOG_DEBUG("DLL loaded at {:p}", result.hModule);

		BYTE zeros[0x1000] = {};
		NtWriteVirtualMemory(hProcess, pShellcode, zeros, sizeof(zeros), nullptr);

		SIZE_T size = 0;
		PVOID base  = pShellcode;
		NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
		base = pMappingCtx;
		NtFreeVirtualMemory(hProcess, &base, &size, MEM_RELEASE);
		LOG_DEBUG("Shellcode and context cleaned up");

		if (config.m_WipePE)
		{
			SIZE_T headerSize        = static_cast<SIZE_T>(std::min(DWORD(0x1000), pOptHeader->SizeOfHeaders));
			BYTE headerZeros[0x1000] = {};
			NtWriteVirtualMemory(hProcess, pTargetBase, headerZeros, headerSize, nullptr);
			LOG_DEBUG("PE headers erased from process memory");
		}

		return InjectResult::Ok();
	}

	InjectResult Inject(const std::string_view& processName, fs::path dllPath, const InjectorConfig& cfg)
	{
		IO::DllInfo dllInfo{};
		IO::ValidateDLL(dllInfo);

		if (!dllInfo.m_Ok)
			return InjectResult::Err("PE validation failed with error: " + dllInfo.m_Error);

		if (!dllPath.is_absolute())
			dllPath = fs::absolute(dllPath);

		auto maybepid = PsUtils::GetProcessId(processName);
		if (!maybepid.has_value())
			return InjectResult::Err("Process not found: " + std::string(processName));

		DWORD pid = maybepid.value();
		ScopedHandle hProcess(OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid));
		if (!hProcess)
			return InjectResult::Err("OpenProcess failed!", GetLastError());

		if (!IsSameArch(hProcess.Get()))
			return InjectResult::Err("Process mismatch (YLP and target process must be the same architecture).");

		InjectResult result;

		if (cfg.m_Mode == 0)
			result = NativeInject(hProcess.Get(), dllPath);
		else
		{
			std::ifstream file(dllPath, std::ios::binary | std::ios::ate);
			std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);
			std::vector<BYTE> buffer(size);
			if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
				return InjectResult::Err("Failed to read DLL file!");

			result = ManualMap(hProcess.Get(), buffer.data(), size, cfg);
		}

		if (result.m_Success)
		{
			char buf[256];
			sprintf_s(buf, "Successfully injected %s into %s", dllPath.filename().string().c_str(), processName.data());
			LOG_INFO(buf);
		}

		return result;
	}
}
