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
#include "../core/gui/msgbox.hpp"
#include "../core/injector/injector.hpp"


namespace YLP::Frontend
{
	using namespace PsUtils;

	class InjectorTab final : public GuiTab
	{
	public:
		InjectorTab() :
		    GuiTab(eTabID::TAB_INJECTOR, ICON_MS_SYRINGE, "Injector")
		{
		}

	private:
		void DrawProcessList()
		{
			auto processes      = ProcessList::GetSnapshot();
			float lineHeight    = ImGui::GetTextLineHeight();
			std::string preview = selectedProcess.m_Name.empty() ? "None." : std::format("{}  [{}]", selectedProcess.m_Name, selectedProcess.m_Pid);

			ImGui::Spacing();
			ImGui::TextCentered("Target Process", Fonts::Title);
			if (!selectedProcess.m_Pid)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
				ImGui::TextCentered("Choose the process that will receive the DLL.", Fonts::Small);
				ImGui::PopStyleColor();
			}

			ImGui::Spacing();
			ImGui::BeginDisabled(!selectedProcess.m_Pid);
			if (ImGui::Button(ICON_MS_CLEAR))
				selectedProcess = {};
			ImGui::ToolTip("Clear");
			ImGui::EndDisabled();

			ImGui::SameLine();
			ImGui::SetNextItemWidth(-1);
			if (ImGui::BeginCombo("##processList", std::format("{} {}", ICON_MS_MEMORY, preview).c_str(), ImGuiComboFlags_HeightLarge))
			{
				if (!initialized)
				{
					ProcessList::StartUpdating();
					initialized = true;
				}

				ImGui::SetNextItemWidth(-1);
				ImGui::SearchBar("##processSearch", searchBuffer);
				ImGui::Separator();

				ImGui::BeginChild("##processList", ImVec2(0, 200), 0, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::Spacing();

				if (std::strncmp(searchBuffer, "ylp", 3) == 0)
					ImGui::TextWrapped("What are you doing, step-bro?");
				else
				{
					for (int i = 0; i < processes.size(); ++i)
					{
						auto& p    = processes[i];
						auto& name = p.m_Name;

						if (p.m_Pid == 0 || name.empty())
							continue;

						auto nameLower = Utils::StringToLower(name);
						if (nameLower == "ylp.exe")
							continue;

						if (searchBuffer[0] != '\0' && nameLower.find(searchBuffer) == std::string::npos)
							continue;

						if (const auto& tex = ProcessList::GetProcessIcon(p.m_ExePath); tex != NULL)
						{
							ImGui::Image(tex, ImVec2(lineHeight, lineHeight));
							ImGui::SameLine();
						}

						ImGui::PushID(p.m_Pid);
						ImGui::Selectable(name.c_str(), p.m_Pid == selectedProcess.m_Pid);
						if (ImGui::IsItemClicked())
						{
							selectedProcess = p;
							ImGui::CloseCurrentPopup();
						}

						ImGui::PopID();
						ImGui::SameLine(ImGui::GetContentRegionAvail().x - 62.0f);
						ImGui::Text("[%u]", p.m_Pid);
					}
				}

				ImGui::EndChild();
				ImGui::EndCombo();
			}
			else if (initialized)
			{
				ProcessList::StopUpdating();
				initialized = false;
			}
		}

		void OnFileSelected(const IO::DllInfo& file)
		{
			std::string lastKnown = file.m_LastKnownProcess;
			if (file.m_Checksum == lastSelectedDLL && selectedProcess.m_Name == lastKnown)
				return;

			if (lastKnown.empty())
				return;

			if (!initialized)
				ProcessList::UpdateProcesses();

			selectedProcess = {}; // is this even necessary? I know my goofy ass once injected YimLuaAPI into spotify because it was the last selected process
			for (auto& p : ProcessList::GetSnapshot())
			{
				if (p.m_Name == lastKnown)
				{
					selectedProcess = p;
					lastSelectedDLL = file.m_Checksum;
					break;
				}
			}
		}

		void AddNewFile(std::vector<IO::DllInfo>& savedDLLs)
		{
			ThreadManager::RunDetached([&]() {
				auto newdll = IO::AddDLL();
				if (newdll.m_Path.empty())
					return;

				if (!newdll.m_Ok && newdll.m_Error != "Canceled by user")
				{
					LOG_ERROR("Failed to add DLL file: {}", newdll.m_Error);
					return;
				}

				{
					std::scoped_lock lock(m_Mutex);
					if (auto it = std::find(savedDLLs.begin(), savedDLLs.end(), newdll); it != savedDLLs.end())
					{
						newdll.m_LastKnownProcess = it->m_LastKnownProcess;
						*it = newdll;
					}
					else
						savedDLLs.push_back(newdll);
				}
			});
		}

