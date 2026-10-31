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


#include <core/utils/unzip.hpp>
#include <core/gui/gui.hpp>
#include <core/gui/notifier.hpp>
#include "gitmgr.hpp"


using namespace std::chrono_literals;

namespace YLP
{
	void GitHubManager::InitImpl()
	{
		m_OrgName	  = "YimMenu-Lua";
		m_ScriptsPath = g_YimPath / "scripts";
		m_CacheFile	  = g_ProjectPath / "repo_cache.json";

		ThreadManager::RunDelayed([this]() {
			LoadETag();
			FetchRepositories();
		}, 400ms);
	}

	void GitHubManager::LoadETagImpl()
	{
		if (!std::filesystem::exists(m_CacheFile))
			return;

		std::ifstream file(m_CacheFile);
		if (!file.is_open())
			return;

		nlohmann::json j;
		file >> j;

		if (j.empty())
			return;

		m_RepoETag = j["ETag"];
	}

	bool GitHubManager::LoadCacheImpl(bool log)
	{
		if (!IO::Exists(m_CacheFile))
			return false;

		std::ifstream file(m_CacheFile);
		if (!file.is_open())
			return false;

		nlohmann::json j;
		file >> j;
		file.close();

		if (j.empty())
		{
			LOG_WARN("Lua repository cache is empty!");
			return false;
		}

		std::map<std::string, Repository> newRepos;
		m_RepoETag = j["ETag"];

		auto& repositories = j["repositories"];
		if (repositories.empty())
		{
			LOG_WARN("Lua repository cache is empty!");
			return false;
		}

		bool should_update_cache = false;
		bool has_pending_updates = false;
		for (auto& [name, data] : repositories.items())
		{
			Repository repo = data.get<Repository>();

			std::filesystem::path localPath		= m_ScriptsPath / name;
			std::filesystem::path disabledPath  = m_ScriptsPath / "disabled" / name;

			bool installed = IsScriptInstalled(name);
			bool disabled  = (IO::Exists(disabledPath) && IO::HasLuaFiles(disabledPath) && !IO::Exists(localPath));

			if (repo.m_IsInstalled && !installed)
			{
				repo.m_IsDisabled   = false;
				repo.m_IsInstalled  = false;
				repo.m_CurrentPath  = fs::path{};
				repo.m_LastChecked  = {};
				should_update_cache = true;
			}

			if (repo.m_IsInstalled != installed)
			{
				repo.m_IsInstalled  = installed;
				should_update_cache = true;
			}

			if (repo.m_IsDisabled != disabled)
			{
				repo.m_IsDisabled   = disabled;
				should_update_cache = true;
			}

			if (repo.IsOutdated())
				repo.m_IsPendingUpdate = has_pending_updates = true;

			repo.m_CurrentPath = installed ? (disabled ? disabledPath : localPath) : fs::path{};
			newRepos[name]     = std::move(repo);
		}

		{
			std::unique_lock lock(m_Mutex);
			m_Repos.swap(newRepos);

			if (should_update_cache)
				SaveCache();
		}

		if (has_pending_updates)
		{
			Notifier::Add("Lua", "Updates are available for some of your installed scripts.", Notifier::Info, [this] {
				SetSortMode(eLuaRepoSortMode::INSTALLED);
				GUI::SetActiveTab(eTabID::TAB_YIMMENU_LUA);
			});
		}

		return true;
	}

	void GitHubManager::SaveCacheImpl()
	{
		if (m_Repos.empty())
			return;

		nlohmann::json j;
		j["ETag"]	= m_RepoETag;
		auto& repos = j["repositories"];

		for (auto& [key, val] : m_Repos)
			repos[key] = val;

		std::ofstream file(m_CacheFile);
		file << j.dump(2);
		file.close();
	}

