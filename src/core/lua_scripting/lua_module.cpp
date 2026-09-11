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


#include "lua_module.hpp"
#include "lua_mgr.hpp"


namespace YLP::LuaJIT
{
	// https://sol2.readthedocs.io/en/latest/exceptions.html
	static int exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_exception, sol::string_view description)
	{
		if (maybe_exception)
		{
			const std::exception& e = *maybe_exception;
			LOG_ERROR(e.what());
		}
		else
		{
			LOG_ERROR(description);
		}
		return sol::stack::push(L, description);
	}

	static inline void panic_handler(sol::optional<std::string> maybe_msg)
	{
		LOG_ERROR("Lua is in a panic state and will now abort the application");
		if (maybe_msg)
		{
			const std::string& msg = maybe_msg.value();
			LOG_ERROR("error message: {}", msg);
		}
	}

	static int traceback_error_handler(lua_State* L)
	{
		std::string msg = "An unknown error has triggered the error handler";
		sol::optional<sol::string_view> maybetopmsg = sol::stack::unqualified_check_get<sol::string_view>(L, 1, &sol::no_panic);
		if (maybetopmsg)
		{
			const sol::string_view& topmsg = maybetopmsg.value();
			msg.assign(topmsg.data(), topmsg.size());
		}
		luaL_traceback(L, L, msg.c_str(), 1);
		sol::optional<sol::string_view> maybetraceback = sol::stack::unqualified_check_get<sol::string_view>(L, -1, &sol::no_panic);
		if (maybetraceback)
		{
			const sol::string_view& traceback = maybetraceback.value();
			msg.assign(traceback.data(), traceback.size());
		}
		LOG_ERROR(msg);
		return sol::stack::push(L, msg);
	}

	LuaModule::LuaModule(fs::path root) :
		m_LuaState(sol::state{}),
		m_Root(root),
		m_Entry(root / "main.lua"),
		m_Name(root.filename().string())
	{
		m_LuaState.open_libraries(
		    sol::lib::base,
		    sol::lib::bit32,
		    sol::lib::coroutine,
		    sol::lib::ffi,
		    sol::lib::jit,
		    sol::lib::math,
		    sol::lib::string,
		    sol::lib::table,
		    sol::lib::utf8);

		m_LuaState["this*"]  = reinterpret_cast<void*>(this);
		m_LuaState["whodis"] = m_Name;

		m_LuaState.set_exception_handler(exception_handler);
		m_LuaState.set_panic(sol::c_call<decltype(&panic_handler), &panic_handler>);
		lua_CFunction traceback_function = sol::c_call<decltype(&traceback_error_handler), &traceback_error_handler>;
		sol::protected_function::set_default_handler(sol::object(m_LuaState.lua_state(), sol::in_place, traceback_function));

		LuaManager::RegisterLibraries(m_LuaState);
		m_DirectoryWatcher = DirectoryWatcher(root, 1s);
	}

	LuaModule::~LuaModule()
	{
		std::unique_lock lock(m_TaskMutex);
		m_Tasks.clear();
		m_ProcessWatchers.clear();

		for (auto& func : m_ShutdownCallbacks)
		{
			if (func.valid())
				func();
		}
	}

	bool LuaModule::Load()
	{
		if (!Config().enableScripting)
			return false;

		auto result = m_LuaState.safe_script_file(m_Entry.string(), &sol::script_pass_on_error, sol::load_mode::text);
		if (!result.valid())
		{
			sol::error e = result;
			LOG_ERROR("Failed to load module '{}' : {}", m_Name, e.what());
			m_LoadState = BROKEN;
			return false;
		}

		LOG_INFO("Loaded module '{}'", m_Name);
		m_LoadState = RUNNING;
		return true;
	}

	void LuaModule::Reload()
	{
		m_LoadState = WANTS_RELOAD;
	}

	void LuaModule::Unload()
	{
		m_LoadState = WANTS_UNLOAD;
	}

	const bool LuaModule::IsSafeToUnload() const noexcept
	{
		return !m_IsRunningTasks;
	}

	lua_State* LuaModule::GetLuaState()
	{
		return m_LuaState.lua_state();
	}

	const LuaModule::eLuaLoadState LuaModule::GetLoadState() const noexcept
	{
		return m_LoadState;
	}

	std::string_view LuaModule::GetName() const noexcept
	{
		return m_Name;
	}

	fs::path LuaModule::GetRoot() const noexcept
	{
		return m_Root;
	}

	const bool LuaModule::IsRunningTasks() const noexcept
	{
		return m_IsRunningTasks;
	}

	void LuaModule::Execute(const std::string_view& code)
	{
		if (!Config().enableScripting)
			return;

		if (auto result = m_LuaState.safe_script(code.data(), &sol::script_pass_on_error); !result.valid())
		{
			sol::error error = result;
			LOG_ERROR(error.what());
		}
	}

	void LuaModule::RegisterTask(sol::protected_function func, std::chrono::milliseconds delayMs, std::vector<sol::object> args)
	{
		std::unique_lock lock(m_TaskMutex);
		auto thread = sol::thread::create(m_LuaState);
		auto co = sol::coroutine(m_LuaState, func);
		m_Tasks.push_back({
			std::move(thread),
			std::move(co),
		    std::chrono::steady_clock::now() + delayMs,
		    std::move(args)
		});
	}

	void LuaModule::RegisterProcessWatcher(const std::string& processName, sol::protected_function callback, std::chrono::milliseconds delayMs)
	{
		m_ProcessWatchers.push_back({processName, std::move(callback), delayMs});
	}

	void LuaModule::RegisterShutdownCallback(sol::protected_function callback)
	{
		m_ShutdownCallbacks.push_back(std::move(callback));
	}

	void LuaModule::DispatchProcessWatchers(const std::chrono::steady_clock::time_point& tickStart)
	{
		if (tickStart - m_LastProcessPollTime < 500ms)
			return;

		std::erase_if(m_ProcessWatchers, [this](const auto& watcher) {
			if (!PsUtils::GetProcessId(watcher.m_ProcessName).has_value())
				return false;

			if (watcher.m_Callback.valid())
			{
				auto proc = sol::make_object<ProcessScanner>(m_LuaState, watcher.m_ProcessName);
				RegisterTask(watcher.m_Callback, watcher.m_CallbackDelayMs, {std::move(proc)});
			}

			return true;
		});

		m_LastProcessPollTime = tickStart;
	}

	void LuaModule::Tick()
	{
		const auto now = std::chrono::steady_clock::now();
		m_IsRunningTasks.store(true);
		DispatchProcessWatchers(now);

		{
			std::scoped_lock lock(m_TaskMutex);
			for (auto it = m_Tasks.begin(); it != m_Tasks.end();)
			{
				if (!g_Running)
					break;

				if (now < it->m_NextRun)
				{
					++it;
					continue;
				}

				auto result = it->m_Coroutine(sol::as_args(it->m_Args));
				if (!result.valid())
				{
					sol::error error = result;
					LOG_ERROR(error.what());
					it = m_Tasks.erase(it);
					continue;
				}

				if (!it->m_Coroutine.runnable())
				{
					it = m_Tasks.erase(it);
					continue;
				}

				const auto delay = result.return_count() > 0 ? result[0] : 0;
				it->m_NextRun = now + std::chrono::milliseconds(delay);
				++it;
			}
		}

		m_DirectoryWatcher.PollOnce([this](const fs::path& _unused, DirectoryWatcher::ePathStatus status) {
			switch (status) // TODO: handle different cases properly
			{
			case DirectoryWatcher::ePathStatus::Created:
			case DirectoryWatcher::ePathStatus::Modified:
			case DirectoryWatcher::ePathStatus::Erased:
				m_LoadState = WANTS_RELOAD;
				break;
			}
		});

		m_IsRunningTasks.store(false);
	}
}
