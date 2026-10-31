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


#include "renderer.hpp"
#include "gui.hpp"
#include "window_bg_state.hpp"

#include "../../resources/res.h"
#include "../../thirdparty/stb_image.h"


namespace YLP
{
	Renderer::~Renderer()
	{
		if (m_Initialized)
			DestroyImpl();
	}

	void Renderer::DestroyImpl()
	{
		if (!m_Initialized)
			return;

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		wglMakeCurrent(nullptr, nullptr);
		wglDeleteContext(m_HGLRC);
		ReleaseDC(m_HWND, m_HDC);
		UnregisterClass(m_WndClass.lpszClassName, g_Instance);

		for (const auto& texture : m_Textures)
		{
			glDeleteTextures(1, &texture.m_Id);
		}

		m_Textures.clear();
		m_Initialized = false;
	}

	bool Renderer::InitImpl()
	{
		if (m_Initialized)
			return true;

		LPCSTR YLP = "YLP";
		m_WndClass = {
		    sizeof(WNDCLASSEX),
		    CS_CLASSDC,
		    WndProc,
		    0L,
		    0L,
		    g_Instance,
		    nullptr,
		    nullptr,
		    nullptr,
		    nullptr,
		    YLP,
		    nullptr};

		ATOM regResult = RegisterClassEx(&m_WndClass);
		if (regResult == 0)
		{
			LOG_ERROR("RegisterClassExW failed with code {}", GetLastError());
			return false;
		}

		auto& cfg = Settings::Get();
		m_Width   = cfg.windowWidth;
		m_Height  = cfg.windowHeight;

		int x, y;
		if (cfg.windowX == -1 || cfg.windowY == -1)
		{
			RECT desktop;
			const HWND hDesktop = GetDesktopWindow();
			GetClientRect(hDesktop, &desktop);
			x = (desktop.right - m_Width) / 2;
			y = (desktop.bottom - m_Height) / 2;
		}
		else
		{
			x = cfg.windowX;
			y = cfg.windowY;
		}

		m_HWND = CreateWindow(
		    m_WndClass.lpszClassName,
		    YLP,
		    WS_OVERLAPPEDWINDOW,
		    x, y,
		    m_Width,
		    m_Height,
		    nullptr,
		    nullptr,
		    m_WndClass.hInstance,
		    nullptr);

		if (!m_HWND)
		{
			LOG_ERROR("CreateWindowW failed with error code {}", GetLastError());
			return false;
		}
		g_Hwnd = m_HWND;

		HICON hIcon = static_cast<HICON>(LoadImageW(
		    GetModuleHandle("YLP.exe"),
		    MAKEINTRESOURCEW(IDI_APPICON),
		    IMAGE_ICON,
		    0,
		    0,
		    LR_DEFAULTSIZE | LR_SHARED));

		if (hIcon)
		{

			auto lParam = reinterpret_cast<LPARAM>(hIcon);
			SendMessage(m_HWND, WM_SETICON, ICON_BIG,   lParam);
			SendMessage(m_HWND, WM_SETICON, ICON_SMALL, lParam);
		}
		else
			LOG_WARN("Failed to load window icon!");

		SetBackgroundAccentState(m_HWND, static_cast<eWindowAccentState>(cfg.windowAccentState));
		ShowWindow(m_HWND, cfg.fullscreenWindow ? SW_SHOWMAXIMIZED : SW_SHOWDEFAULT);
		UpdateWindow(m_HWND);

		m_HDC = GetDC(m_HWND);
		PIXELFORMATDESCRIPTOR pfd{
		    .nSize		  = sizeof(pfd),
		    .nVersion     = 1,
		    .dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		    .iPixelType   = PFD_TYPE_RGBA,
		    .cColorBits   = 32,
			.cAlphaBits   = 8,
		    .cDepthBits   = 24,
		    .cStencilBits = 8,
		    .iLayerType   = PFD_MAIN_PLANE,
		};

		int pf = ChoosePixelFormat(m_HDC, &pfd);
		if (pf == 0)
		{
			LOG_ERROR("ChoosePixelFormat failed!");
			return false;
		}

		if (!SetPixelFormat(m_HDC, pf, &pfd))
		{
			LOG_ERROR("SetPixelFormat failed!");
			return false;
		}

		m_HGLRC = wglCreateContext(m_HDC);
		if (!m_HGLRC)
		{
			LOG_ERROR("wglCreateContext failed!");
			return false;
		}

		wglMakeCurrent(m_HDC, m_HGLRC);
		if (wglGetCurrentContext() == nullptr)
		{
			LOG_ERROR("Failed to initialize OpenGL context!");
			return false;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		RECT rect;
		GetClientRect(m_HWND, &rect);
		m_Width  = rect.right - rect.left;
		m_Height = rect.bottom - rect.top;

		ImGui_ImplWin32_Init(m_HWND);
		ImGui_ImplOpenGL3_Init("#version 130");
		GUI::Init();

		m_Initialized = true;
		return true;
	}

	void Renderer::SetResizingImpl(LPARAM lParam)
	{
		m_Width          = LOWORD(lParam);
		m_Height         = HIWORD(lParam);
		m_ResizePending  = true;
		m_LastResizeTime = std::chrono::steady_clock::now();
	}

	std::atomic<bool> Renderer::IsFocusedImpl() const
	{
		WINDOWPLACEMENT wp{0};
		GetWindowPlacement(m_HWND, &wp);

		const bool minimized = (wp.showCmd == SW_SHOWMINIMIZED);
		const bool focused   = (GetForegroundWindow() == m_HWND);
		return focused && !minimized;
	}

	LRESULT Renderer::WndProcImpl(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_SIZE:
		{
			if (wParam != SIZE_MINIMIZED)
				SetResizing(lParam);
			return 0;
		}
		case WM_GETMINMAXINFO:
		{
			auto mmi = reinterpret_cast<MINMAXINFO*>(lParam);
			mmi->ptMinTrackSize.x = 600;
			mmi->ptMinTrackSize.y = 500;
			return 0;
		}
		case WM_SYSCOMMAND:
			if ((wParam & 0xFFF0) == SC_KEYMENU)
				return 0;
			break;
		case WM_CLOSE:
		{
			WINDOWPLACEMENT wp{};
			wp.length = sizeof(WINDOWPLACEMENT);
			if (GetWindowPlacement(hwnd, &wp))
			{
				RECT& rc  = wp.rcNormalPosition;
				auto& cfg = Settings::Get();

				cfg.windowWidth      = rc.right - rc.left;
				cfg.windowHeight     = rc.bottom - rc.top;
				cfg.windowX		     = rc.left;
				cfg.windowY		     = rc.top;
				cfg.fullscreenWindow = wp.showCmd == SW_SHOWMAXIMIZED;
			}
			DestroyWindow(hwnd);
			return 0;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	void Renderer::DrawImpl()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		
		GUI::Draw();

		if (m_ResizePending)
		{
			auto now     = std::chrono::steady_clock::now();
			auto elapsed = duration_cast<std::chrono::milliseconds>(now - m_LastResizeTime).count();
			if (elapsed > 100)
			{
				m_ResizePending = false;
				ImGuiIO& io     = ImGui::GetIO();
				io.DisplaySize  = ImVec2(static_cast<float>(m_Width), static_cast<float>(m_Height));
				glViewport(0, 0, m_Width, m_Height);
			}
		}

		ImGui::Render();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		SwapBuffers(m_HDC);
		ProcessTextureRequests();
	}

	ImVec2 Renderer::GetWindowSizeImpl() noexcept
	{
		return ImVec2(static_cast<float>(m_Width), static_cast<float>(m_Height));
	}

	std::optional<ImTextureID> Renderer::FindTextureByName(const std::string& name)
	{
		for (auto& tex : m_Textures)
		{
			if (tex.m_Name == name)
			{
				tex.m_RefCount++;
				return tex.m_ImGuiId;
			}
		}

		return std::nullopt;
	}

	ImTextureID Renderer::LoadTextureFromFileImpl(const fs::path& filepath, bool verbose)
	{
		std::string name = filepath.filename().string();
		if (auto maybeTex = FindTextureByName(name); maybeTex.has_value())
			return maybeTex.value();

		int w, h, channels;
		unsigned char* data = stbi_load(filepath.string().c_str(), &w, &h, &channels, 4);
		if (!data)
		{
			LOG_ERROR("Failed to load texture '{}' from '{}'", name, filepath.string());
			return 0;
		}

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);

		ImTextureID imguiId = static_cast<ImTextureID>(static_cast<intptr_t>(tex));
		m_Textures.push_back({name, tex, imguiId, 1});

		if (verbose)
			LOG_DEBUG("Loaded texture '{}' from '{}'", name, filepath.string());

		return imguiId;
	}

	ImTextureID Renderer::LoadTextureFromMemoryImpl(const unsigned char* data, size_t size, const std::string& name, bool verbose)
	{
		if (auto maybeTex = FindTextureByName(name); maybeTex.has_value())
			return maybeTex.value();

		int w, h, channels;
		unsigned char* pdata = stbi_load_from_memory(data, size, &w, &h, &channels, 4);
		if (!pdata)
		{
			LOG_ERROR("Failed to load texture: '{}'", name);
			stbi_image_free(pdata);
			return NULL;
		}

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pdata);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(pdata);

		ImTextureID imguiId = static_cast<ImTextureID>(static_cast<intptr_t>(tex));
		m_Textures.push_back({name, tex, imguiId, 1});

		if (verbose)
			LOG_DEBUG("Loaded texture '{}'", name);

		return imguiId;
	}