	void GitHubManager::FetchRepositoriesImpl()
	{
		try
		{
			LOG_INFO("Fetching Lua repositories from https://github.com/YimMenu-Lua");
			m_State = eLoadState::LOADING;

			bool cacheLoaded  = LoadCache();
			std::wstring host = L"api.github.com";
			std::wstring path = L"/orgs/" + std::wstring(m_OrgName.begin(), m_OrgName.end()) + L"/repos?per_page=100";

			std::vector<std::wstring> headers = {
				L"User-Agent: YLP-GitHubClient\r\n",
				L"Accept: application/vnd.github+json\r\n",
				L"If-None-Match: " + std::wstring(m_RepoETag.begin(), m_RepoETag.end()) + L"\r\n",
			};

			auto response = Utils::HttpRequest(host, path, headers);
			if (!response.success)
			{
				if (response.status == 304)
				{
					LOG_DEBUG("Repository cache is still valid. Loading it...");
					if (cacheLoaded)
					{
						SortRepositories();
						m_State = eLoadState::READY;
						LOG_INFO("Loaded {} Lua repositories from cache", m_Repos.size());
						return;
					}
					else
						LOG_ERROR("Failed to load repository cache! Attempting to re-fetch...");
				}
				else
				{
					LOG_ERROR("GitHub request failed with HTTP {}", response.status);
					m_State = eLoadState::FAILED;
					return;
				}
			}

			if (response.body.empty())
			{
				LOG_ERROR("Empty GitHub response!");
				m_State = eLoadState::FAILED;
				return;
			}

			auto j = nlohmann::json::parse(response.body, nullptr, false);
			if (j.is_discarded())
			{
				LOG_ERROR("Failed to parse GitHub response!");
				m_State = eLoadState::FAILED;
				return;
			}

			m_RepoETag = response.eTag;
			std::map<std::string, Repository> newRepos;
			std::map<std::string, Repository> oldRepos = m_Repos;

			for (auto& data : j)
			{
				std::string name = data["name"];
				if (std::find(m_IgnoreList.begin(), m_IgnoreList.end(), name) != m_IgnoreList.end())
				{
					LOG_INFO("Skipping repository {}", name);
					continue;
				}

				bool isInstalled = IsScriptInstalled(name);

				Repository repo{};
				repo.m_Name        = name;
				repo.m_Stars       = data.value("stargazers_count", 0);
				repo.m_LastUpdate  = data.value("pushed_at", "");
				repo.m_Description = data.value("description", "");
				repo.m_IsInstalled = isInstalled;
				repo.m_HtmlUrl     = std::format("https://github.com/{}/{}", m_OrgName, name);

				fs::path enabledPath  = m_ScriptsPath / name;
				fs::path disabledPath = m_ScriptsPath / "disabled" / name;

				repo.m_IsDisabled = IO::Exists(disabledPath) && IO::HasLuaFiles(disabledPath) && !IO::Exists(enabledPath);

				if (auto it = oldRepos.find(name); it != oldRepos.end())
					repo.m_LastChecked = it->second.m_LastChecked;

				repo.m_IsPendingUpdate = repo.IsOutdated();

				if (isInstalled)
					repo.m_CurrentPath =  repo.m_IsDisabled ? disabledPath : enabledPath;

				newRepos[name] = repo;
			}

			{
				std::unique_lock lock(m_Mutex);
				m_Repos.swap(newRepos);
			}

			LOG_INFO("Fetched {} repositories from GitHub", m_Repos.size());
			SaveCache();
			SortRepositories();
			m_State = eLoadState::READY;
		}
		catch (const std::exception& e)
		{
			LOG_ERROR("Exception caught while fetching repositories from Github! {}", e.what());
		}
	}

	void GitHubManager::RefreshRepositoriesImpl()
	{
		ThreadManager::RunDetached([this]() {
			std::map<std::string, Repository> newRepos;
			try
			{
				FetchRepositories();
				m_State = eLoadState::READY;
			}
			catch (const std::exception& e)
			{
				LOG_ERROR("Failed to refresh repositories! {}", e.what());
				m_State = eLoadState::FAILED;
			}
		});
	}

	void GitHubManager::SortRepositoriesImpl()
	{
		uint8_t mode  = Config().luaRepoSortMode;
		uint8_t order = Config().luaRepoSortOrder;

		std::vector<const Repository*> sorted;
		{
			std::shared_lock lock(m_Mutex);
			sorted.reserve(m_Repos.size());
			for (auto& [name, repo] : m_Repos)
			{
				if (mode == eLuaRepoSortMode::INSTALLED && !repo.m_IsInstalled)
					continue;

				sorted.push_back(&repo);
			}
		}

		std::sort(sorted.begin(), sorted.end(), [mode = mode, order = order](auto a, auto b) {
			bool isDescending = order == DESC;

			switch (mode)
			{
			case eLuaRepoSortMode::NAME: return isDescending ? a->m_Name < b->m_Name : a->m_Name > b->m_Name;
			case eLuaRepoSortMode::STARS: return isDescending ? a->m_Stars > b->m_Stars : a->m_Stars < b->m_Stars;
			case eLuaRepoSortMode::COMMIT: return isDescending ? a->m_LastUpdate > b->m_LastUpdate : a->m_LastUpdate < b->m_LastUpdate;
			default: return false;
			}
		});

		{
			std::unique_lock lock(m_Mutex);
			m_SortedView.swap(sorted);
		}
	}

	void GitHubManager::MoveRepositoryImpl(const std::string& name, std::filesystem::path src, std::filesystem::path dest)
	{
		std::scoped_lock lock(m_Mutex);
		auto it = m_Repos.find(name);
		if (it == m_Repos.end())
			return;

		auto& repo = it->second;
		if (repo.m_IsDownloading)
			return;

		if (!IO::Rename(src, dest)) // the function logs error details
			return;

		repo.m_IsDisabled  = dest.parent_path().filename().string() == "disabled";
		repo.m_CurrentPath = dest;
		SaveCache();
	}

