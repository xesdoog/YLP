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


#include <exception>
#include <cstdio>
#include <ctime>

#include "logger.hpp"


namespace YLP
{
	void Logger::InstallExceptionHandler()
	{
		std::set_terminate([]() {
			try
			{
				if (auto p = std::current_exception())
					std::rethrow_exception(p);
				else
					Log(eLogLevel::Error, std::source_location::current(), "Terminated due to unknown exception!");
			}
			catch (const std::exception& e)
			{
				try
				{
					Log(eLogLevel::Error, std::source_location::current(), "Unhandled exception: %s", e.what());
				}
				catch (...)
				{
					std::fprintf(stderr, "Unhandled exception (logger unavailable): %s\n", e.what());
				}
			}
			catch (...)
			{
				try
				{
					Log(eLogLevel::Error, std::source_location::current(), "Unhandled non-standard exception!");
				}
				catch (...)
				{
					std::fprintf(stderr, "Unhandled non-standard exception (logger unavailable)!\n");
				}
			}
			std::abort();
		});
	}

	void Logger::InitImpl(const std::filesystem::path& file, bool enableConsole, uintmax_t maxFileSize)
	{
		m_ConsoleSink.Init(enableConsole);
		m_FileSink.Init(file, maxFileSize);

		m_Sinks.emplace_back(&m_ConsoleSink);
		m_Sinks.emplace_back(&m_FileSink);
		m_Sinks.emplace_back(&m_ImGuiSink);

		m_Initialized = true;
	}

	void Logger::DestroyImpl()
	{
		if (!m_Initialized)
			return;

		for (auto& s : m_Sinks)
		{
			s->Destroy();
		}
	}

	void Logger::LogImpl(eLogLevel lvl, std::string_view msg, std::source_location location)
	{
		if (!m_Initialized)
			return;

		const std::string ts = Utils::FormatTime();
		const LogEntry entry{
			.timestamp = ts,
			.level = lvl,
			.message = std::string(msg),
		    .filename = fs::path(location.file_name()).filename().string(),
		    .line = std::to_string(location.line()),
		    .funcname = location.function_name()
		};

		for (auto& s : m_Sinks)
		{
			s->Write(entry);
		}
	}
}
