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

#include "core/gui/gui_tab.hpp"
#include "core/lua_scripting/lua_mgr.hpp"
#include "core/gui/theme_mgr.hpp"
#include "core/gui/window_bg_state.hpp"


namespace YLP::Frontend
{
	using namespace YLP;
	using namespace LuaJIT;

	class SettingsTab final : public GuiTab
	{
	public:
		SettingsTab() :
		    GuiTab(eTabID::TAB_SETTINGS, ICON_MS_SETTINGS, "Settings")
		{
		}

	private:
		void DrawGeneral(Settings::Config& cfg)
		{
			auto updateState = YLPUpdater.GetState();
			ImGui::BeginDisabled(updateState == Updater::UpdateState::Error);
			switch (updateState)
			{
			case Updater::UpdateState::Idle:
			{
				if (ImGui::Button(ICON_MS_SYNC))
					YLPUpdater.Check();
				ImGui::SameLine();
				ImGui::Text("Check For Updates");
				break;
			}
			case Updater::UpdateState::Checking:
				ImGui::Spinner("Please Wait...");
				break;
			case Updater::UpdateState::Pending:
			{
				if (ImGui::Button(ICON_MS_DOWNLOAD))
					YLPUpdater.Download();
				ImGui::ToolTip("Update");
				ImGui::SameLine();
				ImGui::Text("A new version of YLP is out!");
				break;
			}
			case Updater::UpdateState::Downloading:
			{
				ImGui::ProgressBar(YLPUpdater.GetProgress(), ImVec2(160, 25));
				ImGui::SameLine();
				ImGui::Text("Downloading...");
				break;
			}
			}
			ImGui::EndDisabled();

			ImGui::Spacing();
			ImGui::Checkbox("Internal Debug Console", &cfg.internalConsole);
			ImGui::HelpMarker("Toggle the internal debug console at the bottom of the UI.");

			if (ImGui::Checkbox("External Debug Console", &cfg.externalConsole))
				Logger::ToggleExternalConsole(cfg.externalConsole);
			ImGui::HelpMarker("Toggle the external debug console.");

			ImGui::Checkbox("Restore Last Tab", &cfg.restoreLastTab);
			ImGui::HelpMarker("Your last selected tab will be restored when the program starts.");

			ImGui::Checkbox("Disable Tooltips", &cfg.disableTooltips);
			ImGui::HelpMarker("Disables... Well, this.");

			ImGui::BeginDisabled(cfg.autoMonitorFlags == MonitorNone);
			ImGui::Checkbox("Auto-Exit", &cfg.autoExit);
			ImGui::HelpMarker("Automatically exit after injecting a dll. This only works if Auto-Inject is enabled for either YimMenu Legacy or V2 or both; does nothing otherwise.");
			ImGui::EndDisabled();
		}

