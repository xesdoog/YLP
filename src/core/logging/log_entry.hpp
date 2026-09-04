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


namespace YLP
{
	enum eLogLevel : uint8_t
	{
		Info,
		Warn,
		Error,
		Debug
	};

	struct LogEntry
	{
		const std::string timestamp;
		const eLogLevel level;
		const std::string message;
		const std::string filename;
		const std::string line;
		const std::string funcname;
	};
}
