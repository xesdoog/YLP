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

#include "log_entry.hpp"


namespace YLP
{
	static std::string LogLevelToString(eLogLevel lvl)
	{
		switch (lvl)
		{
		case eLogLevel::Info: return "INFO";
		case eLogLevel::Warn: return "WARN";
		case eLogLevel::Error: return "ERROR";
		case eLogLevel::Debug: return "DEBUG";
		default: return "UNKNOWN";
		}
	}

	class ILogSink
	{
	public:
		virtual void Write(const LogEntry& entry) = 0;
		virtual std::string FormatEntry(const LogEntry& entry)
		{
			return std::format("[{}] [{}] [{}:{}] {}\n",
				entry.timestamp,
				LogLevelToString(entry.level),
				entry.filename,
				entry.line,
			    entry.message
			);
		}

		void Init(){};
		void Destroy(){};
		void Flush(){};
	};
}
