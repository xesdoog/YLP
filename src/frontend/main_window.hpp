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
#include "../core/gui/fonts/fonts.hpp"
#include "../core/gui/msgbox.hpp"
#include "../core/YimMenu/yimmenu.hpp"
#include "../core/memory/pointers.hpp"
#include "../core/memory/procmon.hpp"
#include "../resources/logos/gtav.hpp"
#include "../resources/logos/gtave.hpp"
#include "../core/gui/widgets/ylp_spinner.hpp"


namespace YLP::Frontend
{
	/*
	This UI is still damn hideous. I hate designing UIs.
	I'll try to make it look better in the future.
	For now it does the job and that's it.
	*/
	class MainTab final : public GuiTab
	{
	public:
		MainTab() :
		    GuiTab(eTabID::TAB_MAIN, ICON_MD_HOME, "Home")
		{
		}

		void Draw() override
		{
			ImVec2 cursorPos = ImGui::GetCursorPos();
			ImVec2 childSize = ImGui::GetWindowSize();
			auto branchIdx = Config().mainWindowIndex;
			float tabButtonWidth = ImGui::CalcTextSize("Enhanced").x + 40.f + (ImGui::GetStyle().ItemSpacing.x * 2);
			float centerX = (childSize.x - (tabButtonWidth * 2)) * 0.5;

			ImGui::PushFont(Fonts::Title);
			ImGui::SegmentedControl("##branchSelector",
				&Config().mainWindowIndex, {"Legacy", "Enhanced"}, 
				ImGui::ImSegmentedControlAnchorPos::CENTER);
			ImGui::PopFont();

			if (branchIdx == 0)
				DrawLegacy();
			else if (branchIdx == 1)
				DrawEnhanced();
		}

	private:
		static inline void LaunchGame(int launcherIndex, YimMenu& menu, std::shared_ptr<ProcessMonitor>& monitor)
		{
			m_AttemptedGameLaunch = true;
			std::string cmd;
			std::string epicID;
			auto version = menu.m_Version;

			switch (launcherIndex)
			{
			case 0:
				cmd = (version == YimMenuV1) ? "steam://run/271590" : "steam://run/3240220";
				break;
			case 1:
			{
				constexpr const wchar_t* REG_SUBKEYS[] = {
				    L"SOFTWARE\\WOW6432Node\\Rockstar Games\\Grand Theft Auto V",
				    L"SOFTWARE\\WOW6432Node\\Rockstar Games\\Grand Theft Auto V Enhanced"};

				const wchar_t* regSubkey = REG_SUBKEYS[version == YimMenuV2];
				cmd = Utils::WideToUTF8(IO::ReadRegistryKey(HKEY_LOCAL_MACHINE, regSubkey, L"InstallFolder")) + "PlayGTAV.exe";
				break;
			}
			case 2:
				// Thanks to DeadlineEm for providing the Enhanced AppID
				epicID = (version == YimMenuV1) ? "9d2d0eb64d5c44529cece33fe2a46482" : "1af1b2c011864f1f9e432b0c64c6a1f5";
				cmd = "com.epicgames.launcher://apps/" + epicID + "?action=launch&silent=true";
				break;
			case 3:
			{
				if (!menu.m_ExePath.empty())
				{
					if (!IO::Exists(menu.m_ExePath))
					{
						std::string msg = "The previously saved GTA V executable path is no longer valid. Please select the executable again.";
						LOG_WARN(msg);
						Notifier::Add("YLP", msg, Notifier::Warning);
						Config().gtaExePaths.erase(menu.m_TargetProcess);
						menu.m_ExePath.clear();
						break;
					}
					else
					{
						cmd = menu.m_ExePath.string();
						break;
					}
				}

				const std::vector<COMDLG_FILTERSPEC> filters = {{L"Executable", L"*.exe"}};
				std::filesystem::path selected = IO::OpenFileDialog(filters, L"Select GTA V Executable");
				if (selected.empty())
					break;

				std::string filename = Utils::StringToLower(selected.filename().string());
				if (filename.find("gta") == std::string::npos)
				{
					LOG_ERROR("Selected file does not appear to be a GTA executable.");
					MsgBox::Error("Invalid file", "Selected file does not appear to be a GTA executable.");
					m_AttemptedGameLaunch = false;
					return;
				}

				cmd = selected.string();
				menu.m_ExePath = selected;
				Config().gtaExePaths[menu.m_TargetProcess] = selected;
				break;
			}
			default:
				LOG_ERROR("Unknown launcher index: {}", launcherIndex);
				m_AttemptedGameLaunch = false;
				return;
			}

			if (cmd.empty())
			{
				m_AttemptedGameLaunch = false;
				std::string err = "Failed to find an appropriate launch command/path. Please start the game manually.";
				LOG_ERROR(err);
				MsgBox::Error("Error", err.c_str());
				return;
			}

			IO::Open(cmd);
			monitor->WaitForProcessStart(6e4); // this is returns earlier than WaitForGameReady which should only be used to gate memory scans
			m_AttemptedGameLaunch = false;
		}

