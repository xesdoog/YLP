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


#include "common.hpp"
#include "core/updater.hpp"
#include "core/gui/renderer.hpp"
#include "core/gui/msgbox.hpp"
#include "core/gui/notifier.hpp"
#include "core/github/gitmgr.hpp"
#include "core/YimMenu/yimmenu.hpp"
#include "core/memory/pointers.hpp"
#include "core/lua_scripting/lua_mgr.hpp"


using namespace YLP;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	GlobalMutex lock("YLPCPPMUTEX");
	if (!lock.IsOwned())
	{
		HWND hExHWND = FindWindowA(nullptr, "YLP");
		if (hExHWND)
			SetForegroundWindow(hExHWND);

		MsgBox::Error(L"Error", L"YLP is already running!");
		return 0;
	}

	auto appdata = std::filesystem::path(std::getenv("appdata"));
	g_Instance = GetModuleHandle(nullptr);
	g_ProjectPath = appdata / "YLP";
	g_YimPath = appdata / "YimMenu";
	g_YimV2Path = appdata / "YimMenuV2";

	if (!std::filesystem::exists(g_ProjectPath))
		std::filesystem::create_directory(g_ProjectPath);

	Settings::Init(g_ProjectPath / "settings.json");
	Logger::Init(g_ProjectPath / "cout.log", Config().externalConsole);
	ThreadManager::Init(4);

	if (!std::filesystem::exists(g_YimPath))
		LOG_INFO("User does not seem to have used YimMenu before, or at least not recently.");

	if (!std::filesystem::exists(g_YimV2Path))
		LOG_INFO("User does not seem to have used YimMenu V2 before, or at least not recently.");

	if (!Renderer::Init())
	{
		Renderer::Destroy();
		ThreadManager::Shutdown();
		Settings::Destroy();
		return 1;
	}

	GitHubManager::Init();
	YimMenuHandler::Init();
	g_Pointers.Init();
	LuaJIT::LuaManager::Init(g_ProjectPath / "Plugins");

	ThreadManager::RunDelayed([] {
		YLPUpdater.Check();
	}, 5s);

	g_Running = true;	
	MSG msg = {};	
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		Renderer::Draw();
	}

	g_Running = false;
	Renderer::Destroy();
	Logger::Destroy();
	ThreadManager::Shutdown();
	Settings::Destroy();

	std::filesystem::path dcache = g_ProjectPath / "downloads_cache";
	if (std::filesystem::exists(dcache))
	{
		try
		{
			std::filesystem::remove(dcache);
		}
		catch (...)
		{
		}
	}

	return 0;
}
