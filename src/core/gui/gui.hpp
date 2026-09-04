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

#include "renderer.hpp"
#include "fonts/fonts.hpp"
#include "frontend/injector.hpp"
#include "frontend/main_window.hpp"
#include "frontend/yimmenu_lua.hpp"
#include "frontend/plugins.hpp"
#include "frontend/settings.hpp"


namespace YLP
{
	using GuiCallback = std::function<void()>;

	class GUI : public Singleton<GUI>
	{
		friend class Singleton<GUI>;

	private:
		GUI() = default;
		~GUI() noexcept = default;
	public:
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

	private:
		struct Tab
		{
			const eTabID m_ID;
			const std::string_view m_Name;
			const GuiCallback m_Callback;
			const std::optional<std::string_view> m_Hint;
		};

	public:
		static void Init()
		{
			GetInstance().InitImpl();
		}

		static void Draw()
		{
			GetInstance().DrawImpl();
		}

		static void ToggleDisableUI(bool toggle) noexcept
		{
			GetInstance().m_ShouldDisableUI = toggle;
		}

		static void SetActiveTab(const eTabID& id)
		{
			GetInstance().SetActiveTabImpl(id);
		}

		static void RefreshCurrentTab()
		{
			GetInstance().RefreshCurrentTabImpl();
		}

		static constexpr size_t TabIDToIndex(eTabID id)
		{
			return static_cast<size_t>(id);
		}

		static void DrawAboutSection();

	private:
		void InitImpl();
		void DrawImpl();
		void DrawTopBarImpl();
		void DrawSideBarImpl();
		void DrawDebugConsoleImpl();
		void InititializeTabImpl(const eTabID& id, const std::string_view& name, GuiCallback&& callback, std::optional<std::string_view> hint);
		void OnTabSwitchImpl();
		void SetActiveTabImpl(const eTabID& tabID);
		void RefreshCurrentTabImpl();

		bool m_ShouldDisableUI = false;
		bool m_IsTabSwitchInProgress = false;
		float m_CallbackChildAlpha = 1.0f;
		float m_SidebarWidth = 60.0f;
		Tab* m_ActiveTab = nullptr;
		Tab* m_NextTab = nullptr;
		ImVec2 m_WindowSize{};

		std::array<std::unique_ptr<Tab>, static_cast<size_t>(eTabID::__COUNT)> m_Tabs;
	};
}