		static inline void DrawMenuDownload(YimMenu& menu)
		{
			if (menu || menu.GetState() == YimMenu::eMenuViewState::Downloading)
				return;

			ImGui::SameLine();
			if (ImGui::Button(ICON_MD_DOWNLOAD " Download", ButtonBig))
			{
				ThreadManager::Run([&menu] {
					menu.Download();
				});
			}
		}

		static inline void DrawMenuControls(YimMenu& menu)
		{
			auto state = menu.GetState();

			ImGui::BeginDisabled(state != YimMenu::eMenuViewState::Idle);
			const uint8_t monitorTarget = (menu.m_Version == YimMenuV1) ? MonitorLegacy : MonitorEnhanced;
			bool wantsAutoInject        = (Config().autoMonitorFlags & monitorTarget) != 0;
			bool wantsAutoUpdate        = (Config().menuAutoUpdateFlags & monitorTarget) != 0;
			if (ImGui::Checkbox("Auto-Inject", &wantsAutoInject))
			{
				Config().autoMonitorFlags ^= monitorTarget;
				SwitchMonitorMode();
			}

			if (ImGui::Checkbox("Auto-Update", &wantsAutoUpdate))
			{
				Config().menuAutoUpdateFlags ^= monitorTarget;
				SwitchMonitorMode();
			}
			ImGui::EndDisabled();
			ImGui::Spacing();

			switch (state)
			{
				case YimMenu::eMenuViewState::PendingUpdate:
				{
					if (ImGui::Button(ICON_MD_UPDATE))
					{
						ThreadManager::Run([&menu] {
							menu.Download();
						});
					}
					ImGui::ToolTip("Update");
					ImGui::SameLine();
					ImGui::Text("Update Available!");
					break;
				}
				case YimMenu::eMenuViewState::Checking:
					ImGui::YLPSpinner("Checking for updates...");
					break;
				case YimMenu::eMenuViewState::Idle:
				{
				    ImGui::BeginDisabled(wantsAutoUpdate);
				    if (ImGui::Button(ICON_MD_SYNC))
					{
						ThreadManager::Run([&menu] {
							menu.CheckForUpdates();
						});
					}
					ImGui::SameLine();
					ImGui::Text("Check For Updates");
				    ImGui::EndDisabled();
				    break;
				}
			    default: break;
			}

			float anchorPos = ImGui::GetContentRegionAvail().y - ButtonBig.y - (ImGui::GetStyle().FramePadding.y * 2);
			if (anchorPos > 0)
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + anchorPos);

			if (state == YimMenu::eMenuViewState::Downloading)
				ImGui::ProgressBar(menu.m_DownloadProgress, ButtonBig);
		}

