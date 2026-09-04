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


#include "notifier.hpp"


namespace YLP
{
	void Notifier::AddToastImpl(const std::shared_ptr<Notification>& notif)
	{
		if (m_IsSnoozed)
			return;

		auto toast = std::make_shared<Toast>();
		toast->Bind(notif);
		m_Toasts.push_back(toast);
	}

	void Notifier::AddImpl(const std::string& title,
	    const std::string& message,
	    eNotificationLevel level,
	    NotificationCallback callback)
	{
		std::scoped_lock lock(m_Mutex);

		auto now = std::chrono::system_clock::now();
		std::time_t time = std::chrono::system_clock::to_time_t(now);
		std::tm local{};
		localtime_s(&local, &time);
		char buffer[32];
		std::strftime(buffer, sizeof(buffer), "%H:%M", &local);
		std::string header = std::format("{}\t{}", title, buffer);
		std::string ChildID = std::format("##{}{}", static_cast<int>(m_Notifications.size() + 1), title);

		const char* icon{};
		ImVec4 color{};
		GetStyle(level, icon, color);
		auto notif = std::make_shared<Notification>(
		    Notification{
		        header,
		        message,
		        ChildID,
		        now,
		        level,
		        callback,
		        false,
		        icon,
		        color});

		m_Notifications.push_back(notif);
		AddToast(notif);
		PlaySoundQueue();
		m_Viewed = false;
	}

	void Notifier::ClearReadImpl()
	{
		std::scoped_lock lock(m_Mutex);
		std::erase_if(m_Notifications, [](std::shared_ptr<Notification> n) {
			return n->m_Read;
		});
		m_Viewed = true;
	}

	void Notifier::Flush()
	{
		if (!m_ShouldFlush)
			return;

		if (m_Notifications.empty())
		{
			m_ShouldFlush = false;
			return;
		}

		float deltaTime = ImGui::GetIO().DeltaTime;
		for (auto& n : m_Notifications)
		{
			auto now = std::chrono::system_clock::now();
			if (now - m_LastFlushed < 33ms)
				continue;

			n->m_GlobalAlpha -= deltaTime * 1.20f;
			if (n->m_GlobalAlpha <= 0.1f)
			{
				n->Dismiss();
				m_LastFlushed = now;
			}
		}
	}

	void Notifier::PlaySoundQueue()
	{
		if (IsMuted() || m_IsSnoozed)
			return;

		auto now = std::chrono::steady_clock::now();
		if (now - m_LastAudioQueueTime < 5s)
			return;

		if (PlaySound(reinterpret_cast<const char*>(notif_audio_data), nullptr, SND_MEMORY | SND_ASYNC))
			m_LastAudioQueueTime = now;
	}

	float Notifier::ComputeTotalHeight()
	{
		float total = 80.0f;
		for (auto& n : GetInstance().m_Notifications)
		{
			if (n->m_Read)
				continue;

			total += n->ComputeHeight();
			total += 10.0f;
		}
		return total + 20.f + ImGui::GetStyle().ItemSpacing.y;
	}

	void Notifier::DrawImpl()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 parentWindowSize = ImGui::GetWindowSize();
		ImVec2 parentWindowPos = ImGui::GetWindowPos();
		float maxPopupHeight = parentWindowSize.y * 0.6f;
		ImVec2 popupSize(std::min(parentWindowSize.x * 0.6f, 440.0f), 0.0f);
		ImVec2 popupPos(parentWindowPos.x + parentWindowSize.x - popupSize.x - style.WindowPadding.x,
		    ImGui::GetCursorPosY() + 11.0f);
		float contentHeight = ComputeTotalHeight();
		float popupHeight = std::min(contentHeight, maxPopupHeight);

		ImGui::SetNextWindowPos(popupPos);
		ImGui::SetNextWindowBgAlpha(0.f);
		ImGui::SetNextWindowSize(ImVec2(popupSize.x, 0));
		if (!ImGui::BeginPopup("notifierPopup",
			ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoBackground))
		{
			m_IsOpen = false;
			return;
		}

		m_IsOpen = true;
		m_Viewed = true;

		if (m_ShouldClose)
		{
			ImGui::CloseCurrentPopup();
			m_IsOpen = false;
			m_ShouldClose = false;
			ImGui::EndPopup();
			return;
		}

		ImDrawList* draw = ImGui::GetWindowDrawList();
		ImVec2 winPos = ImGui::GetWindowPos();
		ImVec2 winSize = ImGui::GetWindowSize();
		ImVec2 winTL = winPos;
		ImVec2 winBR = ImVec2(winPos.x + winSize.x, winPos.y + winSize.y);
		const float popupRounding = style.PopupRounding;
		const ImU32 popupBg = ImGui::GetColorU32(ImGuiCol_PopupBg);
		const ImU32 popupBorder = ImGui::GetColorU32(ImGuiCol_Border);

