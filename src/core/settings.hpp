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

#include "gui/msgbox.hpp"
#include "injector/injector.hpp"


namespace YLP
{
	using json = nlohmann::json;

	enum eAutomationTarget : uint8_t
	{
		AT_NONE		= 0,
		AT_LEGACY	= 1 << 0,
		AT_ENHANCED = 1 << 1,
		AT_BOTH     = AT_LEGACY | AT_ENHANCED
	};

	enum eLuaRepoSortMode : uint8_t
	{
		COMMIT,
		STARS,
		NAME,
		INSTALLED
	};

	enum eLuaRepoSortOrder : uint8_t { ASC, DESC };

	class Settings : public Singleton<Settings>
	{
		friend class Singleton<Settings>;

	private:
		Settings() = default;

	public:
		~Settings()
		{
			Save();
		}

		struct Config
		{
			bool internalConsole      = false;
			bool externalConsole      = false;
			bool autoExit             = false;
			bool restoreLastTab       = false;
			bool fullscreenWindow     = false;
			bool muteNotifs           = false;
			bool enableScripting      = false;
			bool autoReloadLuaModules = false;
			bool disableTooltips      = false;
			bool expandedSidebar      = false;

			int lastTabIndex         = 0;
			int themeIndex           = 0;
			int mainWindowIndex      = 0; // bad naming. this represents Legacy/Enhanced in the main tab
			int scriptingWindowIndex = 0; // bad naming. this represents CodeExecutor/Script GUIs in the Scripting tab
			int windowAccentState    = 0;
			int windowWidth          = 800;
			int windowHeight         = 770;
			int windowX              = -1;
			int windowY              = -1;
			int launcherIndex        = -1;
			float bgAlphaMultiplier  = 1.0f;

			uint8_t autoMonitorFlags    = AT_NONE;
			uint8_t menuAutoUpdateFlags = AT_NONE;
			uint8_t luaRepoSortMode     = eLuaRepoSortMode::COMMIT;
			uint8_t luaRepoSortOrder    = eLuaRepoSortOrder::DESC;

			std::string uuid{};
			std::vector<IO::DllInfo> savedDlls{};
			std::pair<std::string, std::filesystem::path> savedTheme{};
			std::unordered_map<std::string, std::filesystem::path> gtaExePaths{}; // exeName -> path

			Injector::InjectorConfig injectorConfig{};

			void Reset()
			{
				*this = {};
			}
		} m_Config;

		static void Init(const std::filesystem::path& path)
		{
			GetInstance().InitImpl(path);
		}

		static void Destroy()
		{
			GetInstance().SaveImpl();
		}

		static void Save()
		{
			GetInstance().SaveImpl();
		}

		static void Load()
		{
			GetInstance().LoadImpl();
		}

		static Config& Get()
		{
			return GetInstance().m_Config;
		}

		static void Reset()
		{
			GetInstance().ResetImpl();
		}

		template<typename F>
		static void Update(F&& func)
		{
			GetInstance().UpdateImpl(func);
		}

	private:
		fs::path m_FilePath{};

		void InitImpl(const fs::path& path)
		{
			m_FilePath = path;
			if (!fs::exists(path))
				Save();
			Load();

			fs::path parent = path.parent_path();
			std::error_code ec{};
			if (fs::exists(parent / "ylp.id", ec))
			{
				std::ifstream f(parent / "ylp.id");
				if (!f.is_open())
					return;

				std::string uuid;
				std::getline(f, uuid);
				if (m_Config.uuid == uuid)
					return;

				std::string msg = "Your user settings seem to have been tampered with. Lua scripting has been disabled for your safety.";
				MsgBox::Warn("Config Mismatch!", msg);
				LOG_WARN(msg);
				m_Config.enableScripting = false;
			}

			m_Config.enableScripting = false;
			std::ofstream f(parent / "ylp.id");
			if (!f.is_open())
				return;

			std::string uuid = Utils::GenerateUUID();
			f << uuid;
			m_Config.uuid = uuid;
		}