		void RefreshAllFiles(std::vector<IO::DllInfo>& savedDLLs)
		{
			if (isRefreshingList.exchange(true))
				return;

			isRefreshingList.store(true);

			ThreadManager::RunDetached([this, &savedDLLs]() {
				std::scoped_lock lock(m_Mutex);
				for (auto& dll : savedDLLs)
					ValidateDLL(dll);

				isRefreshingList.store(false);
			});
		}

		void OnChildClicked(std::vector<IO::DllInfo>& savedDLLs)
		{
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				selectedDLL = {};

			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				selectedDLL = {};
				ImGui::OpenPopup("##dllListCtx");
			}

			if (ImGui::BeginPopup("##dllListCtx"))
			{
				if (ImGui::MenuItem(ICON_MS_UPLOAD_FILE " Add New File"))
					AddNewFile(savedDLLs);

				ImGui::Separator();
				if (ImGui::MenuItem(ICON_MS_REFRESH " Refresh"))
					RefreshAllFiles(savedDLLs);

				ImGui::Separator();
				ImGui::PushStyleColor(ImGuiCol_Text, ImRed);
				if (ImGui::MenuItem(ICON_MS_DELETE " Remove All"))
				{
					if (MsgBox::Confirm("Remove All", "Are you sure you'd like to remove all files?"))
					{
						std::scoped_lock lock(m_Mutex);
						savedDLLs.clear();
					}
				}
				ImGui::PopStyleColor();
				ImGui::EndPopup();
			}
		}

		void DrawDllList(const ImVec2& regionSize, const float& injectorChildW, Settings::Config& cfg, ImGuiStyle& style)
		{
			auto& savedDLLs        = cfg.savedDlls;
			const float childWidth = regionSize.x - injectorChildW - style.ItemSpacing.x;
			bool isHoveringAfile   = false;

			ImGui::BeginDisabled(isRefreshingList);
			ImGui::BeginChild("##dllList", ImVec2(childWidth, 0), ImGuiChildFlags_Borders);
			ImGui::TitleText(ICON_MS_LIST " Library");

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - ImGui::GetFrameWidth());
			if (ImGui::Button(ICON_MS_UPLOAD_FILE "##addDLL"))
				AddNewFile(savedDLLs);
			ImGui::ToolTip("Add a new file.");

			ImGui::Separator();
			ImGui::Spacing();

			ImGui::BeginChild("##dllListScroll", ImVec2(), 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysUseWindowPadding);
			for (int i = 0; i < savedDLLs.size(); i++)
			{
				auto& dll = savedDLLs[i];
				if (dll.m_Path.empty())
					continue;

				auto label    = dll.m_Path.filename().string();
				bool hasError = !dll.m_Ok;

				if (hasError)
				{
					ImGui::PushStyleColor(ImGuiCol_Text, ImRed);
					ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImRedDisabled);
					label += ("  " ICON_MS_EXCLAMATION);
				}