		static inline void DrawInjectButton(YimMenu& menu, bool running, bool injected)
		{
			if (!menu || menu.GetState() == YimMenu::eMenuViewState::Downloading)
				return;

			ImGui::Spacing();
			const uint8_t monitorTarget = (menu.m_Version == YimMenuV1) ? MonitorLegacy : MonitorEnhanced;
			bool wantsAutoInject = (Config().autoMonitorFlags & monitorTarget) != 0;
			bool disabledCond = (wantsAutoInject || injected || !running);
			auto injectLabel = injected ? ICON_MD_CHECK_CIRCLE_OUTLINE " Injected" : ICON_MD_START " Inject";
			ImGui::BeginDisabled(disabledCond);
			if (ImGui::Button(injectLabel, ButtonBig))
			{
				ThreadManager::Run([&menu] {
					auto result = menu.Inject();
					if (!result.success)
					{
						LOG_ERROR("{}", result.message);
						MsgBox::Error("Error", result.message.c_str());
					}
				});
			}
			ImGui::EndDisabled();
			if (disabledCond)
				ImGui::ToolTip("Currently unavailable.\n\nMake sure the game is running, auto-inject is off, and the menu isn't already injected.");
		}

		static inline void DrawMenuUI(YimMenu& menu,
		    ImTextureID iconTexture,
		    std::shared_ptr<ProcessMonitor>& monitor,
		    GTAPointers pointers)
		{
			bool isRunning = monitor->IsProcessRunning();
			ImVec4 defaultTextCol = ImGui::GetStyle().Colors[ImGuiCol_Text];
			ImGui::Dummy(ImVec2(0, 30));
			ImGui::BeginChild("##logo", ImVec2(153, 165), 0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground);
			ImGui::Image(iconTexture, ImVec2(153, 135));
			ImGui::EndChild();

			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();

			ImGui::BeginChild("##game", ImVec2(0, 165), 0, ImGuiWindowFlags_NoBackground);
			const char* playBtnLabel = isRunning ? "Playing" : ICON_MD_PLAY_ARROW " Play";
			if (m_AttemptedGameLaunch)
				playBtnLabel = hourglassIcons[static_cast<int>(ImGui::GetTime() / 0.12f) & 3];

			ImGui::BeginDisabled(m_AttemptedGameLaunch || isRunning);
			if (ImGui::Button(playBtnLabel, ImVec2(144, 35)))
			{
				int launcherIdx = Config().launcherIndex;
				if (launcherIdx < 0)
					ImGui::OpenPopup("##launcherPopup");
				else
					ThreadManager::RunDetached([&menu, &monitor, &launcherIdx] {
						LaunchGame(launcherIdx, menu, monitor);
					});
			}
			ImGui::EndDisabled();

			ImGui::SameLine();
			if (ImGui::Button(ICON_MD_MORE_VERT, ImVec2(35, 35)))
				ImGui::OpenPopup("##launcherPopup");

			ImGui::SetNextWindowPos(ImGui::GetItemRectMax(), ImGuiCond_Always);
			if (ImGui::BeginPopup("##launcherPopup"))
			{
				ImGui::TextCentered("Select Launcher");
				ImGui::Separator();
				const size_t numLaunchers = m_Launchers.size();
				bool hasExePath           = !menu.m_ExePath.empty();
				int* launcherIdx          = &Config().launcherIndex;
				for (size_t i = 0; i < numLaunchers; i++)
				{
					const auto& launcher = m_Launchers[i];
					bool selected        = (*launcherIdx == i);

					ImGui::Selectable(launcher, selected);
					if (ImGui::IsItemClicked(0))
					{
						int v        = static_cast<int>(i);
						*launcherIdx = v;
						if (v == 3 && !hasExePath)
						{
							const std::vector<COMDLG_FILTERSPEC> filters = {{L"EXE (*.exe)", L"*.exe"}};
							std::filesystem::path selected               = IO::OpenFileDialog(filters, L"Select GTA V Executable");
							if (selected.empty())
								break;

							std::string filename = Utils::StringToLower(selected.filename().string());
							if (filename.find("gta") == std::string::npos)
							{
								LOG_ERROR("Selected file does not appear to be a GTA V executable.");
								MsgBox::Error("Invalid file", "Selected file does not appear to be a GTA V executable.");
								break;
							}
							menu.m_ExePath                             = selected;
							Config().gtaExePaths[menu.m_TargetProcess] = selected;
						}
					}
				}
				if (hasExePath)
				{
					ImGui::Dummy(ImVec2(0, 10));
					ImGui::Separator();
					ImGui::Spacing();
					ImGui::Text("Custom Executable Path:");
					ImGui::Spacing();
					if (ImGui::SelectableLabel(ICON_MD_CLEAR, false))
					{
						menu.m_ExePath.clear();
						Config().gtaExePaths.erase(menu.m_TargetProcess);
					}
					ImGui::SameLine();
					ImGui::BeginDisabled();
					ImGui::PushFont(Fonts::Small);
					ImGui::TextWrapped(menu.m_ExePath.string().c_str());
					ImGui::PopFont();
					ImGui::EndDisabled();
				}
				ImGui::EndPopup();
			}

			ImGui::Spacing();
			ImGui::PushFont(Fonts::Small);
			ImGui::DrawKeyValue("Status:", isRunning ? ICON_MD_CHECK_CIRCLE : ICON_MD_BLOCK, false, isRunning ? ImGreen : ImRed);


			int versionIndex = menu.m_Version == eYimVersion::YimMenuV1 ? 0 : 1;
			auto& versionStr = m_CachedVersions[versionIndex];
			if (versionStr.empty() && pointers.GameVersion && pointers.OnlineVersion)
			{
				auto gv = pointers.GameVersion ? pointers.GameVersion.Read<std::string>() : "";
				auto ov = pointers.OnlineVersion ? pointers.OnlineVersion.Read<std::string>() : "";
				m_CachedVersions[versionIndex] = std::format("{} (Online: {})", gv, ov);
			}

			ImGui::DrawKeyValue("Version:", versionStr);

			/*auto runtime = (isRunning && pointers.GameTime) ? pointers.GameTime.Read<int32_t>() : 0; // unnecessary read. who cares about playtime?
			if (runtime > 0)
				ImGui::DrawKeyValue("Play Time:", Utils::Int32ToTime(runtime / 1000));*/

			auto baseAddress = monitor->GetBaseAddress();
			ImGui::DrawKeyValue("Module Base:", std::format("0x{:X}", baseAddress), baseAddress != 0);

			bool heWatchin = monitor->IsHeWatchin();
			ImGui::DrawKeyValue("BattlEye:", heWatchin ? "Running!" : "Disabled", false, heWatchin ? ImRed : ImGreen);
			ImGui::PopFont();
			ImGui::EndChild();

			ImGui::Dummy(ImVec2(0, 20));
			ImGui::BeginChild("##menu", ImVec2(0, 0), 0, ImGuiWindowFlags_AlwaysUseWindowPadding);

			float menuChildWidth = std::min(420.0f, ImGui::GetContentRegionAvail().x * 0.5f);
			bool injected        = monitor->IsModuleLoaded(menu.m_DllName);

			ImGui::SetNextWindowBgAlpha(0.f);
			ImGui::BeginChild("##controls", ImVec2(menuChildWidth, 0), 0, ImGuiWindowFlags_AlwaysUseWindowPadding);
			DrawMenuControls(menu);

			float anchorPos = ImGui::GetContentRegionAvail().y - ButtonBig.y - (ImGui::GetStyle().FramePadding.y * 2);
			if (anchorPos > 0)
				ImGui::SetCursorPosY(ImGui::GetCursorPosY() + anchorPos);;
			DrawMenuDownload(menu);
			DrawInjectButton(menu, isRunning, injected);
			ImGui::EndChild();

			ImGui::SameLine();
			ImGui::SetNextWindowBgAlpha(0.f);
			ImGui::BeginChild("##stats", ImVec2(0, 0));
			ImGui::TextCentered("Stats");
			ImGui::Separator();
			ImGui::BeginGroup();
			ImGui::PushFont(ImGui::GetScaledFont());

			ImGui::DrawKeyValue(
				"Official GitHub Source: ",
				std::format("{} {}", menu.m_Name, ICON_MD_OPEN_IN_NEW),
				false,
				defaultTextCol,
			    ImGui::KVflagsHyperlink,
				menu.m_Url);

			std::string shortCommit = menu.m_LastCommitHash.substr(0, std::min<size_t>(7, menu.m_LastCommitHash.size()));

			ImGui::DrawKeyValue("Short SHA256 Checksum: ", menu.m_Checksum.empty() ? "Unknown" : menu.m_Checksum.substr(0, 8));
			ImGui::ToolTip(std::format("Full SHA: {}", menu.m_Checksum).c_str());

			ImGui::DrawKeyValue("Latest Release Commit:",
				shortCommit.empty() ? "Unknown" : shortCommit,
				false,
				defaultTextCol,
			    menu.m_LastCommitHash.empty() ? ImGui::KVflagsNone : ImGui::KVflagsHyperlink,
				std::format("{}/commit/{}", menu.m_Url, menu.m_LastCommitHash));

			if (!menu.m_LastCommitHash.empty())
				ImGui::ToolTip(std::format("Full commit hash: {}", menu.m_LastCommitHash).c_str(), Fonts::Small, true, 0);

			ImGui::DrawKeyValue("Injected:",
				injected ? ICON_MD_CHECK_CIRCLE : ICON_MD_BLOCK,
				false,
				injected ? ImGreen : defaultTextCol);

			ImGui::PopFont();
			ImGui::EndGroup();
			ImGui::EndChild();
			ImGui::EndChild();
		}

