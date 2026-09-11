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
#pragma comment(lib, "Version.lib")

#include <winver.h>


namespace YLP
{
	class Updater
	{
	public:
		Updater() = default;
		~Updater() noexcept = default;

	private:
		struct Version
		{
			int m_Major = 0;
			int m_Minor = 0;
			int m_Patch = 0;
			int m_Build = 0;

			std::string m_CachedStr{};

			const std::string ToString()
			{
				if (m_CachedStr.empty())
				{
					std::ostringstream oss;
					oss << m_Major << "." << m_Minor << "." << m_Patch << "." << m_Build;
					m_CachedStr = oss.str();
				}

				return m_CachedStr;
			}

			constexpr explicit operator bool() const noexcept
			{
				return m_Major != 0;
			}

			constexpr bool operator<(const Version& other) const noexcept
			{
				if (m_Major != other.m_Major)
					return m_Major < other.m_Major;
				if (m_Minor != other.m_Minor)
					return m_Minor < other.m_Minor;
				if (m_Patch != other.m_Patch)
					return m_Patch < other.m_Patch;
				return m_Build < other.m_Build;
			}

			constexpr bool operator>(const Version& other) const noexcept
			{
				return other < *this;
			}

			constexpr bool operator!=(const Version& other) const noexcept
			{
				return m_Major != other.m_Major || m_Minor != other.m_Minor || m_Patch != other.m_Patch || m_Build != other.m_Build;
			}

			constexpr bool operator==(const Version& other) const noexcept
			{
				return m_Major == other.m_Major && m_Minor == other.m_Minor && m_Patch == other.m_Patch && m_Build == other.m_Build;
			}
		};

	public:
		enum UpdateState : uint8_t
		{
			Idle,
			Checking,
			Pending,
			Downloading,
			Ready,
			Error,
		};

		UpdateState GetState() const noexcept
		{
			return m_State;
		}

		float GetProgress() const noexcept
		{
			return m_DownloadProgress;
		}

		Version GetLocalVersion();
		Version GetRemoteVersion();

		void Check();
		void Download();
		void Update();
		void Reset();

	private:
		Version m_LocalVersion{};
		std::atomic<UpdateState> m_State{Idle};
		std::pair<std::wstring, std::wstring> m_ReleaseUrl{L"github.com", L"/xesdoog/YLP/releases/latest"};
		float m_DownloadProgress{0.f};
	};

	inline Updater YLPUpdater;
}