	void GitHubManager::RemoveRepositoryImpl(const std::string& name)
	{
		std::scoped_lock lock(m_Mutex);
		auto it = m_Repos.find(name);
		if (it == m_Repos.end())
			return;

		auto& repo = it->second;
		if (repo.m_IsDownloading)
			return;

		if (!IO::RemoveAll(repo.m_CurrentPath))
			return;

		repo.m_IsDisabled  = false;
		repo.m_IsInstalled = false;
		repo.m_CurrentPath = fs::path{};
		repo.m_LastChecked = std::chrono::system_clock::time_point{};
		SaveCache();
	}

	void GitHubManager::DownloadRepositoryImpl(const std::string& name)
	{
		ThreadManager::RunDetached([this, name]() mutable {
			auto it = m_Repos.find(name);
			if (it == m_Repos.end())
				return;

			auto& repo = it->second;

			{
				std::scoped_lock lock(m_Mutex);
				if (repo.m_IsDownloading)
					return;

				repo.m_IsDownloading    = true;
				repo.m_DownloadProgress = 0.f;
			}

			const auto cacheDir = g_ProjectPath / "downloads_cache";
			if (!IO::Exists(cacheDir))
				IO::CreateFolders(cacheDir);

			const auto zipPath		= cacheDir / (name + ".zip");
			const auto extractDir	= cacheDir / (name + "_extracted");
			const auto installDir	= m_ScriptsPath / name;
			const std::wstring host = L"github.com";
			const std::wstring path = L"/"
				+ std::wstring(m_OrgName.begin(), m_OrgName.end())
				+ L"/" + std::wstring(name.begin(), name.end())
				+ L"/archive/refs/heads/main.zip";

			auto response = Utils::HttpRequest(host, path, {}, &zipPath, &repo.m_DownloadProgress);
			if (!response.success)
			{
				LOG_ERROR("Failed to download repository '{}' Error code: HTTP {}", name, response.status);
				std::scoped_lock lock(m_Mutex);
				repo.m_IsDownloading    = false;
				repo.m_DownloadProgress = 0.f;
				return;
			}

			LOG_INFO("Downloaded repository ZIP to {}. Extracting...", zipPath.string());

			if (!Unzip(zipPath, extractDir))
			{
				LOG_ERROR("Failed to extract repository '{}'", repo.m_Name);
				std::scoped_lock lock(m_Mutex);
				repo.m_IsDownloading    = false;
				repo.m_DownloadProgress = 0.f;
				return;
			}

			fs::path repoRoot;
			std::error_code ec{};
			for (auto& entry : fs::directory_iterator(extractDir, ec))
			{
				if (fs::is_directory(entry.path(), ec))
				{
					repoRoot = entry.path();
					break;
				}
			}

			if (repoRoot.empty())
			{
				LOG_ERROR("Could not locate extracted repository root folder in '{}'", extractDir.string());
				std::scoped_lock lock(m_Mutex);
				repo.m_IsDownloading    = false;
				repo.m_DownloadProgress = 0.f;
				return;
			}

			IO::FilterLuaFiles(repoRoot);

			// The human torch was denied a bank loan
			if (IO::Exists(installDir))
				IO::RemoveAll(installDir);

			IO::Rename(repoRoot, installDir);

			std::vector<fs::path> trash{};
			for (auto& entry : fs::recursive_directory_iterator(installDir, ec))
			{
				const auto& sub = entry.path();
				if (IO::IsDir(sub))
				{
					if (fs::is_empty(sub, ec))
						trash.push_back(sub);

					const std::string& name = sub.filename().string();
					if (name == "dev" || name == "SSV2-enhanced" || name.substr(0, 1) == ".") // I should get the clean release instead. am i stupid?
						trash.push_back(sub);
				}
			}

			for (const auto& p : trash)
				IO::RemoveAll(p);

			LOG_INFO("Installed repository at '{}'. Performing cleanup...", installDir.string());
			{
				std::scoped_lock lock(m_Mutex);
				repo.m_DownloadProgress = 0.f;
				repo.m_IsDownloading    = false;
				repo.m_IsInstalled      = true;
				repo.m_CurrentPath      = m_ScriptsPath / name;
				repo.m_LastChecked      = std::chrono::system_clock::now();
				repo.m_IsPendingUpdate  = repo.IsOutdated();
			}

			SaveCache();
			IO::RemoveAll(extractDir);
			IO::Remove(zipPath);
			LOG_INFO("Done.");
		});
	}

	const bool GitHubManager::IsScriptInstalled(const std::string& name)
	{
		auto enabledPath  = m_ScriptsPath / name;
		auto disabledPath = m_ScriptsPath / "disabled" / name;
		const bool exists = IO::Exists(enabledPath) || IO::Exists(disabledPath);
		const bool hasLua = IO::HasLuaFiles(enabledPath) || IO::HasLuaFiles(disabledPath);

		return exists && hasLua;
	}

}
