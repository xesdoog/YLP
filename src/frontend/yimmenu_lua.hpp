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
#include "../core/gui/fonts/fonts.hpp"
#include "../core/gui/widgets/ylp_spinner.hpp"


namespace YLP::Frontend
{
	class YimLuaTab final : public GuiTab
	{
	public:
		YimLuaTab() :
		    GuiTab(eTabID::TAB_YIMMENU_LUA, ICON_MD_EXTENSION, "YimMenu-Lua repositories")
		{
		}
		
		static inline char searchBuffer[64];

		struct SortOption
		{
			const char* label;
			eLuaRepoSortMode mode;
		};

		static constexpr SortOption SortOptions[] = {
		    {"Recently Updated", eLuaRepoSortMode::COMMIT},
		    {"Stars", eLuaRepoSortMode::STARS},
		    {"Name (Descending)", eLuaRepoSortMode::NAME},
		    {"Installed", eLuaRepoSortMode::INSTALLED},
		};

		static const char* GetSortModeName(const eLuaRepoSortMode& mode)
		{
			for (const auto& option : SortOptions)
			{
				if (option.mode == mode)
					return option.label;

				return "Unknown";
			}
		}

		static inline void DrawInstalledRepo(const Repository* repo)
		{
			if (ImGui::ColoredButton(ICON_MD_DELETE, ImVec4(0.95f, 0.20f, 0.20f, 0.69f), 3.0f))
				GitHubManager::RemoveRepository(repo->name);
			ImGui::ToolTip("Delete");

			ImGui::SameLine();
			if (repo->isDisabled)
			{
				if (ImGui::ColoredButton(ICON_MD_CHECK_CIRCLE, ImVec4(0.20f, 0.95f, 0.20f, 0.69f), 2.0f))
				{
					auto src = g_YimPath / "scripts" / "disabled" / repo->name;
					auto dest = g_YimPath / "scripts" / repo->name;
					GitHubManager::MoveRepository(repo->name, src, dest);
				}
				ImGui::ToolTip("Enable");
			}
			else
			{
				if (ImGui::Button(ICON_MD_BLOCK))
				{
					auto src = g_YimPath / "scripts" / repo->name;
					auto dest = g_YimPath / "scripts" / "disabled" / repo->name;
					GitHubManager::MoveRepository(repo->name, src, dest);
				}
				ImGui::ToolTip("Disable");

				if (repo->isPendingUpdate)
				{
					ImGui::SameLine();
					if (ImGui::Button(ICON_MD_UPDATE))
						GitHubManager::DownloadRepository(repo->name);
					ImGui::ToolTip("Update");

					ImGui::SameLine();
					ImGui::PushFont(Fonts::Title);
					auto icon = ICON_MD_CLOUD_DOWNLOAD;
					float iconWidth = ImGui::CalcTextSize(icon).x;
					float regionWidth = ImGui::GetContentRegionAvail().x;
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + regionWidth - iconWidth);
					ImGui::TextColored(ImVec4(0.15, 0.35, 0.6, 1.0), icon);
					ImGui::PopFont();
					ImGui::ToolTip("Update Available!", Fonts::Regular, false);
				}
			}

			ImGui::SameLine();
			if (ImGui::Button(ICON_MD_FOLDER))
				IO::Open(repo->currentPath.string());
			ImGui::ToolTip("Open folder");
		}