		void SaveImpl()
		{
			json j;
			j["auto_monitor_flags"]      = m_Config.autoMonitorFlags;
			j["menu_auto_update_flags"]  = m_Config.menuAutoUpdateFlags;
			j["lua_repo_sort_mode"]      = m_Config.luaRepoSortMode;
			j["lua_repo_sort_order"]     = m_Config.luaRepoSortOrder;
			j["launcher_idx"]            = m_Config.launcherIndex;
			j["bg_alpha_multiplier"]     = m_Config.bgAlphaMultiplier;
			j["main_window_index"]       = m_Config.mainWindowIndex;
			j["scripting_window_index"]  = m_Config.scriptingWindowIndex;
			j["window_accent_state"]     = m_Config.windowAccentState;
			j["last_tab_index"]          = m_Config.lastTabIndex;
			j["internal_console"]        = m_Config.internalConsole;
			j["external_console"]        = m_Config.externalConsole;
			j["auto_exit"]               = m_Config.autoExit;
			j["restore_last_tab"]        = m_Config.restoreLastTab;
			j["full_screen"]             = m_Config.fullscreenWindow;
			j["mute_notifs"]             = m_Config.muteNotifs;
			j["saved_theme"]             = m_Config.savedTheme;
			j["window_width"]            = m_Config.windowWidth;
			j["window_height"]           = m_Config.windowHeight;
			j["window_x"]                = m_Config.windowX;
			j["window_y"]                = m_Config.windowY;
			j["enable_scripting"]        = m_Config.enableScripting;
			j["auto_reload_lua_modules"] = m_Config.autoReloadLuaModules;
			j["disable_tooltips"]        = m_Config.disableTooltips;
			j["expanded_sidebar"]        = m_Config.expandedSidebar;
			j["uuid"]                    = m_Config.uuid;
			j["injector_config"]         = m_Config.injectorConfig;

			for (const auto& dll : m_Config.savedDlls)
				j["saved_dlls"].push_back({
				    {"name",				dll.m_Name},
				    {"path",				dll.m_Path.string()},
				    {"checksum",			dll.m_Checksum},
				    {"is64bit",				dll.m_Is64bit},
				    {"has_exports",			dll.m_HasExports},
				    {"last_process_name",	dll.m_LastKnownProcess},
				});

			for (const auto& pair : m_Config.gtaExePaths)
				j["gta_exe_paths"][pair.first] = pair.second.string();

			std::ofstream f(m_FilePath);
			f << std::setw(4) << j;
			f.close();
		}

		void LoadImpl()
		{
			if (!IO::Exists(m_FilePath))
				return;

			std::ifstream f(m_FilePath);
			if (!f.is_open())
				return;

			json j;
			try
			{
				f >> j;
			}
			catch (const std::exception&)
			{
				LOG_WARN("Settings file is invalid, resetting to defaults...");
				ResetImpl();
				return;
			}
			f.close();

			m_Config.autoMonitorFlags     = j.value("auto_monitor_flags", AT_NONE);
			m_Config.menuAutoUpdateFlags  = j.value("menu_auto_update_flags", AT_NONE);
			m_Config.luaRepoSortMode      = j.value("lua_repo_sort_mode", eLuaRepoSortMode::COMMIT);
			m_Config.luaRepoSortOrder     = j.value("lua_repo_sort_order", eLuaRepoSortOrder::DESC);
			m_Config.lastTabIndex         = j.value("last_tab_index", 0);
			m_Config.windowAccentState    = j.value("window_accent_state", 0);
			m_Config.mainWindowIndex      = j.value("main_window_index", 0);
			m_Config.scriptingWindowIndex = j.value("scripting_window_index", 0);
			m_Config.launcherIndex        = j.value("launcher_idx", -1);
			m_Config.bgAlphaMultiplier    = j.value("bg_alpha_multiplier", 1.0f);
			m_Config.internalConsole      = j.value("internal_console", true);
			m_Config.externalConsole      = j.value("external_console", false);
			m_Config.autoExit             = j.value("auto_exit", false);
			m_Config.restoreLastTab       = j.value("restore_last_tab", false);
			m_Config.fullscreenWindow     = j.value("full_screen", false);
			m_Config.muteNotifs           = j.value("mute_notifs", false);
			m_Config.savedTheme           = j.value("saved_theme", std::pair<std::string, std::filesystem::path>{});
			m_Config.windowX              = j.value("window_x", -1);
			m_Config.windowY              = j.value("window_y", -1);
			m_Config.windowWidth          = j.value("window_width", 680);
			m_Config.windowHeight         = j.value("window_height", 720);
			m_Config.enableScripting      = j.value("enable_scripting", false);
			m_Config.autoReloadLuaModules = j.value("auto_reload_lua_modules", false);
			m_Config.disableTooltips      = j.value("disable_tooltips", false);
			m_Config.expandedSidebar      = j.value("expanded_sidebar", false);
			m_Config.uuid                 = j.value("uuid", "");
			m_Config.injectorConfig       = j.value("injector_config", Injector::InjectorConfig{});

			json& savedDlls = j["saved_dlls"];
			if (!savedDlls.is_null() && savedDlls.is_array())
			{
				for (auto& entry : savedDlls)
				{
					fs::path path = entry.value("path", "");
					if (!IO::Exists(path))
						continue;

					IO::DllInfo dll{};
					dll.m_Ok                   = true;
					dll.m_Path                 = path;
					dll.m_Name                 = entry.value("name", "");
					dll.m_Checksum             = entry.value("checksum", "");
					dll.m_LastKnownProcess     = entry.value("last_process_name", "");
					dll.m_Is64bit              = entry.value("is64bit", false);
					dll.m_HasExports           = entry.value("has_exports", false);

					m_Config.savedDlls.push_back(std::move(dll));
				}
			}

			if (j.contains("gta_exe_paths") && !j["gta_exe_paths"].is_null() && j["gta_exe_paths"].is_object())
			{
				for (auto& [key, value] : j["gta_exe_paths"].items())
				{
					m_Config.gtaExePaths[key] = value.get<std::string>();
				}
			}
		}

		template<typename F>
		void UpdateImpl(F&& func)
		{
			func(m_Config);
			SaveImpl();
		}

		void ResetImpl()
		{
			m_Config.Reset();
			SaveImpl();
		}
	};
}
