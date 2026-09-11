// YLP Project - GPL-3.0
// See LICENSE file or <https://www.gnu.org/licenses/> for details.


#pragma once

#include <TlHelp32.h>
#include <Psapi.h>
#include <stdexcept>
#include <thread>
#include "pointer.hpp"


namespace YLP
{
	// Ported from YLP Python
	class ProcessScanner
	{
	public:
		explicit ProcessScanner(const std::string& processName);
		~ProcessScanner();

		ProcessScanner(const ProcessScanner&)				 = delete;
		ProcessScanner& operator=(const ProcessScanner&)	 = delete;
		ProcessScanner(ProcessScanner&&) noexcept            = default;
		ProcessScanner& operator=(ProcessScanner&&) noexcept = default;

		bool FindProcess();
		bool IsProcessRunning() const;
		bool IsModuleLoaded(const std::string& moduleName);

		std::vector<uint8_t> ReadMemory(uintptr_t address, size_t size) const;
		bool IsMemoryReadable(uintptr_t address) const;
		bool IsAddressValid(uintptr_t address) const;
		void RefreshModules();

		Pointer FindPattern(const std::string& pattern, const std::string& name = "", size_t chunkSize = 4096);

		HANDLE GetProcessHandle() const noexcept
		{
			return m_ProcessHandle;
		}

		DWORD GetProcessID() const noexcept
		{
			return m_Pid;
		}

		const std::string GetProcessName() const noexcept
		{
			return m_ProcessName;
		}

		uintptr_t GetBaseAddress() const;
		size_t GetModuleSize() const;

	private:
		static std::vector<std::optional<uint8_t>> ParsePattern(const std::string& sig);
		uint64_t ScanPattern(std::vector<std::optional<uint8_t>> bytes, std::vector<uint8_t> mem);

		HANDLE m_ProcessHandle;
		DWORD m_Pid;

		mutable uintptr_t m_BaseAddress;

		std::string m_ProcessName;
		std::unordered_map<std::string, std::string> m_Modules;
	};
}