		void DrawUISettings(Settings::Config& cfg)
		{
			ImGui::Text(ICON_MS_OPACITY " Global Opacity");
			ImGui::HelpMarker("Overrides window opacity across the entire application. When selecting a window blur or acrylic effect, this must be lowered in order to see those effects.");
			ImGui::SliderFloat("##alphaoverride", &cfg.bgAlphaMultiplier, 0.0f, 1.0f, "%.2f");

			ImGui::Spacing();
			ImGui::Text(ICON_MS_BACKGROUND_REPLACE " Window Background Effect");
			ImGui::HelpMarker("Set or remove a background effect from the main window. You must lower the 'Global Opacity' setting in order to see changes.");
			if (ImGui::BeginCombo("##windowEffects", accentStates[cfg.windowAccentState]))
			{
				for (int i = ACCENT_DISABLED; i < ACCENT_ENABLE_HOSTBACKDROP; i++)
				{
					ImGui::Selectable(accentStates[i], (i == cfg.windowAccentState));
					if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
					{
						cfg.windowAccentState = i;
						SetBackgroundAccentState(g_Hwnd, static_cast<eWindowAccentState>(i));
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Spacing();
			ImGui::SeparatorText(ICON_MS_PALETTE " Themes");
			ImVec2 previewSize(200, 220);
			Theme* currentTheme      = ThemeManager::GetCurrentTheme();
			auto& themes             = ThemeManager::GetThemes();
			std::string_view preview = currentTheme ? currentTheme->m_Name : "";
			for (auto& [name, theme] : themes)
			{
				if (ImGui::ThemePreview(theme, currentTheme == &theme, previewSize))
					ThemeManager::ApplyTheme(name);
				if (!theme.m_AuthorName.empty())
					ImGui::ToolTip(std::format("Theme by {}", theme.m_AuthorName).c_str());

				ImGui::SameLineIfAvail(previewSize.x);
			}
		}

		void DrawPlugins(Settings::Config& cfg)
		{
			if (ImGui::Checkbox("Enable Lua Scripting", &cfg.enableScripting))
			{
				if (!cfg.enableScripting)
				{
					LuaManager::Destroy();
				}
				else if (cfg.enableScripting = MsgBox::Confirm("YLP", "This will allow YLP to run Lua scripts. Are you sure you would like to enable this feature?"); cfg.enableScripting)
				{
					Notifier::Add("Scripting",
					    "Warning! This feature can be harmful if not handled properly. Please make sure to only execute Lua code from trusted sources.",
					    Notifier::Warning);
					LuaManager::Init(g_ProjectPath);
				}
			}

			if (!cfg.enableScripting)
				return;

			ImGui::Checkbox("Auto-Reload Modules", &cfg.autoReloadLuaModules);
			ImGui::HelpMarker("Automatically reload a Lua module when changes are detected in its root folder.");

			ImGui::Dummy(ImVec2(0, 5));

			auto& modules         = LuaManager::GetModules();
			auto& disabledModules = LuaManager::GetDisabledModules();
			ImVec2 region         = ImGui::GetContentRegionAvail();
			ImGui::BeginChild("##enabledPlugins", ImVec2(region.x * 0.5, 0), ImGuiChildFlags_Borders);
			if (ImGui::SmallButton(ICON_MS_REFRESH))
				LuaManager::ReloadAllModules();
			ImGui::ToolTip("Reload All");

			ImGui::SameLine();
			ImGui::TextCentered("Enabled Plugins");
			ImGui::Separator();

			if (modules.empty())
				ImGui::TextDisabled("Wow! Such Empty!");
			else
			{
				for (auto& m : modules)
				{
					auto pathName = m->GetRoot().string();

					ImGui::PushID(&m);
					ImGui::BeginDisabled(dragSourcePtr && m == dragSourcePtr);
					bool isBroken = m->GetLoadState() == LuaModule::BROKEN;
					auto icon     = isBroken ? ICON_MS_RUNNING_WITH_ERRORS : ICON_MS_SDK;

					if (isBroken)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9, 0, 0, 1.0));
						ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImVec4(0.9, 0, 0, 0.5));
					}
					ImGui::Selectable(std::format("{} {}", icon, m->GetName()).c_str(), (selectedModule && m == selectedModule));

					if (isBroken)
					{
						ImGui::PopStyleColor(2);
						std::string lastErr = m->GetErrorMsg();
						ImGui::ToolTip(std::format("Unloaded due to a malfunction: '{}'", lastErr.empty() ? "Unknown error." : lastErr).c_str());
					}

					ImGui::EndDisabled();
					ImGui::PopID();
					ImGui::ToolTip("Right click to open the context menu or drag to move to the disabled list.");

					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
					{
						selectedModule = m;
						ImGui::OpenPopup(pathName.c_str());
					}

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						dragSourcePtr = m;
						ImGui::SetDragDropPayload("ENABLED_MODULE", &m, sizeof(m));
						ImGui::Selectable(m->GetName().data(), true);
						ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
						ImGui::EndDragDropSource();
					}
					else
						dragSourcePtr = nullptr;

					if (ImGui::BeginPopup(pathName.c_str()))
					{
						if (ImGui::MenuItem(ICON_MS_REFRESH " Reload"))
							m->Reload();

						if (ImGui::MenuItem(ICON_MS_TOGGLE_ON " Disable"))
							m->Disable();

						ImGui::EndPopup();
					}
				}
			}
			ImGui::EndChild();

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DISABLED_MODULE"))
				{
					const char* droppedPath = static_cast<const char*>(payload->Data);
					LuaManager::EnableModule(droppedPath);
					dragSourceStr = "";
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine();
			ImGui::BeginChild("##disabledPlugins", ImVec2(0, 0), ImGuiChildFlags_Borders);
			ImGui::TextCentered("Disabled Plugins");
			ImGui::Separator();
			ImGui::SetNextWindowBgAlpha(0.0f);
			if (disabledModules.empty())
				ImGui::TextDisabled("Wow! Such Empty!");
			else
			{
				for (auto& m : disabledModules)
				{
					auto pathName = m.m_Path.string();

					ImGui::PushID(&m);
					ImGui::BeginDisabled(pathName == dragSourceStr);
					if (ImGui::Selectable(m.m_Name.c_str(), pathName == selectedDisabledPath))
						selectedDisabledPath = pathName;
					ImGui::EndDisabled();
					ImGui::PopID();
					ImGui::ToolTip("Right click to open the context menu or drag to move to the enabled modules list.");

					if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
					{
						selectedDisabledPath = pathName;
						ImGui::OpenPopup(pathName.c_str());
					}

					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
					{
						dragSourceStr = pathName;
						ImGui::SetDragDropPayload("DISABLED_MODULE", pathName.c_str(), pathName.size() + 1);
						ImGui::Selectable(m.m_Name.c_str(), true);
						ImGui::EndDragDropSource();
					}
					else
						dragSourceStr = "";

					if (ImGui::BeginPopup(pathName.c_str()))
					{
						if (ImGui::MenuItem(ICON_MS_TOGGLE_OFF " Enable"))
							LuaManager::EnableModule(pathName);

						ImGui::EndPopup();
					}
				}
			}
			ImGui::EndChild();
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENABLED_MODULE"))
				{
					LuaJIT::LuaModule* droppedModule = *static_cast<LuaJIT::LuaModule**>(payload->Data);
					droppedModule->Disable();
					dragSourcePtr = nullptr;
				}
				ImGui::EndDragDropTarget();
			}
		}

	public:
		void Draw() override
		{
			auto& style = ImGui::GetStyle();
			float totalWidth = 0.0f;
			for (int i = 0; i < tabCount; i++)
				totalWidth += ImGui::CalcTextSize(tabs[i]).x + style.FramePadding.x * 2.0f;

			totalWidth += style.ItemSpacing.x * (tabCount - 1);

			float regionWidth = ImGui::GetContentRegionAvail().x;
			float startX	  = (regionWidth - totalWidth) * 0.5f;

			ImGui::SetCursorPosX(startX > 0.0f ? ImGui::GetCursorPosX() + startX : 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_TabBarBorderSize, 0.0f);
			if (ImGui::BeginTabBar("##SettingsTabBar"));
			{
				auto& cfg = Config();

				if (ImGui::BeginTabItem(tabs[0]))
				{
					ImGui::Separator();
					ImGui::Dummy(ImVec2(0, 10));
					ImGui::BeginChild(tabs[1], ImVec2(0, 0), 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoBackground);
					DrawGeneral(cfg);
					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(tabs[1]))
				{
					ImGui::Separator();
					ImGui::Dummy(ImVec2(0, 10));
					ImGui::BeginChild(tabs[1], ImVec2(0, 0), 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoBackground);
					DrawUISettings(cfg);
					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem(tabs[2]))
				{
					ImGui::Separator();
					ImGui::Dummy(ImVec2(0, 10));
					ImGui::BeginChild(tabs[2], ImVec2(0, 0), 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoBackground);
					DrawPlugins(cfg);
					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				ImGui::EndTabBar();
			}
			ImGui::PopStyleVar();
		}

	private:
		std::shared_ptr<LuaJIT::LuaModule> selectedModule{nullptr};
		std::shared_ptr<LuaJIT::LuaModule> dragSourcePtr{nullptr};

		std::string selectedDisabledPath{};
		std::string dragSourceStr{};

		const int tabCount = 3;

		std::array<const char*, 3> tabs {
			ICON_MS_TUNE			 " General",
			ICON_MS_DISPLAY_SETTINGS " User Interface",
			ICON_MS_CODE			 " Scripting"
		};

		std::array<const char*, 6> accentStates {
		    "None",
		    "Gradient",
		    "Transparent Gradient",
		    "Blur",
		    "Acrylic Blur",
		    "Host Backdrop", // does jack shit. will exclude it in the loop
		};
	};

	inline SettingsTab _SettingsTab;
}
