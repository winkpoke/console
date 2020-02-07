#ifndef CONSOLE_INCLUDE_UI_H 
#define CONSOLE_INCLUDE_UI_H

#include <memory>

#include "cl.h"
#include "ui/image.h"

namespace window {
    struct window_t;
}

namespace ui
{
    bool init();
    void render(window::window_t* window);
    void drop();
}

#endif // CONSOLE_INCLUDE_UI_H

#ifdef CONSOLE_UI_IMPLEMENTATION
#ifndef CONSOLE_UI_IMPLEMENTED
#define CONSOLE_UI_IMPLEMENTED

#include "control/control.h"

#include "window.h"

#include "ui/app.hxx"

namespace ui
{


    bool init(ui::window_t* win)
    {
    //    if (!win) {
    //        return false;
    //    }

    //    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    //    // Setup Dear ImGui style
    //    ImGui::StyleColorsDark();
    //    //ImGui::StyleColorsClassic();

    //    io.Fonts->AddFontDefault();
    //    //io.Fonts->AddFontFromFileTTF("src/ThirdParty/imgui/misc/fonts/Roboto-Medium.ttf", 14.0f);
    //    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //    //IM_ASSERT(font != NULL);

    //    // auto image = sil::make_shared<unsigned char>(w, h, 4, img);
    //    auto image = cl::build_shared<sil::image_t<unsigned char>>(sil::drop, w, h, 4, img);
    //    init(&g_image_widget[0], 512, 512, image);
    //    init(&g_image_widget[1], 512, 512, image);
    //    init(&g_image_widget[2], 512, 512, image);
    //    init(&g_image_widget[3], 512, 512, image);

    //    auto patient = cl::build_shared<sil::image_t<cl::u8>>(sil::drop, w1, h1, 4, img1);
    //    init(&g_image_patient, 50, 73, patient);

        return true;
    }

    //void render(window::window_t* win)
    //{
    //    //render(win);
    //}

    void drop()
    {}
}
#endif // !CONSOLE_UI_IMPLEMENTED
#endif   // CONSOLE_UI_IMPLEMENTATION
