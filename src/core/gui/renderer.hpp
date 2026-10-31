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

#include <gl/GL.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_opengl3.h>

namespace YLP
{
	using PFNWGLSWAPINTERVALEXTPROC    = BOOL(WINAPI*)(int);
	using PFNWGLGETSWAPINTERVALEXTPROC = int(WINAPI*)();

	class Renderer : public Singleton<Renderer>
	{
		friend class Singleton<Renderer>;

	private:
		Renderer() = default;

	public:
		~Renderer();

		Renderer(const Renderer&)                = delete;
		Renderer(Renderer&&) noexcept            = delete;
		Renderer& operator=(const Renderer&)     = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		enum eTextureLoadType : uint8_t
		{
			TLT_FILE,
			TLT_MEMORY,
			TLT_RAW,
		};

		static void Destroy()
		{
			GetInstance().DestroyImpl();
		}

		static bool Init()
		{
			return GetInstance().InitImpl();
		}

		static void Draw()
		{
			return GetInstance().DrawImpl();
		};

		static void SetResizing(LPARAM lParam)
		{
			GetInstance().SetResizingImpl(lParam);
		}

		static const std::atomic<bool> IsFocused()
		{
			return GetInstance().IsFocusedImpl();
		}

		static HWND GetWindowHandle()
		{
			return GetInstance().m_HWND;
		}

		static ImVec2 GetWindowSize() noexcept
		{
			return GetInstance().GetWindowSizeImpl();
		}

		static ImTextureID LoadTextureFromFile(const fs::path& filepath, bool verbose = true)
		{
			return GetInstance().LoadTextureFromFileImpl(filepath, verbose);
		}

		static ImTextureID LoadTextureFromMemory(const unsigned char* data, size_t size, const std::string& name, bool verbose = true)
		{
			return GetInstance().LoadTextureFromMemoryImpl(data, size, name, verbose);
		}

		static ImTextureID LoadRawTexture(const unsigned char* rgbaData, int w, int h, const std::string& name, bool verbose = true)
		{
			return GetInstance().LoadRawTextureImpl(rgbaData, w, h, name, verbose);
		}

		static void RequestTextureFromFile(const fs::path& filepath, ImTextureID* outTexture, bool verbose)
		{
			GetInstance().RequestTextureImpl(filepath, outTexture, verbose);
		}

		static void RequestTextureFromMemory(const std::vector<uint8_t> data, size_t size, const std::string& name, ImTextureID* outTexture, bool verbose)
		{
			GetInstance().RequestTextureImpl(data, size, name, outTexture, verbose);
		}

		static void RequestTextureRaw(const std::vector<uint8_t> rgbaData, int w, int h, const std::string& name, ImTextureID* outTexture, bool verbose)
		{
			GetInstance().RequestTextureImpl(rgbaData, w, h, name, outTexture, verbose);
		}

		static void ReleaseTexture(const std::string& name)
		{
			GetInstance().ReleaseTextureImpl(name);
		}

		static void ReleaseTexture(const ImTextureID& imguiID)
		{
			GetInstance().ReleaseTextureImpl(imguiID);
		}

		static const bool IsResizing() noexcept
		{
			return GetInstance().m_ResizePending;
		}

		static const bool IsVSyncEnabled() noexcept
		{
			return GetInstance().m_HasVSync;
		}

		static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:
		void DestroyImpl();
		bool InitImpl();
		void DrawImpl();
		void SetResizingImpl(LPARAM lParam);

		std::atomic<bool> IsFocusedImpl() const;

		ImVec2 GetWindowSizeImpl() noexcept;

		std::optional<ImTextureID> FindTextureByName(const std::string& name);

		ImTextureID LoadTextureFromFileImpl(const fs::path& filepath, bool verbose);
		ImTextureID LoadTextureFromMemoryImpl(const unsigned char* data, size_t size, const std::string& name, bool verbose);
		ImTextureID LoadRawTextureImpl(const unsigned char* rgbaData, int w, int h, const std::string& name, bool verbose);

		void RequestTextureImpl(const fs::path& filepath, ImTextureID* outTexture, bool verbose);
		void RequestTextureImpl(const std::vector<uint8_t> data, size_t size, const std::string& name, ImTextureID* outTexture, bool verbose);
		void RequestTextureImpl(const std::vector<uint8_t> rgbaData, int w, int h, const std::string& name, ImTextureID* outTexture, bool verbose);
		void ProcessTextureRequests();
		void ReleaseTextureImpl(const std::string& name);
		void ReleaseTextureImpl(const ImTextureID& imguiID);

		WNDCLASSEX m_WndClass{};
		HWND m_HWND{};
		HDC m_HDC{};
		HGLRC m_HGLRC{};

		bool m_Initialized   = false;
		bool m_ResizePending = false;
		bool m_HasVSync      = false;

		int m_Width  = 680;
		int m_Height = 720;

		std::chrono::steady_clock::time_point m_LastResizeTime;
		std::chrono::steady_clock::time_point m_LastPendingTexProcessTime;

		LRESULT WndProcImpl(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		struct Texture
		{
			std::string m_Name;
			GLuint m_Id;
			ImTextureID m_ImGuiId;
			int m_RefCount = 1;
		};

		struct PendingTexture
		{
			std::string m_Name;
			fs::path m_FilePath;

			std::vector<uint8_t> m_TexData;
			size_t m_DataSize;

			int m_Width;
			int m_Height;

			eTextureLoadType m_LoadType;
			ImTextureID* m_OutId;
		};

		std::mutex m_TextureMutex;
		std::mutex m_PendingTextureMutex;

		std::vector<Texture> m_Textures;
		std::vector<PendingTexture> m_PendingTextures;

		static Renderer& GetInstance()
		{
			static Renderer i{};
			return i;
		}
	};
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
