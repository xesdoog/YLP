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
//
// Credit goes to https://github.com/YimMenu/YimMenuV2 for most of this code.


#pragma once

#include <sol/sol.hpp>

#include "core/directory_watcher.hpp"
#include "../memory/scanner.hpp"


namespace YLP::LuaJIT
{
	class LuaModule
	{
	public:
		LuaModule(fs::path root);
		~LuaModule();

		enum eLuaLoadState : uint8_t
		{
			NONE,
			RUNNING,
			BROKEN,
			WANTS_RELOAD,
			WANTS_UNLOAD
		};

		bool Load();
		void Reload();
		void Unload();
		void Execute(const std::string_view& code);
		void RegisterProcessWatcher(const std::string& processName, sol::protected_function callback, std::chrono::milliseconds delayMs);
		void DispatchProcessWatchers(const std::chrono::steady_clock::time_point& tickStart);
		void RegisterShutdownCallback(sol::protected_function callback);
		void Tick();

		void RegisterTask(sol::protected_function func, std::chrono::milliseconds delayMs = 0ms, std::vector<sol::object> args = {});

		const bool IsSafeToUnload() const noexcept;
		const bool IsRunningTasks() const noexcept;

		const eLuaLoadState GetLoadState() const noexcept;

		lua_State* GetLuaState();

		std::string_view GetName() const noexcept;

		fs::path GetRoot() const noexcept;

	private:
		struct LuaTask
		{
			sol::thread m_Thread;
			sol::coroutine m_Coroutine;
			std::chrono::steady_clock::time_point m_NextRun;
			std::vector<sol::object> m_Args;
		};

		struct LuaProcessWatcher
		{
			std::string m_ProcessName;
			sol::protected_function m_Callback;
			std::chrono::milliseconds m_CallbackDelayMs = 0ms;
		};

		std::string m_Name;
		fs::path m_Root;
		fs::path m_Entry;

		sol::state m_LuaState;

		eLuaLoadState m_LoadState{NONE};

		std::mutex m_ProcessWatcherMutex;
		std::mutex m_TaskMutex{};

		std::vector<LuaTask> m_Tasks{};
		std::vector<sol::protected_function> m_ShutdownCallbacks{};
		std::vector<LuaProcessWatcher> m_ProcessWatchers{}; // was named m_ProcessHandlers but it looked a lot like m_ProcessHandles

		std::atomic_bool m_IsRunningTasks{false};
		std::chrono::time_point<std::chrono::steady_clock> m_LastProcessPollTime{};

		DirectoryWatcher m_DirectoryWatcher; // was initially part of LuaManager
	};

	inline LuaModule* GetModuleFromLuaState(lua_State* L, std::optional<std::string_view> errorMsg = std::nullopt)
	{
		auto sv = sol::state_view(L);
		auto ptr = sv["this*"];
		if (!ptr.is<void*>())
		{
			if (errorMsg.has_value())
				sv.safe_script(std::format("error('{}', 2)", errorMsg.value().data()));
			else
				LOG_ERROR("Module pointer is null!");

			return nullptr;
		}

		return static_cast<LuaModule*>(ptr.get<void*>());
	};
}
