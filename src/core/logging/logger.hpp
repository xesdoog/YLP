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

#include <source_location>

#include "conout_sink.hpp"
#include "file_sink.hpp"
#include "imgui_sink.hpp"


namespace YLP
{
	class Logger : Singleton<Logger>
	{
		friend class Singleton<Logger>;

	public:
		Logger() noexcept = default;
		~Logger()
		{
			Destroy();
		}

		Logger(const Logger&) = delete;
		Logger& operator=(const Logger&) = delete;

		static void Init(const std::filesystem::path& filePath, bool enableConsole = false, uintmax_t maxFileSize = 0x7D000)
		{
			GetInstance().InitImpl(filePath, enableConsole, maxFileSize);
		}

		static void Destroy()
		{
			GetInstance().DestroyImpl();
		}

		static void InstallExceptionHandler();

		static void Log(eLogLevel level, std::string_view msg, std::source_location location = std::source_location::current())
		{
			GetInstance().LogImpl(level, std::string(msg), location);
		}

		template<typename... Args>
		static void Log(eLogLevel level, std::source_location location, std::string_view fmt, Args&&... args)
		{
			GetInstance().LogImpl(level, std::vformat(fmt, std::make_format_args(args...)), location);
		}

		static const std::deque<ILogSink*>& GetSinks()
		{
			return GetInstance().m_Sinks;
		}

		static ImGuiSink& GetImGuiSink()
		{
			return GetInstance().m_ImGuiSink;
		}

		static void ToggleExternalConsole(bool toggle)
		{
			GetInstance().m_ConsoleSink.ToggleConsole(toggle);
		}

	private:
		void InitImpl(const std::filesystem::path& filePath, bool enableConsole, uintmax_t maxFileSize);
		void LogImpl(eLogLevel level, std::string_view msg, std::source_location location);
		void DestroyImpl();

		ConsoleSink m_ConsoleSink;
		FileSink m_FileSink;
		ImGuiSink m_ImGuiSink;
		std::deque<ILogSink*> m_Sinks{};
		std::atomic<bool> m_Initialized{false};
	};
}
