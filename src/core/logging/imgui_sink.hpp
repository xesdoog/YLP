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

#include <deque>
#include "log_sink.hpp"


namespace YLP
{
	class ImGuiSink final : public ILogSink
	{
	public:
		void Write(const LogEntry& entry) override
		{
			std::scoped_lock lock(m_EntriesMutex);
			m_Entries.push_back(entry);

			if (m_Entries.size() >= m_MaxEntries)
				m_Entries.pop_front();
		}

		const auto& GetEntries() const noexcept
		{
			return m_Entries;
		}

		const ImVec4 GetLevelColor(const LogEntry& entry)
		{
			switch (entry.level)
			{
			case eLogLevel::Info: return ImVec4(0.7f, 0.7f, 0.7f, 1.f);
			case eLogLevel::Warn: return ImVec4(1.f, 0.8f, 0.f, 1.f);
			case eLogLevel::Error: return ImVec4(1.f, 0.3f, 0.3f, 1.f);
			case eLogLevel::Debug: return ImVec4(0.5f, 0.8f, 1.f, 1.f);			
			}
			return ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
		}

		void Clear()
		{
			std::scoped_lock lock(m_EntriesMutex);
			m_Entries.clear();
		}

	private:
		std::deque<LogEntry> m_Entries;
		std::mutex m_EntriesMutex;
		size_t m_MaxEntries{100};
	};
}
