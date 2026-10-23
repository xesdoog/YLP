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
	class AboutTab final : public GuiTab
	{
	public:
		AboutTab() :
		    GuiTab(eTabID::TAB_INFO, ICON_MS_INFO, "About")
		{
		}

		static inline void DrawHeaderAndText(const char* header, const char* text, std::initializer_list<const char*> bullets = {})
		{
			ImGui::PushFont(Fonts::Title);
			ImGui::Text(header);
			ImGui::PopFont();
			ImGui::Separator();
			ImGui::TextWrapped(text);
			if (bullets.size() > 0)
			{
				for (auto& c : bullets)
				{
					ImGui::Bullet();
					ImGui::SameLine();
					ImGui::TextWrapped(c);
				}
			}
			ImGui::Spacing();
		}

		void Draw() override
		{
			DrawHeaderAndText("About",
			    R"(
YLP (formerly YimLaunchpad) is a free and open-source companion application created by a member of the community, for the community.

The software and author are not affiliated, associated, or endorsed by any mod menu or third-party commercial tool.

YLP exists solely to enhance the player's experience through transparency and convenience.
)");

			ImGui::Dummy(ImVec2(0, 10));
			ImGui::InfoCallout(ImGui::ImCalloutType_Important,
			    R"(YLP is provided "as is", without any warranty of any kind, express or implied.
The author shall not be held liable for any damages, data loss, or issues arising from the use or misuse of this software.
)");
			////////////////////////////////////////////////////////

			ImGui::NewLine();
			ImGui::TitleText("Licenses and Acknowledgements", true);
			ImGui::TextWrapped("YLP makes use of several open-source libraries and assets that are licensed under their respective terms.");
			ImGui::Spacing();
			ImGui::TextWrapped("See");
			ImGui::SameLine();
			ImGui::TextLinkOpenURL("Third-Party " ICON_MS_OPEN_IN_NEW, "https://github.com/xesdoog/YLP/tree/main/docs/thirdparty");
			ImGui::SameLine();
			ImGui::TextWrapped("for details.");

			auto& ylpTag = YLPUpdater.GetLocalVersion().ToString();
			ImGui::NewLine();
			ImGui::TitleText("Useful Links", true);
			ImGui::Spacing();
			ImGui::TextLinkOpenURL("Changelog "          ICON_MS_OPEN_IN_NEW, std::format("https://github.com/xesdoog/YLP/releases/tag/{}", ylpTag).c_str());
			ImGui::TextLinkOpenURL("Source Code "        ICON_MS_OPEN_IN_NEW, "https://github.com/xesdoog/YLP");
			ImGui::TextLinkOpenURL("Report a Bug "       ICON_MS_OPEN_IN_NEW, "https://github.com/xesdoog/ylp/issues/new/choose");
			ImGui::TextLinkOpenURL("Start a Discussion " ICON_MS_OPEN_IN_NEW, "https://github.com/xesdoog/YLP/discussions/new/choose");
		}
	};

	inline AboutTab _AboutTab;
}
