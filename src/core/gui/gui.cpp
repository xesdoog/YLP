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


#include "gui.hpp"
#include "theme_mgr.hpp"
#include "notifier.hpp"


namespace YLP
{
	using namespace Frontend;

	void GUI::InitImpl()
	{
		Fonts::Load(ImGui::GetIO());
		ThemeManager::Init();

		eTabID lastTabIdx = eTabID::TAB_MAIN;
		if (Config().restoreLastTab)
		{
			lastTabIdx = static_cast<eTabID>(Config().lastTabIndex);
			if (lastTabIdx >= eTabID::__COUNT || lastTabIdx < eTabID::TAB_MAIN)
				lastTabIdx = eTabID::TAB_MAIN;
		}
		m_ActiveTab = m_Tabs[TabIDToIndex(lastTabIdx)];
	}

	void GUI::OnTabSwitchImpl()
	{
		if (m_ActiveTab == m_NextTab)
		{
			m_IsTabSwitchInProgress = false;
			m_NextTab = nullptr;
			m_CallbackChildAlpha = 1.f;
			return;
		}

		const float fadeSpeed = 3.86f;
		auto& io = ImGui::GetIO();

		if (m_IsTabSwitchInProgress)
		{
			m_CallbackChildAlpha -= io.DeltaTime * fadeSpeed;
			if (m_CallbackChildAlpha <= 0.0f)
			{
				m_ActiveTab = m_NextTab;
				m_NextTab = nullptr;
				m_CallbackChildAlpha = 0.0f;
				m_IsTabSwitchInProgress = false;
			}
		}
		else if (m_CallbackChildAlpha < 1.0f)
		{
			m_CallbackChildAlpha += io.DeltaTime * fadeSpeed;
			if (m_CallbackChildAlpha > 1.0f)
				m_CallbackChildAlpha = 1.0f;
		}
	}

	void GUI::SetActiveTabImpl(const eTabID& id)
	{
		if (id < eTabID::TAB_MAIN || id >= eTabID::__COUNT)
			return;

		if (m_ActiveTab && m_ActiveTab->GetID() == id)
			return;

		auto tab = m_Tabs[TabIDToIndex(id)];
		if (tab)
		{
			m_IsTabSwitchInProgress = true;
			m_NextTab = tab;
		}
	}

	void GUI::RefreshCurrentTabImpl()
	{
		if (!m_ActiveTab)
			return;

		m_NextTab = m_ActiveTab;
		m_ActiveTab = nullptr;
		m_IsTabSwitchInProgress = true;
	}

	void GUI::DrawImpl()
	{
		m_WindowSize = Renderer::GetWindowSize();
		ImGui::SetNextWindowSize(m_WindowSize, ImGuiCond_Always);
		ImGui::SetNextWindowPos(ImVec2(0.f, 0.f), ImGuiCond_Always);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::Begin("YLP", nullptr,
		    ImGuiWindowFlags_NoMove
		    | ImGuiWindowFlags_NoResize
		    | ImGuiWindowFlags_NoTitleBar);

		ImGui::PopStyleVar();
		ImGui::BeginDisabled(m_ShouldDisableUI);

		const float consoleChildHeight = std::min(m_WindowSize.y * 0.3f, 240.0f);
		float mainChildHeight = Config().internalConsole ? m_WindowSize.y - consoleChildHeight : ImGui::GetContentRegionAvail().y;
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, .11f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 8.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 8.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(8.0f, 8.0f));
		ImGui::BeginChild("##sidebar", ImVec2(m_SidebarWidth, mainChildHeight), 
			ImGuiChildFlags_Borders, 
			ImGuiWindowFlags_AlwaysUseWindowPadding 
			| ImGuiWindowFlags_NoScrollbar);
		DrawSideBarImpl();
		ImGui::EndChild();
		ImGui::PopStyleVar(4);

