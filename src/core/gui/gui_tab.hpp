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

#include "notifier.hpp"


namespace YLP
{
	enum class eTabID : uint8_t
	{
		TAB_MAIN,
		TAB_YIMMENU_LUA,
		TAB_INJECTOR,
		TAB_SCRIPTING,
		TAB_SETTINGS,
		TAB_INFO,
		__COUNT,
	};

	class GuiTab
	{
	public:
		GuiTab(eTabID id, std::string_view name, std::optional<std::string_view> hint = std::nullopt);

		virtual ~GuiTab() = default;
		virtual void Draw() = 0;

		eTabID GetID() const noexcept
		{
			return m_ID;
		}

		std::string_view GetName() const noexcept
		{
			return m_Name;
		}

		std::optional<std::string_view> GetHint() const noexcept
		{
			return m_Hint;
		}

	private:
		eTabID m_ID;
		std::string_view m_Name;
		std::optional<std::string_view> m_Hint;
	};
}
