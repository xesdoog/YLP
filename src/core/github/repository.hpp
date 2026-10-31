// YLP Project - GPL-3.0
// See LICENSE file or <https://www.gnu.org/licenses/> for details.


#pragma once


namespace YLP
{
	struct Repository
	{
		std::string m_Name;
		std::string m_Description;
		std::string m_LastUpdate;
		std::string m_HtmlUrl;

		std::chrono::system_clock::time_point m_LastChecked{};

		bool m_IsInstalled     = false;
		bool m_IsDisabled      = false;
		bool m_IsDownloading   = false;
		bool m_IsPendingUpdate = false;
		bool m_IsStarred       = false; // this is unused. We no longer have GitHub authentication like we did in the deprecated Python version

		int m_Stars              = 0;
		float m_DownloadProgress = 0.f;

		fs::path m_CurrentPath;

		bool IsOutdated() const
		{
			if (!m_IsInstalled)
				return false;

			if (m_LastChecked.time_since_epoch().count() == 0)
				return false;

			std::tm tm = {};
			std::istringstream ss(m_LastUpdate);
			ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
			auto pushedTime = std::chrono::system_clock::from_time_t(_mkgmtime(&tm));

			return m_LastChecked < pushedTime;
		}
	};

	inline void to_json(nlohmann::json& j, const Repository& r)
	{
		j = nlohmann::json{
		    {"name", r.m_Name},
		    {"description", r.m_Description},
		    {"lastUpdate", r.m_LastUpdate},
		    {"stars", r.m_Stars},
		    {"isStarred", r.m_IsStarred},
		    {"isInstalled", r.m_IsInstalled},
		    {"isDisabled", r.m_IsDisabled},
		    {"htmlUrl", r.m_HtmlUrl},
		    {"lastChecked", std::chrono::system_clock::to_time_t(r.m_LastChecked)},
		};
	}

	inline void from_json(const nlohmann::json& j, Repository& r)
	{
		j.at("name").get_to(r.m_Name);
		j.at("description").get_to(r.m_Description);
		j.at("lastUpdate").get_to(r.m_LastUpdate);
		j.at("stars").get_to(r.m_Stars);
		j.at("isStarred").get_to(r.m_IsStarred);
		j.at("isInstalled").get_to(r.m_IsInstalled);
		j.at("isDisabled").get_to(r.m_IsDisabled);
		j.at("htmlUrl").get_to(r.m_HtmlUrl);

		if (j.contains("lastChecked") && !j["lastChecked"].is_null())
		{
			std::time_t t = j["lastChecked"].get<std::time_t>();
			r.m_LastChecked = std::chrono::system_clock::from_time_t(t);
		}
		else
		{
			r.m_LastChecked = std::chrono::system_clock::time_point{};
		}
	}
}
