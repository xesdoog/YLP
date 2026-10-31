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

#include "../core/gui/gui_tab.hpp"
#include "../core/github/gitmgr.hpp"
#include "../core/gui/renderer.hpp"
#include "../core/gui/widgets/ylp_spinner.hpp"


namespace YLP::Frontend
{
	class YimLuaTab final : public GuiTab
	{
	public:
		YimLuaTab() :
		    GuiTab(eTabID::TAB_YIMMENU_LUA, ICON_MS_EXTENSION, "YimMenu-Lua")
		{
		}

	private:

		static inline ImGuiChildFlags cardChildFlags = ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY;
		static inline ImGuiWindowFlags cardWinFlags  = ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize;

		struct SortOption
		{
			const char* label;
			eLuaRepoSortMode mode;
		};

		static constexpr inline const SortOption SortOptions[] = {
		    {ICON_MS_HISTORY " Recently Updated", eLuaRepoSortMode::COMMIT},
		    {ICON_MS_STAR " Stars", eLuaRepoSortMode::STARS},
		    {ICON_MS_SORT_BY_ALPHA " Name", eLuaRepoSortMode::NAME},
		    {ICON_MS_INSTALL_DESKTOP " Installed", eLuaRepoSortMode::INSTALLED},
		};

		static inline void DrawInstalledRepo(const Repository* repo)
		{
			if (ImGui::ColoredButton(ICON_MS_DELETE, ImVec4(0.95f, 0.20f, 0.20f, 0.69f), 3.0f))
				GitHubManager::RemoveRepository(repo->m_Name);
			ImGui::ToolTip("Delete");

			ImGui::SameLine();
			if (repo->m_IsDisabled)
			{
				if (ImGui::ColoredButton(ICON_MS_CHECK_CIRCLE, ImVec4(0.20f, 0.95f, 0.20f, 0.69f), 2.0f))
				{
					const std::string& name = repo->m_Name;

					auto src  = g_YimPath / "scripts" / "disabled" / name;
					auto dest = g_YimPath / "scripts" / name;
					GitHubManager::MoveRepository(name, src, dest);
				}
				ImGui::ToolTip("Enable");
			}
			else
			{
				if (ImGui::Button(ICON_MS_BLOCK))
				{
					const std::string& name = repo->m_Name;

					auto src  = g_YimPath / "scripts" / name;
					auto dest = g_YimPath / "scripts" / "disabled" / name;
					GitHubManager::MoveRepository(name, src, dest);
				}
				ImGui::ToolTip("Disable");

				if (repo->m_IsPendingUpdate)
				{
					ImGui::SameLine();
					if (ImGui::Button(ICON_MS_UPDATE))
						GitHubManager::DownloadRepository(repo->m_Name);
					ImGui::ToolTip("Update");

					auto icon         = ICON_MS_CLOUD_DOWNLOAD;
					float iconWidth   = ImGui::CalcTextSize(icon).x;
					float regionWidth = ImGui::GetContentRegionAvail().x;

					ImGui::SameLine();
					ImGui::PushFont(Fonts::Title);
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + regionWidth - iconWidth);
					ImGui::TextColored(ImVec4(0.15, 0.35, 0.6, 1.0), icon);
					ImGui::PopFont();
					ImGui::ToolTip("Update Available!", Fonts::Regular, false);
				}
			}

			ImGui::SameLine();
			if (ImGui::Button(ICON_MS_FOLDER))
				IO::Open(repo->m_CurrentPath.string());
			ImGui::ToolTip("Open folder");
		}

