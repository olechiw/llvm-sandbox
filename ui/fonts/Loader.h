//
// Created by jolechiw on 6/29/23.
//

#ifndef TESTPROJECT_LOADER_H
#define TESTPROJECT_LOADER_H

#include "IconsFontAwesome5.h"
#include "IconsFontAwesome5.h_fa-regular-400.ttf.h"
#include "imgui.h"

namespace Fonts {
    static constexpr float FontSize = 13.f;
    inline void loadFonts() {
        ImGui::GetIO().Fonts->AddFontDefault();
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
        const float iconSize = FontSize * 2.f / 3.f;
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        icons_config.GlyphMinAdvanceX = iconSize;
        icons_config.
        ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)s_fa_regular_400_ttf, sizeof(s_fa_regular_400_ttf), iconSize, &icons_config, icons_ranges);
    }
}

#endif //TESTPROJECT_LOADER_H
