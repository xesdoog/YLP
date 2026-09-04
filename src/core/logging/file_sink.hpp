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
	class FileSink final : public ILogSink
	{
	public:
		~FileSink()
		{
			Destroy();
		}

		void Init(fs::path path, uintmax_t maxFileSize)
		{
			m_FilePath = path;
			m_MaxFileSize = maxFileSize;

			Rotate();
			m_FileStream.open(m_FilePath, std::ios::out | std::ios::app);
			if (!m_FileStream.is_open())
			{
				std::fprintf(stderr, "[Logger]: failed to open file: %s\n", m_FilePath.string().c_str());
				return;
			}

			m_FileStream << "\n\n========== Initializing YLP ==========\n\n";
		}

		void Destroy()
		{
			if (m_FileStream.is_open())
			{
				m_FileStream << "\n\n============== Farewell ==============\n\n";
				m_FileStream.close();
			}
		}

		void Write(const LogEntry& entry) override
		{
			if (!m_FileStream.is_open())
				return;

			m_FileStream << FormatEntry(entry);
		}

		void Flush()
		{
			if (m_FileStream.is_open())
				m_FileStream.flush();
		}

	private:
		void Rotate()
		{
			std::error_code ec{};
			if (!fs::exists(m_FilePath, ec))
				return;

			auto fileSize = fs::file_size(m_FilePath, ec);
			if (fileSize < m_MaxFileSize)
				return;

			if (m_FileStream.is_open())
			{
				m_FileStream.flush();
				m_FileStream.close();
			}

			auto now = std::chrono::system_clock::now();
			std::time_t t = std::chrono::system_clock::to_time_t(now);
			std::tm tm{};
			localtime_s(&tm, &t);

			std::ostringstream oss;
			oss << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S");
			std::string timestamp = oss.str();

			auto rotatedDir = g_ProjectPath / "Logs";
			if (!fs::create_directories(rotatedDir, ec))
				return;

			auto rotatedFile = rotatedDir / (m_FilePath.filename().string() + "_" + timestamp + ".log");
			try
			{
				fs::rename(m_FilePath, rotatedFile);
			}
			catch (const std::exception& e)
			{
				std::fprintf(stderr, "[Logger] Failed to rotate log: %s\n", e.what());
				return;
			}

			std::deque<fs::path> backups;
			for (auto& f : fs::directory_iterator(rotatedDir))
			{
				if (!f.is_regular_file() || f.path().extension() != ".log")
					fs::remove(f);

				backups.push_back(f.path());
			}

			size_t backupSize = backups.size();
			if (backupSize > 5)
			{
				std::sort(backups.begin(), backups.end(), [](auto& a, auto& b) {
					return fs::last_write_time(a) > fs::last_write_time(b);
				});

				for (size_t i = 5; i < backupSize; ++i)
					fs::remove(backups[i]);
			}
			backups.clear();

			m_FileStream.open(m_FilePath, std::ios::out | std::ios::trunc);
			if (!m_FileStream.is_open())
			{
				std::fprintf(stderr, "[Logger] Failed to re-open log after rotation!\n");
				return;
			}
		}

		fs::path m_FilePath;
		std::ofstream m_FileStream;
		uintmax_t m_MaxFileSize{0x7D000};
	};
}
