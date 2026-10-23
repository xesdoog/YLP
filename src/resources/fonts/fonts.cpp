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


#if defined(__GNUC__) || defined(__clang__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wnull-character"
#elif defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable : 4820)
#endif


#include "fonts.hpp"
#include "JetBrainsMono.hpp"
#include "JetBrainsMonoBold.hpp"
#include "MaterialSymbolsStripped.hpp"

#if defined(__GNUC__) || defined(__clang__)
	#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
	#pragma warning(pop)
#endif


ImFont* Fonts::Small    = nullptr;
ImFont* Fonts::Regular  = nullptr;
ImFont* Fonts::Bold     = nullptr;
ImFont* Fonts::Title    = nullptr;
ImFont* Fonts::Subtitle = nullptr;
ImFont* Fonts::IconsBig = nullptr;

static void MergeIcons(ImGuiIO& io, float size)
{
	ImFontConfig icon_cfg;
	icon_cfg.MergeMode        = true;
	icon_cfg.PixelSnapH       = true;
	icon_cfg.GlyphMinAdvanceX = 16.0f;
	icon_cfg.GlyphOffset      = ImVec2(1, 3);

	strcpy(icon_cfg.Name, "MaterialDesignSymbols");
	static const ImWchar icons_range[] = {ICON_MIN_MS, ICON_MAX_MS, 0};

	io.Fonts->AddFontFromMemoryCompressedTTF(
	    material_symbols_compressed_data,
	    material_symbols_compressed_size,
	    size,
	    &icon_cfg,
	    icons_range);
}

void Fonts::Load(ImGuiIO& io)
{
	ImFontConfig cfg;
	cfg.OversampleH = 2;
	cfg.OversampleV = 2;
	cfg.PixelSnapH  = true;

	auto& fonts = io.Fonts;
	fonts->Clear();

	strcpy(cfg.Name, "JetBrainsMono Default");
	Regular = fonts->AddFontFromMemoryCompressedTTF(jbm_data, jbm_size, 19.0f, &cfg);
	MergeIcons(io, 21.f);

	strcpy(cfg.Name, "JetBrainsMono Bold");
	Bold = fonts->AddFontFromMemoryCompressedTTF(jbmb_data, jbmb_size, 19.0f, &cfg);
	MergeIcons(io, 21.f);

	strcpy(cfg.Name, "JetBrainsMono Small");
	Small = fonts->AddFontFromMemoryCompressedTTF(jbm_data, jbm_size, 15.0f, &cfg);
	MergeIcons(io, 17.0f);

	strcpy(cfg.Name, "JetBrainsMono Subtitle");
	Subtitle = fonts->AddFontFromMemoryCompressedTTF(jbm_data, jbm_size, 20.0f, &cfg);
	MergeIcons(io, 22.0f);

	strcpy(cfg.Name, "JetBrainsMono Title");
	Title = fonts->AddFontFromMemoryCompressedTTF(jbmb_data, jbmb_size, 25.0f, &cfg);
	MergeIcons(io, 27.0f);

	strcpy(cfg.Name, "Icons Big");
	IconsBig = fonts->AddFontFromMemoryCompressedTTF(jbmb_data, jbmb_size, 25.0f, &cfg);
	MergeIcons(io, 32.0f);

	io.FontDefault = Regular;
	fonts->AddFontDefault(&cfg);
	fonts->Build();
}