	ImTextureID Renderer::LoadRawTextureImpl(const unsigned char* rgbaData, int w, int h, const std::string& name, bool verbose)
	{
		if (auto maybeTex = FindTextureByName(name); maybeTex.has_value())
			return maybeTex.value();

		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaData);

		ImTextureID imguiId = static_cast<ImTextureID>(static_cast<intptr_t>(tex));
		m_Textures.push_back({name, tex, imguiId, 1});

		if (verbose)
			LOG_DEBUG("Loaded texture '{}'", name);

		return imguiId;
	}

	void Renderer::RequestTextureImpl(const fs::path& filepath, ImTextureID* outTexture, bool verbose)
	{
		std::scoped_lock lock(m_PendingTextureMutex);
		m_PendingTextures.push_back({.m_Name = filepath.filename().string(),
		    .m_FilePath                      = filepath,
		    .m_LoadType                      = eTextureLoadType::TLT_FILE,
		    .m_OutId                         = outTexture});
	}

	void Renderer::RequestTextureImpl(const std::vector<uint8_t> data, size_t size, const std::string& name, ImTextureID* outTexture, bool verbose)
	{
		std::scoped_lock lock(m_PendingTextureMutex);
		m_PendingTextures.push_back({.m_Name = name,
		    .m_TexData                       = data,
		    .m_DataSize                      = size,
		    .m_LoadType                      = eTextureLoadType::TLT_MEMORY,
		    .m_OutId                         = outTexture});
	}

	void Renderer::RequestTextureImpl(const std::vector<uint8_t> rgbaData, int w, int h, const std::string& name, ImTextureID* outTexture, bool verbose)
	{
		std::scoped_lock lock(m_PendingTextureMutex);
		m_PendingTextures.push_back({.m_Name = name,
		    .m_TexData                       = rgbaData,
		    .m_Width                         = w,
		    .m_Height                        = h,
		    .m_LoadType                      = eTextureLoadType::TLT_RAW,
		    .m_OutId                         = outTexture});
	}

	void Renderer::ReleaseTextureImpl(const std::string& name)
	{
		std::scoped_lock lock(m_TextureMutex);
		for (auto it = m_Textures.begin(); it != m_Textures.end(); ++it)
		{
			if (it->m_Name == name)
			{
				if (--it->m_RefCount <= 0)
				{
					glDeleteTextures(1, &it->m_Id);
					m_Textures.erase(it);
				}
				break;
			}
		}
	}

	void Renderer::ReleaseTextureImpl(const ImTextureID& imguiID)
	{
		std::scoped_lock lock(m_TextureMutex);
		for (auto it = m_Textures.begin(); it != m_Textures.end(); ++it)
		{
			if (it->m_ImGuiId == imguiID)
			{
				if (--it->m_RefCount <= 0)
				{
					glDeleteTextures(1, &it->m_Id);
					m_Textures.erase(it);
				}
				break;
			}
		}
	}

	void Renderer::ProcessTextureRequests()
	{
		if (m_PendingTextures.empty())
			return;

		auto now = std::chrono::steady_clock::now();
		if (now - m_LastPendingTexProcessTime < 16ms)
			return;

		std::scoped_lock lock(m_PendingTextureMutex);

		auto& it       = m_PendingTextures.back();
		bool shouldPop = false;

		switch (it.m_LoadType)
		{
		case eTextureLoadType::TLT_FILE:
		{
			auto& path = it.m_FilePath;
			if (!IO::Exists(path))
			{
				shouldPop = true;
				break;
			}

			*it.m_OutId = LoadTextureFromFile(path, false);
			shouldPop   = true;
			break;
		}
		case eTextureLoadType::TLT_MEMORY:
		{
			const unsigned char* data = it.m_TexData.data();
			if (!data)
			{
				shouldPop = true;
				break;
			}

			*it.m_OutId = LoadTextureFromMemory(data, it.m_DataSize, it.m_Name, false);
			shouldPop   = true;
			break;
		}
		case eTextureLoadType::TLT_RAW:
		{
			const unsigned char* data = it.m_TexData.data();
			if (!data)
			{
				shouldPop = true;
				break;
			}

			*it.m_OutId = LoadRawTexture(data, it.m_Width, it.m_Height, it.m_Name, false);
			shouldPop   = true;
			break;
		}
		}

		if (shouldPop)
			m_PendingTextures.pop_back();

		m_LastPendingTexProcessTime = now;
	}

	LRESULT CALLBACK Renderer::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		return GetInstance().WndProcImpl(hwnd, msg, wparam, lparam);
	}
}