		ImGui::SameLine();
		ImGui::BeginChild("##main", ImVec2(0, mainChildHeight), 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoBackground);
		DrawTopBarImpl();

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_CallbackChildAlpha);
		ImGui::BeginChild("##main_scroll_region", ImVec2(0, 0), 0, ImGuiWindowFlags_NoBackground);
		if (m_ActiveTab)
			m_ActiveTab->Draw();
		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::EndChild();

		DrawDebugConsoleImpl();
		ImGui::EndDisabled();

		OnTabSwitchImpl();
		Notifier::DrawToasts();
		ImGui::End();
	}

	void GUI::DrawTopBarImpl()
	{
		auto version = YLPUpdater.GetLocalVersion();
		ImGui::PushFont(Fonts::Small);
		ImGui::TextDisabled("v%s", version.ToString().c_str());
		ImGui::PopFont();

		ImGuiStyle& style = ImGui::GetStyle();
		const bool isSnoozed = Notifier::IsSnoozed();
		const char* notifIcon = isSnoozed ? ICON_MD_NOTIFICATIONS_OFF : (Notifier::IsViewed() ? ICON_MD_NOTIFICATIONS : ICON_MD_NOTIFICATIONS_ACTIVE);
		ImVec4 notifColor = Notifier::IsViewed() ? style.Colors[ImGuiCol_Text] : style.Colors[ImGuiCol_CheckMark];
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x - 10.f, 10.f));
		ImGui::TextColored(Notifier::IsOpen() ? style.Colors[ImGuiCol_ButtonActive] : notifColor, notifIcon);

		if (ImGui::IsItemHovered())
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

		if (ImGui::IsItemClicked())
			Notifier::Toggle();

		ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.f);
		Notifier::Draw();
		ImGui::PopStyleVar();
		ImGui::Dummy(ImVec2(-1, 10));
	}

	void GUI::DrawSideBarImpl()
	{
		size_t tabCount = m_Tabs.size();
		if (tabCount == 0)
			return;

		const float frameH = ImGui::GetFrameHeight();
		static float padding = 8.0f;
		static float accentY = 0.0f;
		static float accentHeight = 0.0f;
		static float accentTargetY = 0.0f;
		static float accentTargetHeight = 0.0f;

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::SetCursorPosY(60);
		ImGui::PushFont(Fonts::Title);
		float iconWidth = ImGui::CalcTextSize(ICON_MD_EXTENSION).x;
		float offsetX = (ImGui::GetContentRegionAvail().x - iconWidth) * 0.5f;
		for (size_t i = 0; i < tabCount; i++)
		{
			auto tab = m_Tabs[i];
			if (!tab)
				continue;

			bool selected = (m_ActiveTab == tab || m_NextTab == tab);
			std::string_view name = tab->GetName();
			ImGui::SetCursorPosX(offsetX);
			if (ImGui::SelectableLabel(name.data(), selected))
			{
				m_IsTabSwitchInProgress = true;
				m_NextTab = tab;
				Config().lastTabIndex = i;
			}
			auto hint = tab->GetHint();
			auto tooltip = hint ? hint.value_or(name) : name;
			ImGui::ToolTip(tooltip.data());

			if (selected)
			{
				accentTargetY = ImGui::GetItemRectMin().y;
				accentTargetHeight = frameH;
			}

			ImGui::Dummy(ImVec2(0, padding));
		};
		ImGui::PopFont();

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		const ImVec2 childMin = ImGui::GetWindowPos();
		const ImVec2 childMax = childMin + ImGui::GetWindowSize();
		const float accentWidth = 4.0f;
		float speed = ImGui::GetIO().DeltaTime * 12.0f;
		float accentPosX = childMax.x - accentWidth;
		accentY = ImLerp(accentY, accentTargetY, speed);
		accentHeight = ImLerp(accentHeight, accentTargetHeight, speed);
		ImVec2 accentMin(accentPosX, accentY);
		ImVec2 accentMax(accentPosX + accentWidth, accentY + accentHeight);
		drawList->AddRectFilled(
			accentMin, 
			accentMax, 
			ImGui::GetColorU32(ImGuiCol_ButtonActive), 
			style.FrameRounding
		);
	}

	void GUI::DrawDebugConsoleImpl()
	{
		if (!Config().internalConsole)
			return;

		ImGui::Spacing();
		if (ImGui::BeginChild("##console", ImVec2(0, 0), ImGuiChildFlags_Border))
		{
			auto& imguiSink = Logger::GetImGuiSink();
			auto& entries = imguiSink.GetEntries();
			ImGui::PushFont(Fonts::Small);
			ImGui::BeginDisabled(entries.empty());

			if (ImGui::Button(ICON_MD_CONTENT_COPY))
				ImGui::SetClipboardText(imguiSink.GetText().c_str());
			ImGui::ToolTip("Copy all log entries");

			ImGui::SameLine();
			if (ImGui::Button(ICON_MD_DELETE))
				imguiSink.Clear();
			ImGui::ToolTip("Clear all log entries");

			ImGui::EndDisabled();
			ImGui::Spacing();

			imguiSink.Draw();

			ImGui::PopFont();
		}
		ImGui::EndChild();
	}
}
