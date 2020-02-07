#ifndef CONSOLE_UI_APP_H
#define CONSOLE_UI_APP_H

#include "def.h"
#include "modal/patient.hxx"
#include "ui/image.h"
#include "stb_image.h"

namespace ui {
    static int w, h;
    static unsigned char* img = stbi_load("resources\\images\\img.jpg", &w, &h, NULL, 4);

    static int w1, h1;
    static unsigned char* img1 = stbi_load("resources\\images\\patient.png", &w1, &h1, NULL, 4);

    struct app_t {
        // FPD 
        fpd_status_t fpd;

        // HVG 
        hvg_status_t hvg;
        float kv;
        float mAs;
        cbct_mode_t cbct_mode;

        // Patient
        modal::patient_t* patient;

        // Reconstruction 
        resolution_t resolution;
        float slice_dist;

        // Window
        struct window_t* win;
    };

    bool init(app_t* app);
    
    void drop(app_t* app);

    void run(app_t* app);

    void update(app_t* app);
} //namespace ui

#endif // CONSOLE_UI_APP_H

#ifdef CONSOLE_UI_APP_IMPLEMENTATION
#ifndef CONSOLE_UI_APP_IMPLEMENTED
#define CONSOLE_UI_APP_IMPLEMENTED

#include "ui/window.h"
#include "ui/log.h"
#include "modal/modal.h"
#include "control/control.h"

#include "renders/render_image.hxx"
#include "renders/render_patient_info.hxx"
#include "renders/render_maintenance.hxx"
#include "renders/render_status.hxx"

namespace ui
{
    bool init(app_t* app)
    {
        assert(app);

        app->patient = cl::build_raw<modal::patient_t>();
        // auto p = app->patient;
        // assert(p);
        auto p = cl::build_raw<modal::patient_t>();
        strncpy(p->name, u8"����", sizeof(p->name));
        strncpy(p->id, "209845", sizeof(p->id));
        p->age = 65;
        p->gender = modal::gender_e::MALE;
        strncpy(p->category, u8"H&N", sizeof(p->category));
        strncpy(p->site, "Hospital 1", sizeof(p->site));
        p->portrait = cl::build_raw<sil::image_t<cl::u8>>(w1, h1, 4, img1);

        auto j = modal::to_json(p);
        modal::from_json(app->patient, j);


        app->fpd = modal::g_app_stat.fpd;
        app->hvg = modal::g_app_stat.hvg;
        app->kv = modal::g_app_stat.kv;
        app->mAs = modal::g_app_stat.mAs;
        app->cbct_mode = modal::g_app_stat.cbct_mode;
        app->resolution = modal::g_app_stat.resolution;
        app->slice_dist = modal::g_app_stat.slice_dist;

        app->win = cl::build_raw<window_t>(300, 300, 1024, 768);
        if (!app->win) {
            return false;
        }

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        //io.Fonts->GetGlyphRangesChineseFull());// 
        auto font0 = io.Fonts->AddFontDefault();
        assert(font0);
        ImFontConfig config;
        config.MergeMode = true;
        //config.OversampleH = 3;
        //config.OversampleV = 2;
        //config.PixelSnapH = true;
        config.RasterizerMultiply = 2.0f;
        auto font1 = io.Fonts->AddFontFromFileTTF("resources\\fonts\\simyou.ttf", 13, &config, io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
        assert(font1);
        io.Fonts->Build();

        // assert(font1);
        // auto image = sil::make_shared<unsigned char>(w, h, 4, img);
        auto image = cl::build_shared<sil::image_t<unsigned char>>(sil::drop, w, h, 4, img);
        init(&g_image_widget[0], 512, 512, image);
        init(&g_image_widget[1], 512, 512, image);
        init(&g_image_widget[2], 512, 512, image);
        init(&g_image_widget[3], 512, 512, image);

        // key events
        app->win->key_events.push_back([](window_t* win, int key) -> bool {
            if (ImGui::IsKeyPressed(key) && key == 0x12B) {
                static bool toggle_fullscreen = true;
                set_fullscreen(win, toggle_fullscreen);
                toggle_fullscreen = !toggle_fullscreen;
            }
            return true;
            });

        // renders
        app->win->renders.push_back([=](window_t*) {
            render_status_window(app);
            render_image_window(app);
            render_patient_info_window(app);
            render_maintenance_window(app);
            //win->renders.push_back(&ui::process_camera_data);
            return true;
            });

        return true;
    }

    void drop(app_t* app)
    {
        if (app && app->win) {
            drop(app->win);
        }
    }

    void run(app_t* app)
    {
        // Main loop
        while (!is_close(app->win))
        {
            new_frame(app->win);

            render(app->win);

            process_event(app->win);
            draw(app->win);
        }
    }

    void update(app_t* app)
    {

    }
}
#endif // !CONSOLE_UI_APP_IMPLEMENTED
#endif // CONSOLE_UI_APP_IMPLEMENTATION