		static inline void DrawRepoCard(const Repository* repo)
		{
			ImGui::BeginChild(repo->htmlUrl.c_str(), 
				ImVec2(0, 0), 
				ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY, 
				ImGuiWindowFlags_AlwaysUseWindowPadding 
				| ImGuiWindowFlags_NoScrollbar 
				| ImGuiWindowFlags_NoResize);

			std::string date = Utils::FormatDateShort(repo->lastUpdate);
			const char* desc = repo->description.empty() ? "No description." : repo->description.c_str();
			ImGuiStyle& style = ImGui::GetStyle();
			ImVec4 textCol = style.Colors[ImGuiCol_Text];
			const auto titleSize = ImGui::CalcTextSize(repo->name.c_str());
			const bool tileHovered = ImGui::IsMouseHoveringRect(ImGui::GetCursorScreenPos(),
			                             ImVec2(ImGui::GetCursorScreenPos().x + titleSize.x + 5.f,
			                                 ImGui::GetCursorScreenPos().y + titleSize.y + 5.f)) && !m_SortPopupOpen;

			if (tileHovered)
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

			if (tileHovered && ImGui::IsMouseClicked(0))
				IO::Open(repo->htmlUrl);

			ImGui::Spacing();
			ImGui::TextDisabled(ICON_MD_BOOK);
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.3f, 0.5f, 0.8f, 1.0f), repo->name.c_str());

			ImGui::PushFont(Fonts::Small);
			ImGui::Spacing();
			ImGui::BeginDisabled();
			ImGui::TextWrapped(repo->description.c_str());
			ImGui::EndDisabled();
			if (ImGui::CalcTextSize(repo->description.c_str()).y >= (ImGui::GetContentRegionAvail().y + 20))
				ImGui::ToolTip(repo->description.c_str());

			ImGui::Dummy(ImVec2(0, 10));

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.f, 0.f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 128, 255));
			ImGui::Text(ICON_MD_CIRCLE);
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::TextDisabled("Lua");
			ImGui::PopStyleVar();

			ImGui::SameLine();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2.f, 0.f));
			ImGui::TextDisabled(ICON_MD_STAR);
			ImGui::SameLine();
			ImGui::TextDisabled("%d", repo->stars);
			ImGui::PopStyleVar();

			float dateWidth = ImGui::CalcTextSize(date.c_str()).x;
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - dateWidth * 2);
			ImGui::TextDisabled("Updated on: %s", date.c_str());
			ImGui::Separator();

			ImGui::BeginDisabled(!std::filesystem::exists(g_YimPath / "scripts"));
			if (repo->isInstalled)
				DrawInstalledRepo(repo);
			else if (repo->isDownloading)
			{
				ImGui::Spinner(std::format("##{}", repo->htmlUrl).c_str(), 7.1f);
				ImGui::Spacing();
			}
			else
			{
				if (ImGui::Button(ICON_MD_DOWNLOAD))
					GitHubManager::DownloadRepository(repo->name);
				ImGui::ToolTip("Download");
			}
			ImGui::EndDisabled();
			ImGui::PopFont();
			ImGui::EndChild();
		}

		void Draw() override
		{
			auto sortmode = GitHubManager::GetSortMode();
			auto state = GitHubManager::GetState();

			if (ImGui::Button(ICON_MD_FILTER_LIST))
				ImGui::OpenPopup("##filterScripts");

			ImVec2 filterPopupPos = ImGui::GetItemRectMax();
			if (ImGui::IsPopupOpen("##filterScripts"))
				ImGui::SetNextWindowPos(ImVec2(filterPopupPos.x, filterPopupPos.y), ImGuiCond_Always);

			ImGui::SameLine();
			ImGui::Text("Filter");

			if (ImGui::BeginPopup("##filterScripts"))
			{
				m_SortPopupOpen = true;
				for (const auto& opt : SortOptions)
				{
					if (ImGui::MenuItem(opt.label, nullptr, (opt.mode == sortmode)))
					{
						GitHubManager::SetSortMode(opt.mode);
						ImGui::CloseCurrentPopup();
					}
				}
				ImGui::EndPopup();
			}
			else
				m_SortPopupOpen = false;

			ImGui::SameLine(0.f, 10.f);
			if (ImGui::Button(ICON_MD_REFRESH))
			{
				ThreadManager::Run([] {
					GitHubManager::FetchRepositories();
				});
			}
			ImGui::SameLine();
			ImGui::Text("Refresh");

			ImGui::SameLine(0.0f, 60.0f);
			ImGui::SetNextItemWidth(-1);
			ImGui::InputTextWithHint("##searchBar", ICON_MD_SEARCH, searchBuffer, sizeof(searchBuffer));

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
					if (searchBuffer[0] != '\0' && Utils::StringToLower(repos[i]->name).find(Utils::StringToLower(std::string(searchBuffer))) == std::string::npos)
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
		static inline const float m_RepoCardHeight = 190;
		static inline const ImVec2 m_CtrlBtnSize = ImVec2(120, 25);
		static inline bool m_SortPopupOpen = false;
	};

	inline YimLuaTab _YimLuaTab;
}
