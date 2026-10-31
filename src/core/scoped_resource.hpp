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
	template<typename T, auto Deleter, T Invalid = nullptr>
	class ScopedResource
	{
	private:
		T m_Handle = Invalid;

	public:
		ScopedResource() = default;

		explicit ScopedResource(T handle) noexcept :
		    m_Handle(handle)
		{
		}

		~ScopedResource()
		{
			Reset();
		}

		ScopedResource(const ScopedResource&)            = delete;
		ScopedResource& operator=(const ScopedResource&) = delete;

		ScopedResource(ScopedResource&& other) noexcept :
		    m_Handle(std::exchange(other.m_Handle, Invalid))
		{
		}

		ScopedResource& operator=(ScopedResource&& other) noexcept
		{
			if (this != &other)
			{
				Reset();
				m_Handle = std::exchange(other.m_Handle, Invalid);
			}

			return *this;
		}

		bool IsValid() const noexcept
		{
			return m_Handle != Invalid;
		}

		explicit operator bool() const noexcept
		{
			return IsValid();
		}

		T Get() const noexcept
		{
			return m_Handle;
		}

		void Reset(T handle = Invalid) noexcept
		{
			if (IsValid())
				Deleter(m_Handle);

			m_Handle = handle;
		}

		T Release() noexcept
		{
			return std::exchange(m_Handle, Invalid);
		}

		bool operator==(T other) const noexcept
		{
			return m_Handle == other;
		}
	};

	using ScopedHandle = ScopedResource<HANDLE, CloseHandle, INVALID_HANDLE_VALUE>;
	using ScopedIconHandle = ScopedResource<HICON, DestroyIcon>;
	using ScopedDC = ScopedResource<HDC, DeleteDC>;
	using ScopedBitmap = ScopedResource<HBITMAP, DeleteObject>;
	using ScopedModule = ScopedResource<HMODULE, FreeLibrary>;
	using ScopedView = std::unique_ptr<void, decltype(&UnmapViewOfFile)>;
}
