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
#include "thirdparty/ImGuiColorTextEditor/TextEditor.h"


namespace YLP::Frontend
{
	using namespace YLP;
	using namespace LuaJIT;

	class ScriptingTab final : public GuiTab
	{
	public:
		ScriptingTab() :
		    GuiTab(eTabID::TAB_SCRIPTING, ICON_MS_CODE, "Scripting")
		{
		}

	private:
		void DrawCodeExecutor()
		{
			if (!m_EditorInitialized)
			{
				m_Editor.SetLanguage(TextEditor::Language::Lua());
				m_Editor.SetPalette(TextEditor::GetDarkPalette());
				m_Editor.SetTabSize(4);
				m_Editor.SetAutoIndentEnabled(true);
				m_Editor.SetCompletePairedGlyphs(true);
				m_Editor.SetWordWrapEnabled(true);
				m_Editor.SetImGuiContext(ImGui::GetCurrentContext());
				m_Editor.SetShowSpacesEnabled(false);
				m_EditorInitialized = true;
			}

			ImGuiWindowFlags defaultFlags = 2052;
			m_Editor.Render("##luaEditor", ImVec2(0.0f, ImGui::GetContentRegionAvail().y - 60.0f), 0, defaultFlags | ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::Dummy(ImVec2(0, 5));
			ImGui::Separator();

			ImGui::BeginDisabled(m_Editor.IsEmpty());
			if (ImGui::Button(ICON_MS_TERMINAL " Execute"))
				LuaManager::ExecuteCode(m_Editor.GetText().data());

			ImGui::SameLine();
			if (ImGui::Button(ICON_MS_BACKSPACE " Clear"))
				m_Editor.ClearText();
			ImGui::EndDisabled();
		}

		void DrawLuaTabs()
		{
			auto& modules = LuaManager::GetModules();
			if (modules.empty())
				ImGui::TextCentered("Nothing to see here.");
			else
			{
				ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 0.f);
				ImGui::BeginChild("##scriptLst", ImVec2(140.f, 0.f), ImGuiChildFlags_ResizeX, ImGuiWindowFlags_NoBackground);
				for (auto& m : LuaManager::GetModules())
				{
					if (m->GetLoadState() != LuaModule::RUNNING)
						continue;

					if (ImGui::Selectable(std::format("{} {}", ICON_MS_SDK, m->GetName()).c_str(), (m_SelectedModule && m == m_SelectedModule)))
						m_SelectedModule = m;

					ImGui::Separator();
				}
				ImGui::EndChild();

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2());
				ImGui::SameLine();
				ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 3.0f);
				ImGui::PopStyleVar(2);
				ImGui::SameLine();

				ImGui::SetNextWindowBgAlpha(0.0f);
				ImGui::BeginChild("##scritpCb");
				if (m_SelectedModule && m_SelectedModule->GetLoadState() == LuaModule::RUNNING)
				{
					auto& cb = m_SelectedModule->m_GuiCallback;
					if (!cb.valid())
						ImGui::TextWrapped("This module has no GUI to draw.");
					else
					{
						auto res = cb();
						if (!res.valid())
						{
							sol::error e = res;
							auto msg     = e.what();
							LOG_ERROR(msg);
							m_SelectedModule->SetAsBroken(msg);
						}
					}
				}
				ImGui::EndChild();
			}
		}

	public:
		void Draw() override
		{
			auto& cfg = Config();
			if (!cfg.enableScripting)
			{
				ImGui::TextCentered("Currently Unavailable", Fonts::Title);
				ImGui::Dummy(ImVec2(0, 20));
				ImGui::TextWrapped("The scripting feature is disabled. You can enable it in the Settings tab.");
				return;
			}

			ImGui::BeginChild("##segments", ImVec2(),
				ImGuiChildFlags_Borders
				| ImGuiChildFlags_AlwaysAutoResize
				| ImGuiChildFlags_AutoResizeY,
				ImGuiWindowFlags_NoScrollbar);

			int* scriptingWindowIndex = &cfg.scriptingWindowIndex;
			ImGui::PushFont(Fonts::Bold);
			ImGui::SegmentedControl(scriptingWindowIndex, {"Code Executor", "Script GUIs"}, ImGui::ImSegmentedCtrlPos_Center);
			ImGui::PopFont();
			ImGui::EndChild();
			ImGui::Spacing();

			switch (*scriptingWindowIndex)
			{
			case 0:
				DrawCodeExecutor();
				break;
			case 1:
				DrawLuaTabs();
				break;
			}
		}
	private:
		std::shared_ptr<LuaModule> m_SelectedModule;

		bool m_EditorInitialized{false};

		int m_SelectedDrawMode{0};

		TextEditor m_Editor;
	};

	inline ScriptingTab _ScriptingTab;
}