		static inline void DrawLegacy()
		{
			if (!IconGTAV) // this is redundant because renderer loads once anyway but it would bebetter to make it lazy-load instead. i'm too lazy to do it
				IconGTAV = Renderer::LoadTextureFromMemory(gta_legacy_data, gta_legacy_size, "IconGTAV");
			DrawMenuUI(g_YimV1, IconGTAV, g_ProcLegacy, g_Pointers.Legacy);
		}

		static inline void DrawEnhanced()
		{
			if (!IconGTAVE)
				IconGTAVE = Renderer::LoadTextureFromMemory(gta_enhanced_data, gta_enhanced_size, "IconGTAVE");

			DrawMenuUI(g_YimV2, IconGTAVE, g_ProcEnhanced, g_Pointers.Enhanced);
		}

		static inline void SwitchMonitorMode()
		{
			switch (Config().autoMonitorFlags)
			{
			case MonitorNone:
				g_ProcLegacy->Stop();
				g_ProcEnhanced->Stop();
				break;
			case MonitorLegacy:
				g_ProcLegacy->Start(100ms);
				g_ProcEnhanced->Stop();
				break;
			case MonitorEnhanced:
				g_ProcLegacy->Stop();
				g_ProcEnhanced->Start(100ms);
				break;
			case MonitorBoth:
				g_ProcLegacy->Start(100ms);
				g_ProcEnhanced->Start(100ms);
				break;
			default:
				break;
			}
		}

	private:
		static inline ImTextureID IconGTAV{0};
		static inline ImTextureID IconGTAVE{0};
		static inline ImVec4 ImRed{1, 0, 0, 1};
		static inline ImVec4 ImGreen{0, 1, 0, 1};
		static inline ImVec2 ButtonBig{-1, 37};
		static inline bool m_AttemptedGameLaunch{false};
		static inline std::array<std::string, 2> m_CachedVersions{"", ""};

		static inline std::array hourglassIcons{
			ICON_MD_HOURGLASS_EMPTY, 
			ICON_MD_HOURGLASS_TOP, 
			ICON_MD_HOURGLASS_EMPTY, 
			ICON_MD_HOURGLASS_BOTTOM
		};

		static inline std::array m_Launchers{
		    "Steam",
		    "Rockstar Games Launcher",
		    "Epic Games Launcher",
		    "Executable Path",
		};
	};

	inline MainTab _MainTab;
}
