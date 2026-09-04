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

#include "core/lua_scripting/lua_mgr.hpp"


namespace YLP::Frontend
{
	using namespace YLP::LuaJIT;

	class PluginsUI
	{
	public:
		PluginsUI() = default;
		~PluginsUI() noexcept = default;

		static void Draw()
		{
			if (!Config().enableScripting)
			{
				ImGui::TextCentered("Currently Unavailable", Fonts::Title);
				ImGui::Dummy(ImVec2(0, 20));
				ImGui::Text("The scripting feature is disabled. You can enable it in the General Settings tab.");
				return;
			}

			ImGui::InputTextMultiline("##codeExecutor", codeBuffer.data(), sizeof(codeBuffer), ImVec2(-1, ImGui::GetContentRegionAvail().y - 60.0f));
			ImGui::Separator();

			ImGui::BeginDisabled(codeBuffer[0] == '\0');
			if (ImGui::Button(ICON_MD_TERMINAL " Execute"))
			{
				ThreadManager::Run([&] {
					LuaManager::ExecuteCode(codeBuffer.data());
				});
			}

			ImGui::SameLine();
			if (ImGui::Button(ICON_MD_BACKSPACE " Clear"))
				codeBuffer.fill('\0');
			ImGui::EndDisabled();
		}
	private:
		static inline std::shared_ptr<LuaModule> selectedModule{nullptr};
		static inline std::string selectedDisabledModule{};
		static inline std::array<char, 64 * 1024> codeBuffer{};
	};
}