		for (int i = 0; i < 4; ++i)
		{
			float pad = 2.0f + i * 2.0f;
			ImU32 col = IM_COL32(0, 0, 0, 20 - i * 4);
			draw->AddRectFilled(ImVec2(winTL.x - pad, winTL.y - pad), ImVec2(winBR.x + pad, winBR.y + pad), col, popupRounding + pad);
		}

		draw->AddRectFilled(winTL, winBR, popupBg, popupRounding);
		draw->AddRect(winTL, winBR, popupBorder, popupRounding, 0, 1.0f);

		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 12.0f);
		ImGui::TitleText("Notifications");

		bool muted = IsMuted();
		bool empty = m_Notifications.empty();

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - 120.0f);
		if (ImGui::SelectableLabel(muted ? ICON_MD_VOLUME_MUTE : ICON_MD_VOLUME_UP, false))
			ToggleMute();
		ImGui::ToolTip(muted ? "Unmute" : "Mute");

		ImGui::SameLine();
		if (ImGui::SelectableLabel(m_IsSnoozed ? ICON_MD_NOTIFICATIONS_PAUSED : ICON_MD_NOTIFICATIONS_ACTIVE, false))
			ToggleSnooze();
		ImGui::ToolTip(m_IsSnoozed ? "Enable toast notifications." : "Snooze toasts notifications.");

		ImGui::SameLine();
		ImGui::BeginDisabled(empty);
		if (ImGui::SelectableLabel(ICON_MD_CLEAR_ALL, false))
			m_ShouldFlush = true;
		ImGui::ToolTip("Clear All Notifications");
		ImGui::EndDisabled();

		ImGui::Separator();
		ImGui::Spacing();

		if (empty)
		{
			ImGui::TextDisabled("Wow! Such empty!");
		}
		else
		{
			float yCursor = ImGui::GetCursorScreenPos().y;
			float xLeft = ImGui::GetCursorScreenPos().x;
			float contentW = ImGui::GetContentRegionAvail().x;
			for (auto& n : m_Notifications)
			{
				if (n->m_Read)
					continue;

				n->Draw(xLeft, contentW, draw);
				ImGui::Dummy(ImVec2(1, 1));
			}

			ClearRead();
		}
		Flush();
		ImGui::EndPopup();
	}

	void Notifier::DrawToastsImpl()
	{
		if (m_Toasts.empty() || m_IsSnoozed)
			return;

		if (m_IsOpen)
		{
			m_Toasts.clear();
			return;
		}

		ImDrawList* drawList = ImGui::GetForegroundDrawList();
		ImGuiIO& io = ImGui::GetIO();
		const float margin = 15.0f;
		const float maxWidth = 360.0f;
		auto& toast = **m_Toasts.begin();
		auto notif = toast.Get();

		if (!notif || toast.HasExpired())
			m_Toasts.erase(m_Toasts.begin());
		else
		{
			ImVec2 toastPos(io.DisplaySize.x - maxWidth - margin, margin * 4);
			ImGui::SetCursorScreenPos(toastPos);
			bool isHovered = notif->Draw(toastPos.x, maxWidth, drawList);
			toast.SetIsHovered(isHovered && !m_IsOpen);

			auto remaining = m_Toasts.size() - 1;
			if (remaining > 0)
			{
				float radius = 12.0f;
				float toastHeight = notif->ComputeHeight();
				ImU32 counterBg = IM_COL32(55, 55, 55, 255);
				ImVec2 counterPos(toastPos.x + maxWidth - radius, toastPos.y - radius);
				ImGui::SetCursorScreenPos(toastPos);
				drawList->AddCircleFilled(counterPos, radius, counterBg);

				ImGui::PushFont(Fonts::Small);
				std::string countText = std::format("+{}", remaining);
				ImVec2 countTextSize = ImGui::CalcTextSize(countText.c_str());
				ImVec2 textPos(counterPos.x - (countTextSize.x / 2), counterPos.y - (countTextSize.y / 2));
				drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), countText.c_str());
				ImGui::PopFont();
			}
		}
	}

	void Notifier::Notification::Dismiss()
	{
		m_Read = true;
		m_Callback = nullptr;

		if (!IsOpen())
			ClearRead();
	}

	void Notifier::Notification::Invoke()
	{
		if (m_Callback)
		{
			auto& callback = m_Callback;
			ThreadManager::Run([callback] {
				callback();

				if (IsOpen())
					Close();
			});
		}
		Dismiss();
	}

	float Notifier::Notification::ComputeHeight() const noexcept
	{
		ImGui::PushFont(Fonts::Small);
		float wrapWidth = ImGui::GetContentRegionAvail().x - 20.0f;
		ImVec2 textSize = ImGui::CalcTextSize(m_Message.c_str(), nullptr, false, wrapWidth);
		ImGui::PopFont();

		float panelHeight = std::min(textSize.y + 60.0f, 144.0f);
		return panelHeight;
	}

	bool Notifier::Notification::Draw(float xLeft, float contentW, ImDrawList* drawList)
	{
		using namespace std::chrono_literals;

		ImGuiIO& io = ImGui::GetIO();
		auto now = std::chrono::system_clock::now();
		float age = std::chrono::duration_cast<std::chrono::duration<float>>(now - m_TimeCreated).count();
		const float animDur = 0.22f;
		float animT = ImSaturate(age / animDur);
		float ease = ImSaturate(animT);
		const float cardRounding = 10.0f;
		const float padding = 12.0f;
		const float accentWidth = 8.0f;
		const float titleSpacing = 9.0f;
		const float rightButtonW = 28.0f;
		ImFont* titleFont = Fonts::Bold;
		ImFont* bodyFont = Fonts::Small;
		ImVec2 cursorPos = ImVec2(xLeft, ImGui::GetCursorScreenPos().y);

		ImGui::PushFont(titleFont);
		ImVec2 titleSize = ImGui::CalcTextSize(m_Title.c_str());
		ImGui::PopFont();

		ImGui::PushFont(bodyFont);
		float wrap = contentW - (padding * 3.0f) - (accentWidth * 2.0f) - rightButtonW;
		ImVec2 bodySize = ImGui::CalcTextSize(m_Message.c_str(), nullptr, false, wrap);
		ImGui::PopFont();

		float cardHeight = std::max(50.0f, titleSize.y + titleSpacing + bodySize.y + (padding * 2));
		float cardWidth = contentW;
		float slideOffset = (1.0f - ease) * 10.0f;
		float alpha = (0.0f + ease) * m_GlobalAlpha;
		ImU32 textColor = ImGui::GetColorU32(ImGuiCol_Text, alpha);
		ImVec2 cardTL = ImVec2(cursorPos.x, cursorPos.y + slideOffset);
		ImVec2 cardBR = ImVec2(cursorPos.x + cardWidth, cursorPos.y + slideOffset + cardHeight);

		drawList->AddRectFilled(ImVec2(cardTL.x, cardTL.y + 4.0f), ImVec2(cardBR.x, cardBR.y + 6.5f), IM_COL32(0, 0, 0, static_cast<int>(55.0f * alpha)), cardRounding);
		ImU32 cardBg = ImGui::GetColorU32(ImGuiCol_ChildBg);
		drawList->AddRectFilled(cardTL, cardBR, cardBg, cardRounding);
		drawList->AddRect(cardTL, cardBR, IM_COL32(255, 255, 255, static_cast<int>(18.0f * alpha)), cardRounding, 0, 1.0f);
		bool isHovered = ImGui::IsMouseHoveringRect(cardTL, cardBR);

		ImVec2 accentCenter = ImVec2(cardTL.x + padding, cardTL.y + padding);
		ImU32 accentColor = ImGui::GetColorU32(m_Color);
		drawList->AddText(accentCenter, accentColor, m_Icon);

		ImVec2 titlePos = ImVec2(accentCenter.x + accentWidth + 20.0f, cardTL.y + padding);
		ImGui::PushFont(titleFont);
		drawList->AddText(titlePos, textColor, m_Title.c_str());
		ImGui::PopFont();

		ImVec2 btnPos = ImVec2(cardBR.x - padding - rightButtonW + 6.0f, cardTL.y + padding - 2.0f);
		ImVec2 btnBR = ImVec2(btnPos.x + 20.0f, btnPos.y + 20.0f);
		drawList->AddRectFilled(btnPos, btnBR, cardBg, 6.0f);
		ImGui::PushFont(Fonts::Small);
		drawList->AddText(ImVec2(btnPos.x + 3.0f, btnPos.y + 1), textColor, ICON_MD_CLEAR);
		ImGui::PopFont();

		ImVec2 bodyPos = ImVec2(accentCenter.x, accentCenter.y + titleSize.y + titleSpacing);
		ImGui::PushFont(bodyFont);
		drawList->AddText(
		    bodyFont,
		    ImGui::GetFontSize(),
		    ImVec2(cardTL.x + padding, cardTL.y + padding + titleSize.y + titleSpacing),
		    textColor,
		    m_Message.c_str(),
		    nullptr,
		    contentW - (padding * 2.0f) - accentWidth - rightButtonW
		);
		ImGui::PopFont();

		ImRect btnRect(btnPos, btnBR);
		if (!m_Read && ImGui::IsMouseHoveringRect(btnRect.Min, btnRect.Max))
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			if (ImGui::IsMouseClicked(0))
				Dismiss();
		}

		ImGui::SetCursorScreenPos(ImVec2(xLeft, cardBR.y + 8.0f));
		if (m_Callback != nullptr)
		{
			ImRect contentRect(ImVec2(cardTL.x, bodyPos.y), cardBR);
			if (ImGui::IsMouseHoveringRect(contentRect.Min, contentRect.Max))
			{
				drawList->AddRectFilled(contentRect.Min, contentRect.Max, IM_COL32(255, 255, 255, static_cast<int>(10.0f * alpha)), 0.f);
				ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
				if (ImGui::IsMouseClicked(0))
					Invoke();
			}
		}

		return isHovered;
	}
}
