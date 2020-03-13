#ifndef CONSOLE_UI_APP_H
#define CONSOLE_UI_APP_H

#include <mutex>
#include <shared_mutex>

//#include "stb_image.h"

//#include "def.h"
#include "modal/modal.h"
#include "control/runtime_data.hxx"
#include "ui/image.h"
#include "mod/fpd/control/fpd.hxx"
#include "mod/hvg/control/hvg.hxx"


namespace ui {
    static int w, h;
    //static unsigned char* img = stbi_load("resources\\images\\img.jpg", &w, &h, NULL, 4);

    struct app_t {
        cl::runtime_object_t* objects;
        // Window
        struct window_t* win;
    };

    bool init(app_t* app);
    
    void drop(app_t* app);

    void run(app_t* app);

    void update(app_t* app, control::runtime_data_t* data);
    void update(control::runtime_data_t* data, app_t* app);
} //namespace ui

#endif // CONSOLE_UI_APP_H

#ifdef CONSOLE_UI_APP_IMPLEMENTATION
#ifndef CONSOLE_UI_APP_IMPLEMENTED
#define CONSOLE_UI_APP_IMPLEMENTED

#include <filesystem>

#include "cl.h"
#include "ui/window.h"
#include "ui/log.h"
#include "modal/modal.h"
#include "control/control.hxx"
#include "image.h"

#include "renders/render_maintenance.hxx"

#include "mod/cbct/ui.hxx"
#include "mod/patient/ui.hxx"

namespace ui
{
    static void set_window_style(app_t* app)
    {
        assert(app);
        auto win = app->win;
        assert(win);

        ImGuiStyle& style = ImGui::GetStyle();
        style.FramePadding = ImVec2(11.0f, 8.f);
        style.ItemInnerSpacing[0] = 11.f;
        style.GrabMinSize = 20.f;
        style.WindowRounding = 0.f;
        style.FrameRounding = 3.f;
        style.ScrollbarRounding = 3.f;
    }

    bool init(app_t* app)
    {
        SPDLOG_TRACE("Staring initialization ...");
        assert(app);

        app->objects = cl::build_raw<cl::runtime_object_t>();
        app->win = cl::build_raw<window_t>(300, 300, 1024, 768);
        if (!app->win) {
            return false;
        }

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        //ImGui::StyleColorsDark();
        ImGui::StyleColorsClassic();
        //ImGui::StyleColorsLight();

        //io.Fonts->GetGlyphRangesChineseFull());// 
        auto font0 = io.Fonts->AddFontDefault();
        //const char font_path[256] = "resources\\fonts\\fontawesome-webfont.ttf";
        //const char font_path[256] = "resources\\fonts\\AdventPro-Regular.ttf";
        //auto font0 = io.Fonts->AddFontFromFileTTF(font_path, 20);
        assert(font0);
        ImFontConfig config;
        config.MergeMode = true;
        //config.OversampleH = 3;
        //config.OversampleV = 2;
        //config.PixelSnapH = true;
        config.RasterizerMultiply = 2.0f;

        // Load chinese font into memory and merge with other loaded fonts
        const char chinese_font_path[256] = "resources\\fonts\\simyou.ttf";
        if (std::filesystem::exists(chinese_font_path)) {
            auto font1 = io.Fonts->AddFontFromFileTTF(chinese_font_path, 13, &config, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
            assert(font1);
            if (font1) io.Fonts->Build();
        }

        // ui::patient has to be initialized after window's created
        auto control_runtime = control::get_runtime_data();
        auto obj = cl::get<mod::patient::control::patient_t>(control_runtime->objects, "patient");
        auto patient = cl::build_shared<mod::patient::ui::patient_t>(obj.get());
        cl::mount(app->objects, patient, "patient", "0.0.1");

        auto cbct_control = cl::get<mod::cbct::control::cbct_dummy_t>(control_runtime->objects, "cbct");
        using cbct_ui_t = mod::cbct::ui::cbct_t;
        using cbct_control_t = mod::cbct::control::cbct_t;

        auto cbct_ui = cl::build_shared<mod::cbct::ui::cbct_t>(cbct_control.get());
        cl::mount(app->objects, cbct_ui, "cbct", "0.0.1");

        //auto j = modal::to_json(p);
        //modal::from_json(app->patient, j);

        //FILE* fp;
        //fp = fopen("D:\\patient.json", "w+");
        //fprintf_s(fp, "%s", j.c_str());
        //fclose(fp);

        //auto image = cl::build_raw<sil::image_t<cl::u8>>(w, h, 4, img);
        //app->image_demo = cl::build_raw<ui::image_view<cl::u8>>(512, 512, image);

        auto data = control::get_runtime_data();
        //auto fpd = cl::get<mod::fpd::control::fpd_t>(data->objects, "fpd");
        //assert(fpd);


        // key events
        app->win->key_events.push_back([](window_t* win, int key) -> bool {
            if (ImGui::IsKeyReleased(key) && key == 0x12B) {
                static bool toggle_fullscreen = true;
                set_fullscreen(win, toggle_fullscreen);
                toggle_fullscreen = !toggle_fullscreen;
            }
            return true;
            });

        // renders
        app->win->renders.push_back([=](window_t*) {
            auto patient = cl::get<mod::patient::ui::patient_t>(app->objects, "patient");
            mod::patient::ui::render(patient.get());

            auto cbct = cl::get<mod::cbct::ui::cbct_t>(app->objects, "cbct");
            mod::cbct::ui::render(cbct.get());

            renders::render_maintenance_window(app);
            return true;
            });

        // set style
        set_window_style(app);

        SPDLOG_TRACE("End of initialization ...");
        return true;
    }

    void drop(app_t* app)
    {
        if (app) {
            if (app->objects) {
                cl::unmount_all(app->objects);
            }

            cl::recycle(app->win);
            cl::recycle(app->objects);
        }
    }

    void update(app_t* app, control::runtime_data_t* data)
    {
        std::shared_lock lk(data->mutex);
        mod::patient::ui::update(app->objects, data->objects);
        mod::cbct::ui::update(app->objects, data->objects);
    }

    void run(app_t* app)
    {
        // Main loop
        while (!is_close(app->win)) {
            new_frame(app->win);
            update(app, control::get_runtime_data());
            render(app->win);
            process_event(app->win);
            draw(app->win);
        }
    }
}
#endif // !CONSOLE_UI_APP_IMPLEMENTED
#endif // CONSOLE_UI_APP_IMPLEMENTATION