				ImGui::PushID(&dll.m_Checksum);
				ImGui::BeginDisabled(dndTarget == dll.m_Checksum);
				ImGui::Selectable(label.c_str(), dll.m_Path == selectedDLL.m_Path);
				isHoveringAfile = isHoveringAfile || ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled);
				ImGui::EndDisabled();
				ImGui::PopID();

				if (hasError)
				{
					ImGui::PopStyleColor(2);
					ImGui::ToolTip(dll.m_Error.empty() ? "This file has an error. Consider removing or re-uploading it." : dll.m_Error.c_str());
				}
				else if (dndTarget != dll.m_Checksum)
					ImGui::ToolTip("Drag to reorder or right click to open the context menu.");

				if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
				{
					selectedDLL = dll;
					OnFileSelected(dll);
				}

				if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
				{
					selectedDLL = dll;
					OnFileSelected(dll);
					ImGui::OpenPopup(dll.m_Checksum.c_str());
				}

				if (ImGui::BeginPopup(dll.m_Checksum.c_str()))
				{
					if (ImGui::MenuItem(ICON_MS_DELETE " Remove"))
					{
						std::scoped_lock lock(m_Mutex);
						std::erase_if(savedDLLs, [&](auto& d) {
							if (d.m_Path == dll.m_Path)
							{
								if (dll.m_Checksum == selectedDLL.m_Checksum)
									selectedDLL = {};
								return true;
							}
							return false;
						});
					}

					ImGui::Separator();

					if (ImGui::MenuItem(ICON_MS_INFO " Properties"))
						shouldDrawDllInfo = true;

					ImGui::EndPopup();
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					dndTarget = dll.m_Checksum;
					ImGui::SetDragDropPayload("DLLINFO_INDEX", &i, sizeof(int));
					ImGui::Selectable((label).c_str(), true);
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
					ImGui::EndDragDropSource();
				}
				else
					dndTarget = "";

				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DLLINFO_INDEX"))
					{
						if (const int src = *(const int*)payload->Data; src != i)
						{
							IO::DllInfo cpy = savedDLLs[src];
							std::scoped_lock lock(m_Mutex);
							savedDLLs.erase(savedDLLs.begin() + src);
							savedDLLs.insert(savedDLLs.begin() + i, std::move(cpy));
						}
					}
					ImGui::EndDragDropTarget();
				}

				ImGui::Separator();
			}
			ImGui::EndChild(); // ##dllListScroll
			if (!isHoveringAfile)
				OnChildClicked(savedDLLs);

			ImGui::EndChild(); // ##dllList
			if (!isHoveringAfile)
				OnChildClicked(savedDLLs);

			ImGui::EndDisabled();
		}

		void DrawInjector(const float& injectorChildW, Settings::Config& cfg, ImGuiStyle& style)
		{
			ImVec2 injectBtnSize = ImVec2(-1, 50.0f);
			auto& injectorCfg    = cfg.injectorConfig;

			ImGui::SetNextWindowBgAlpha(0.0f);
			ImGui::BeginChild("##injectorChild", ImVec2(injectorChildW, 0));
			ImGui::TitleText("Injector", true);
			ImGui::Spacing();
			ImGui::Text("Mode:");
			ImGui::SetNextItemWidth(injectBtnSize.x);
			ImGui::Combo("##Mode", &injectorCfg.m_Mode, "Standard\0Manual Mapping\0");

			if (injectorCfg.m_Mode == 1)
			{
				float childH = -injectBtnSize.y - (style.ItemSpacing.y * 2);
				ImGui::BeginChild("##injectorCfgScroll", ImVec2(0, childH), 0, ImGuiWindowFlags_NoBackground);
				ImGui::PushFont(Fonts::Small);
				ImGui::Checkbox("Erase PE Headers", &injectorCfg.m_WipePE);
				ImGui::HelpMarker("Remove PE signatures from memory.");

				ImGui::Checkbox("Enable SEH", &injectorCfg.m_EnableSEH);
				ImGui::HelpMarker("Enable 'Structured Exception Handling' in injected DLL. Click this help icon to learn more.");
				if (ImGui::IsItemHovered())
				{
					ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
					if (ImGui::IsItemClicked(0))
						IO::Open("https://learn.microsoft.com/en-us/cpp/cpp/structured-exception-handling-c-cpp?view=msvc-170");
				}

				ImGui::Checkbox("Randomize Base Address", &injectorCfg.m_RandomizeAddress);
				ImGui::HelpMarker("Assign a random base address. Injector will fall back to Windows default-chosen address if all randomization attempts fail.");

				if (injectorCfg.m_RandomizeAddress)
				{
					ImGui::Indent();
					ImGui::Text("Max Randomization Attempts");
					ImGui::HelpMarker("Number of times to retry failed randomization attempts before giving up.");
					ImGui::SetNextItemWidth(-1);
					ImGui::SliderInt("##attempts", &injectorCfg.m_MaxRandomizationAttempts, 1, 5);
					ImGui::Unindent();
				}
				ImGui::PopFont();
				ImGui::EndChild();
			}

			ImGui::BeginDisabled(!selectedProcess.m_Pid || !selectedDLL.m_Ok);
			if (ImGui::Button(ICON_MS_SYRINGE " Inject", injectBtnSize))
			{
				ThreadManager::RunDetached([&]() {
					try
					{
						auto result = Injector::Inject(selectedProcess.m_Name, selectedDLL.m_Path, cfg.injectorConfig);
						if (!result.m_Success)
						{
							LOG_ERROR(result.m_Message);
							MsgBox::Error("Error", result.m_Message.c_str());
							return;
						}

						if (auto it = std::find(cfg.savedDlls.begin(), cfg.savedDlls.end(), selectedDLL); it != cfg.savedDlls.end())
							it->m_LastKnownProcess = selectedProcess.m_Name;
					}
					catch (const std::exception& e)
					{
						LOG_ERROR("Failed to inject DLL! {}", e.what());
					}
				});
			}
			ImGui::EndDisabled();
			if (!selectedProcess.m_Pid)
				ImGui::ToolTip("Please choose a target process from the list above.");
			else if (!selectedDLL.m_Ok)
				ImGui::ToolTip(selectedDLL.m_Error.empty() ? "Please select a valid file from your library." : selectedDLL.m_Error.c_str());

			ImGui::EndChild();

			if (shouldDrawDllInfo)
			{
				shouldDrawDllInfo = false;
				ImGui::OpenPopup("##dllInfoModal");
			}

			ImVec2 mainWindowSize = ImGui::GetWindowSize();
			ImVec2 center         = ImGui::GetMainViewport()->GetCenter();
			float popupWidth      = std::min(mainWindowSize.x * 0.8, 600.0);

			ImGui::SetNextWindowSizeConstraints(ImVec2(popupWidth, 200), ImVec2(popupWidth, 800));
			ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGuiWindowFlags modalFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;
			if (ImGui::BeginPopupModal("##dllInfoModal", nullptr, modalFlags))
			{
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 25.0f);
				if (ImGui::SelectableLabel(ICON_MS_CLEAR))
					ImGui::CloseCurrentPopup();

				ImGui::TextCentered(selectedDLL.m_Name.c_str(), Fonts::Bold);
				ImGui::Spacing();

				std::string lastProcessText = selectedDLL.m_LastKnownProcess.empty() ? "None" : selectedDLL.m_LastKnownProcess;
				ImGuiTableFlags tableFlags  = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInner | ImGuiTableFlags_RowBg;
				if (ImGui::BeginTable("##fileInfo", 2, tableFlags))
				{
					ImGui::TableSetupColumn("#k", ImGuiTableColumnFlags_WidthFixed);
					ImGui::TableSetupColumn("#v", ImGuiTableColumnFlags_WidthStretch);
					ImGui::PushFont(ImGui::GetScaledFont());

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Architecture");
					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(selectedDLL.m_Is64bit ? "x86-64" : "x86");

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Has Exports");
					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(selectedDLL.m_HasExports ? "Yes" : "No");

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("File Path");
					ImGui::SameLine();
					if (ImGui::SelectableLabel(ICON_MS_OPEN_IN_NEW))
						IO::HighlightFile(selectedDLL.m_Path.string());

					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(selectedDLL.m_Path.string().c_str());

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("SHA256 Hash");
					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(selectedDLL.m_Checksum.c_str());

					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Last Known Process");
					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(lastProcessText.c_str());

					auto& fileErr = selectedDLL.m_Error;
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Issue(s):");
					ImGui::TableSetColumnIndex(1);
					ImGui::TextWrapped(fileErr.empty() ? "None." : fileErr.c_str());

					ImGui::PopFont();
					ImGui::EndTable();
				}
				ImGui::EndPopup();
			}
		}

	public:
		void Draw() override
		{
			auto& cfg            = Config();
			auto& style          = ImGui::GetStyle();
			auto childRegion     = ImGui::GetContentRegionAvail();
			float injectorChildW = std::min(childRegion.x * 0.5, 400.0);

			DrawProcessList();
			ImGui::Dummy(ImVec2(0, 15));

			DrawDllList(childRegion, injectorChildW, cfg, style);

			ImGui::SameLine();
			DrawInjector(injectorChildW, cfg, style);
		}

	private:
		ProcessEntry selectedProcess{};

		IO::DllInfo selectedDLL{};

		bool initialized{false};
		bool shouldDrawDllInfo{false};

		std::atomic<bool> isRefreshingList{false};

		int selectedInjectorMode{0};

		char searchBuffer[64]{};

		std::mutex m_Mutex{};

		std::string lastSelectedDLL{};
		std::string dndTarget{};

		ImVec4 ImRed         = ImVec4(0.9, 0, 0, 1.0);
		ImVec4 ImRedDisabled = ImVec4(0.9, 0, 0, 0.5);
	};

	inline InjectorTab _InjectorTab;
}
