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

#include <windows.h>

#include <atomic>
#include <chrono>
#include <deque>
#include <filesystem>
#include <format>
#include <functional>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <mutex>
#include <shared_mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>


#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>


namespace YLP
{
	using namespace std::chrono_literals;
	namespace fs = std::filesystem;

	extern HINSTANCE g_Instance;
	extern HWND g_Hwnd;
	extern std::filesystem::path g_ProjectPath;
	extern std::filesystem::path g_YimPath;
	extern std::filesystem::path g_YimV2Path;
	extern std::atomic<bool> g_Running;

	class GlobalMutex
	{
		HANDLE hMutex = nullptr;

	public:
		GlobalMutex(const char* name)
		{
			hMutex = CreateMutexA(nullptr, TRUE, name);
			if (GetLastError() == ERROR_ALREADY_EXISTS)
				hMutex = nullptr;
		}

		bool IsOwned() const
		{
			return hMutex != nullptr;
		}

		~GlobalMutex()
		{
			if (hMutex)
				CloseHandle(hMutex);
		}
	};
}


#include "core/singleton.hpp"
#include "core/logging/logger.hpp"

#define LOG(level, ...) Logger::Log(level, std::source_location::current(), ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) LOG(eLogLevel::Info, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) LOG(eLogLevel::Warn, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG(eLogLevel::Error, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG(eLogLevel::Debug, fmt, ##__VA_ARGS__)                                                                  \


#include "core/utils/utils.hpp"
#include "core/utils/io.hpp"
#include "core/utils/psutils.hpp"
#include "core/settings.hpp"
#include "core/threadmgr.hpp"
#include "core/updater.hpp"
#include "core/gui/imgui_helpers.hpp"


inline decltype(auto) Config()
{
	return YLP::Settings::Get();
}
