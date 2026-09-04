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

#include "log_sink.hpp"


namespace YLP
{
	class ConsoleSink final : public ILogSink
	{
	public:
		~ConsoleSink()
		{
			Destroy();
		}

		void Init(bool enableConsole)
		{
			if (!enableConsole)
				return;

			ToggleConsole(true);
		}

		void Destroy()
		{
			if (m_ConOut.is_open())
				m_ConOut << "\n============== Farewell ==============\n\n";

			ToggleConsole(false);
		}

		void Write(const LogEntry& entry) override
		{
			if (!m_IsEnabled)
				return;

			if (!m_ConOut.is_open())
			{
				m_IsEnabled = false;
				return;
			}

			m_ConOut << FormatEntry(entry);
			m_ConOut.flush();
		}

		void ToggleConsole(bool toggle)
		{
			if (m_IsEnabled == toggle)
				return;

			m_IsEnabled = toggle;

			m_ConOut.close();

			if (m_HadConsole && m_DefaultConsoleMode != NULL)
				SetConsoleMode(m_ConsoleHandle, m_DefaultConsoleMode);

			if (!m_HadConsole)
				FreeConsole();

			if (toggle)
			{
				if (m_HadConsole = AttachConsole(GetCurrentProcessId()); !m_HadConsole)
					AllocConsole();

				if (m_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); m_ConsoleHandle != nullptr)
				{
					SetConsoleTitleA("YLP");
					SetConsoleOutputCP(CP_UTF8);

					DWORD consoleMode;
					GetConsoleMode(m_ConsoleHandle, &consoleMode);
					m_DefaultConsoleMode = consoleMode;
					consoleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
					SetConsoleMode(m_ConsoleHandle, consoleMode);
				}

				m_ConOut.open("CONOUT$", std::ios_base::out | std::ios_base::app);
				if (m_ConOut.is_open())
					m_ConOut << R"(

  _        _          _             _      
 /\ \     /\_\       _\ \          /\ \    
 \ \ \   / / /      /\__ \        /  \ \   
  \ \ \_/ / /      / /_ \_\      / /\ \ \  
   \ \___/ /      / / /\/_/     / / /\ \_\ 
    \ \ \_/      / / /         / / /_/ / / 
     \ \ \      / / /         / / /__\/ /  
      \ \ \    / / / ____    / / /_____/   
       \ \ \  / /_/_/ ___/\ / / /          
        \ \_\/_______/\__\// / /           
         \/_/\_______\/    \/_/            



)";
			}
		}

		enum class eConsoleColor
		{
			RESET,
			BLACK = 30,
			RED = 31,
			GREEN = 32,
			YELLOW = 33,
			BLUE = 34,
			WHITE = 97,
		};

	private:
		std::ofstream m_ConOut{};
		bool m_IsEnabled{false};
		bool m_HadConsole{false};
		DWORD m_DefaultConsoleMode{};
		HANDLE m_ConsoleHandle{INVALID_HANDLE_VALUE};

		const eConsoleColor GetLevelColor(const eLogLevel level)
		{
			switch (level)
			{
			case Info: return eConsoleColor::GREEN;
			case Warn: return eConsoleColor::YELLOW;
			case Error: return eConsoleColor::RED;
			case Debug: return eConsoleColor::BLUE;
			}
			return eConsoleColor::WHITE;
		}

		std::string FormatEntry(const LogEntry& entry)
		{
			return std::format(
			    "\x1b[{}m[{}] [{}] [{}:{}]\x1b[0m {}\n",
			    static_cast<int>(GetLevelColor(entry.level)),
			    entry.timestamp,
				LogLevelToString(entry.level),
				entry.filename,
				entry.line,
			    entry.message);
		}
	};
}