		inline void DrawRepoCard(const Repository* repo) const
		{
			ImGui::BeginChild(repo->m_HtmlUrl.c_str(), ImVec2(), cardChildFlags, cardWinFlags);

			const std::string& name = repo->m_Name;
			const std::string& desc = repo->m_Description.empty() ? "No description." : repo->m_Description;
			std::string date        = Utils::FormatDateShort(repo->m_LastUpdate);

			ImGuiStyle& style      = ImGui::GetStyle();
			ImVec4 textCol         = style.Colors[ImGuiCol_Text];
			const auto titleSize   = ImGui::CalcTextSize(name.c_str());
			const auto rectMax     = ImVec2(ImGui::GetCursorScreenPos().x + titleSize.x + 5.f, ImGui::GetCursorScreenPos().y + titleSize.y + 5.f);
			const bool tileHovered = ImGui::IsMouseHoveringRect(ImGui::GetCursorScreenPos(), rectMax) && !m_SortPopupOpen;

			if (tileHovered)
			{
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				ImGui::SetTooltip("Open in browser.");
			}

			if (tileHovered && ImGui::IsMouseClicked(0))
				IO::Open(repo->m_HtmlUrl);

			ImGui::Spacing();
			ImGui::TextDisabled(ICON_MS_BOOK);
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.3f, 0.5f, 0.8f, 1.0f), name.c_str());

			ImGui::PushFont(Fonts::Small);
			ImGui::Spacing();
			ImGui::BeginDisabled();
			ImGui::TextWrapped(desc.c_str());
			ImGui::EndDisabled();
			if (ImGui::CalcTextSize(desc.c_str()).y >= (ImGui::GetContentRegionAvail().y + 20))
				ImGui::ToolTip(desc.c_str());

			ImGui::Dummy(ImVec2(0, 10));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 128, 255));
			ImGui::Text(ICON_MS_BRIGHTNESS_1);
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::TextDisabled("Lua");
			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.f, 0.f));
			ImGui::TextDisabled(ICON_MS_STAR);
			ImGui::SameLine();
			ImGui::TextDisabled("%d", repo->m_Stars);
			ImGui::PopStyleVar();

			float dateWidth = ImGui::CalcTextSize(date.c_str()).x;
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - dateWidth * 2);
			ImGui::TextDisabled("Updated on: %s", date.c_str());
			ImGui::Separator();

			ImGui::BeginDisabled(!std::filesystem::exists(g_YimPath / "scripts"));
			if (repo->m_IsInstalled)
				DrawInstalledRepo(repo);
			else if (repo->m_IsDownloading)
			{
				ImGui::Spinner("", 7.1f);
				ImGui::Spacing();
			}
			else
			{
				if (ImGui::Button(ICON_MS_DOWNLOAD))
					GitHubManager::DownloadRepository(name);
				ImGui::ToolTip("Download");
			}
			ImGui::EndDisabled();
			ImGui::PopFont();
			ImGui::EndChild();
		}

	public:
		void Draw() override
		{
			auto sortMode  = GitHubManager::GetSortMode();
			auto sortOrder = GitHubManager::GetSortOrder();
			auto state     = GitHubManager::GetState();

			if (ImGui::Button(ICON_MS_SORT))
				ImGui::OpenPopup("##sortScripts");

			ImVec2 sortPopupPos = ImGui::GetItemRectMax();
			if (ImGui::IsPopupOpen("##sortScripts"))
				ImGui::SetNextWindowPos(ImVec2(sortPopupPos.x, sortPopupPos.y), ImGuiCond_Always);

			ImGui::SameLine();
			ImGui::Text("Sort");

			if (ImGui::BeginPopup("##sortScripts"))
			{
				m_SortPopupOpen = true;

				ImGui::TextDisabled("Sort By");
				ImGui::Spacing();

				for (const auto& opt : SortOptions)
				{
					if (ImGui::MenuItem(opt.label, nullptr, (opt.mode == sortMode)))
						GitHubManager::SetSortMode(opt.mode);
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::TextDisabled("Order");
				ImGui::Spacing();

				if (ImGui::MenuItem(ICON_MS_ARROW_UPWARD " Ascending", nullptr, (sortOrder == ASC)))
					GitHubManager::SetSortOrder(ASC);

				if (ImGui::MenuItem(ICON_MS_ARROW_DOWNWARD " Descending", nullptr, (sortOrder == DESC)))
					GitHubManager::SetSortOrder(DESC);

				ImGui::EndPopup();
			}
			else
				m_SortPopupOpen = false;

			ImGui::SameLine(0.f, 10.f);
			if (ImGui::Button(ICON_MS_REFRESH))
				ThreadManager::Run([] { GitHubManager::FetchRepositories(); });

			ImGui::SameLine();
			ImGui::Text("Refresh");

			ImGui::SameLine(0.0f, 60.0f);
			ImGui::SetNextItemWidth(-1);
			ImGui::SearchBar("##searchBar", searchBuffer);

			ImGui::Separator();
			ImGui::Spacing();

			ImGui::SetNextWindowBgAlpha(0.0f);
			ImGui::BeginChild("##repoCards", ImVec2(0, 0), 0);
			switch (state)
			{
			case GitHubManager::eLoadState::NONE:
				if (ImGui::Button("Load Repositories"))
					GitHubManager::RefreshRepositories();
				break;

			case GitHubManager::eLoadState::LOADING:
			{
				auto region		 = ImGui::GetContentRegionAvail();
				auto minsize	 = std::min(region.x * 0.5f, region.y * 0.5f);
				auto spinnerSize = ImVec2(minsize, minsize);

				ImGui::SetCursorPos((ImGui::GetCursorPos() + region - spinnerSize) * 0.5);
				ImGui::YLPSpinner("##loading", spinnerSize);
				ImGui::Spacing();
				ImGui::Text("Loading Lua repositories...");
				break;
			}
			case GitHubManager::eLoadState::READY:
			{
				const float padding = 12.f;
				const auto& repos = GitHubManager::GetSortedRepos();
				if (repos.empty())
				{
					ImGui::TextDisabled("No repositories loaded.");
					return;
				}

				ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 2.f);
				for (size_t i = 0; i < repos.size(); ++i)
				{
					if (searchBuffer[0] != '\0' && Utils::StringToLower(repos[i]->m_Name).find(searchBuffer) == std::string::npos)
						continue;

					DrawRepoCard(repos[i]);
					ImGui::Spacing();
				}
				ImGui::PopStyleVar();
				break;
			}
			case GitHubManager::eLoadState::FAILED:
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to load repositories!");
				break;
			}
			ImGui::EndChild();
		}

	private:
		char searchBuffer[64]{};

		const float m_RepoCardHeight = 190;

		const ImVec2 m_CtrlBtnSize = ImVec2(120, 25);

		bool m_SortPopupOpen = false;
	};

	inline YimLuaTab _YimLuaTab;
}
