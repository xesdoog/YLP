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


namespace YLP::Frontend
{
	using namespace PsUtils;

	class InjectorTab final : public GuiTab
	{
	public:
		InjectorTab() :
		    GuiTab(eTabID::TAB_INJECTOR, ICON_MD_STORAGE, "Standalone injector & custom DLLs")
		{
		}

		static inline void OnFileSelected(const DllInfo& file)
		{
			std::string lastKnown = file.lastKnownProcess;
			if (file.checksum == lastSelectedDLL || selectedProcess.m_Name == lastKnown)
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
					lastSelectedDLL = file.checksum;
					break;
				}
			}
		}

		void Draw() override
		{
			auto processes		= ProcessList::GetSnapshot();
			auto childRegion	= ImGui::GetContentRegionAvail();
			auto& savedDLLs		= Config().savedDlls;
			std::string preview = selectedProcess.m_Name.empty() ? "Process List" : selectedProcess.m_Name;

			ImGui::Spacing();
			ImGui::SetNextItemWidth(-1);
			if (ImGui::BeginCombo("##processList", std::format("{} {}", ICON_MD_MEMORY, preview).c_str(), ImGuiComboFlags_HeightLarge))
			{
				if (!initialized)
				{
					ProcessList::StartUpdating();
					initialized = true;
				}

				ImGui::SetNextItemWidth(-1);
				ImGui::InputTextWithHint("##SearchBox", ICON_MD_SEARCH, searchBuffer, sizeof(searchBuffer));
				ImGui::Separator();

				ImGui::BeginChild("##processList", ImVec2(0, 200), 0, ImGuiWindowFlags_AlwaysUseWindowPadding);
				ImGui::Spacing();
				for (int i = 0; i < processes.size(); ++i)
				{
					auto& p = processes[i];
					if (p.m_Name.empty())
						continue;

					auto nameLower = Utils::StringToLower(p.m_Name);
					if (searchBuffer[0] != '\0' && nameLower.find(Utils::StringToLower(searchBuffer)) == std::string::npos)
						continue;

					if (nameLower.find("system") != std::string::npos)
						continue;

					ImGui::PushID(p.m_Pid);
					ImGui::Selectable(p.m_Name.c_str(), p.m_Pid == selectedProcess.m_Pid);
					if (ImGui::IsItemClicked())
					{
						selectedProcess = p;
						ImGui::CloseCurrentPopup();
					}

					ImGui::PopID();
					ImGui::SameLine(ImGui::GetContentRegionAvail().x - 62.0f);
					ImGui::Text("[%u]", p.m_Pid);
				}
				ImGui::EndChild();
				ImGui::EndCombo();
			}
			else if (initialized)
			{
				ProcessList::StopUpdating();
				initialized = false;
			}

			ImGui::Dummy(ImVec2(0, 30));
			ImGui::BeginChild("##dllList", ImVec2(childRegion.x * 0.5, 0), ImGuiChildFlags_Borders);
			ImGui::Text(ICON_MD_LIST " Your Files");
			
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
			ImGui::Selectable(ICON_MD_ADD, false);
			ImGui::PopStyleVar();
			ImGui::ToolTip("Add a new file.");
			if (ImGui::IsItemClicked(0))
			{
				ThreadManager::RunDetached([&] {
					DllInfo newdll = PsUtils::AddDLL();
					if (newdll.filepath.empty())
						return;

					if (!newdll.ok && newdll.error != "Canceled by user")
					{
						LOG_ERROR("Failed to add DLL file: {}", newdll.error);
						return;
					}

					{
						std::scoped_lock lock(m_Mutex);
						std::erase_if(savedDLLs, [&](const DllInfo& d) {
							return d.filepath == newdll.filepath || d.checksum == newdll.checksum;
						});

						savedDLLs.push_back(newdll);
					}
				});
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			for (int i = 0; i < savedDLLs.size(); i++)
			{
				auto& dll = savedDLLs[i];
				if (dll.filepath.empty())
					continue;

				auto short_sha = dll.checksum.substr(0, 8);
				auto label = std::format("[{}] {}", short_sha, dll.filepath.filename().string());

				ImGui::PushFont(Fonts::Small);
				if (ImGui::ColoredButton(std::format("{}##{}", ICON_MD_DELETE, short_sha).c_str(), ImVec4(0.95f, 0.20f, 0.20f, 0.69f), 4.0f))
				{
					std::scoped_lock lock(m_Mutex);
					std::erase_if(savedDLLs, [&](auto& d) {
						if (d.filepath == dll.filepath)
						{
							if (dll.checksum == selectedDLL.checksum)
								selectedDLL = {};
							return true;
						}
						return false;
					});
				}
				float buttonWidth = ImGui::GetItemRectMax().x;
				ImGui::PopFont();
				ImGui::ToolTip("Delete");
				ImGui::SameLine();

				ImGui::BeginDisabled(dndTarget == dll.checksum);
				ImGui::Selectable(label.c_str(), dll.filepath == selectedDLL.filepath);
				ImGui::EndDisabled();
				if (dndTarget != dll.checksum)
					ImGui::ToolTip("Drag to reorder");

				if (ImGui::IsItemClicked(0))
				{
					selectedDLL = dll;
					OnFileSelected(dll);
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					dndTarget = dll.checksum;
					ImGui::SetDragDropPayload("DLLINFO_INDEX", &i, sizeof(int));
					ImGui::Selectable((label).c_str(), true);
					ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
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
							std::scoped_lock lock(m_Mutex);
							DllInfo cpy = savedDLLs[src];
							savedDLLs.erase(savedDLLs.begin() + src);
							savedDLLs.insert(savedDLLs.begin() + i, std::move(cpy));
						}
					}
					ImGui::EndDragDropTarget();
				}
			}
			ImGui::EndChild();

			ImGui::SameLine();
			ImGui::BeginChild("##dllInfo", ImVec2(0, 0), 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoBackground);
			if (!selectedDLL.name.empty())
			{
				ImGui::TextCentered(selectedDLL.name.c_str(), Fonts::Title);
				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();
				ImGui::PushFont(Fonts::Small);
				ImGui::PushTextWrapPos(ImGui::GetContentRegionAvail().x);
				ImGui::Bullet();
				ImGui::SameLine();
				ImGui::Text("Architecture: %s", selectedDLL.is64bit ? "x64" : "x86-64");

				ImGui::Bullet();
				ImGui::SameLine();
				std::string exportsText = selectedDLL.hasExports ? "Yes" : "No";
				ImGui::Text("Has Exports: %s", exportsText.c_str());

				ImGui::Bullet();
				ImGui::SameLine();
				ImGui::Text("File Path: %s", selectedDLL.filepath.string().c_str());
				
				ImGui::Bullet();
				ImGui::SameLine();
				ImGui::Text("SHA256 Hash: %s", selectedDLL.checksum.c_str());
				
				ImGui::Bullet();
				ImGui::SameLine();
				std::string lastProcessText = selectedDLL.lastKnownProcess.empty() ? "None" : selectedDLL.lastKnownProcess;
				ImGui::Text("Last Known Process: %s", lastProcessText.c_str());
				ImGui::PopTextWrapPos();
				ImGui::PopFont();

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();

				ImGui::SameLine();
				bool injectBtnDisabled = !selectedProcess.m_Pid;
				ImGui::BeginDisabled(injectBtnDisabled);
				if (ImGui::Button("Inject", ImVec2(-1, 40)))
				{
					const std::string processName = selectedProcess.m_Name;
					const DllInfo dll = selectedDLL;
					ThreadManager::RunDetached([dll, processName] {
						try
						{
							auto result = PsUtils::Inject(processName, dll.filepath);
							if (!result.success)
							{
								LOG_ERROR(result.message);
								MsgBox::Error("Error", result.message.c_str());
								return;
							}

							auto it = std::ranges::find_if(
							    Config().savedDlls,
							    [&](const DllInfo& d) {
								    return d.filepath == dll.filepath;
							    });

							if (it != Config().savedDlls.end())
								it->lastKnownProcess = processName;
						}
						catch (const std::exception& e)
						{
							LOG_ERROR("Failed to inject DLL! {}", e.what());
						}
					});
				}
				ImGui::EndDisabled();
				if (injectBtnDisabled)
					ImGui::ToolTip("Please choose a target process from the list above.");
			}
			ImGui::EndChild();
		}

	private:
		static inline ProcessEntry selectedProcess{};
		static inline DllInfo selectedDLL{};
		static inline bool initialized{false};
		static inline char searchBuffer[256];
		static inline std::mutex m_Mutex{};
		static inline std::string lastSelectedDLL{};
		static inline std::string dndTarget{};
	};

	inline InjectorTab _InjectorTab;
}
