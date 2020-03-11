#ifndef CONSOLE_UI_APP_H
#define CONSOLE_UI_APP_H

#include <mutex>
#include <shared_mutex>

#include "stb_image.h"

#include "def.h"
#include "mod/patient/ui.hxx"
#include "modal/modal.h"
#include "control/runtime_data.hxx"
#include "ui/image.h"
#include "mod/fpd/control/fpd.hxx"
#include "mod/hvg/control/hvg.hxx"


namespace ui {
    static int w, h;
    static unsigned char* img = stbi_load("resources\\images\\img.jpg", &w, &h, NULL, 4);

    struct app_t {
        mod::fpd::control::status_e fpd_status;
        mod::hvg::control::status_e hvg_status;
        float kv;
        float mAs;
        cbct_mode_t cbct_mode;

        cl::runtime_object_t* objects;

        cl::i64 index;
        ui::image_view<cl::u16>* image0;
        ui::image_view<cl::u16>* image3;
        ui::image_view<cl::u16>* image1;
        ui::image_view<cl::u16>* image2;
        ui::image_view<cl::u8>* image_demo;

        // Reconstruction 
        resolution_t resolution;
        float slice_dist;

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

#include "renders/render_image.hxx"
#include "renders/render_maintenance.hxx"
#include "renders/render_status.hxx"

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
        //auto j = modal::to_json(p);
        //modal::from_json(app->patient, j);

        //FILE* fp;
        //fp = fopen("D:\\patient.json", "w+");
        //fprintf_s(fp, "%s", j.c_str());
        //fclose(fp);

        auto image = cl::build_raw<sil::image_t<cl::u8>>(w, h, 4, img);
        app->image_demo = cl::build_raw<ui::image_view<cl::u8>>(512, 512, image);

        auto data = control::get_runtime_data();
        auto fpd = cl::get<mod::fpd::control::fpd_t>(data->objects, "fpd");
        assert(fpd);

        app->index = -1;

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
            renders::render_status_window(app);
            renders::render_image_window(app);
            renders::render_maintenance_window(app);
            //win->renders.push_back(&ui::process_camera_data);
            auto patient = cl::get<mod::patient::ui::patient_t>(app->objects, "patient");
            mod::patient::ui::render(patient.get());
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
            recycle(app->image0);
            recycle(app->image1);
            recycle(app->image2);
            recycle(app->image3);
            cl::recycle(app->image_demo);
        }
    }

    void update(app_t* app, control::runtime_data_t* data)
    {
        std::shared_lock lk(data->mutex);
        
        auto fpd = cl::get<mod::fpd::control::fpd_t>(data->objects, "fpd");
        assert(fpd);
        auto hvg = cl::get<mod::hvg::control::hvg_t>(data->objects, "hvg");
        assert(hvg);

        auto patient = cl::get<mod::patient::control::patient_t>(data->objects, "patient");
        assert(patient);

        auto dummy_fpd = cl::get<mod::fpd::control::fpd_dummy_t>(data->objects, "fpd_dummy");
        assert(dummy_fpd);

        mod::patient::ui::update_ui_data(app->objects, data->objects);

        //app->fpd_status = fpd->status;
        app->fpd_status = dummy_fpd->fpd->status;
        app->hvg_status = hvg->status;
        app->kv = hvg->kv;
        app->mAs = hvg->mAs;
        app->cbct_mode = data->cbct_mode;
        app->resolution = data->resolution;
        app->slice_dist = data->slice_dist;

        const cl::usize index = len(dummy_fpd->fpd->scan);
        if (app->index != index) {
            auto old_image = app->image0;
            assert(index < 360 && index >= 0);
            auto p = modal::get_data_at(dummy_fpd->fpd->scan, index);
            const cl::usize len = 1024 * 1024;
            static auto pnew = cl::alloc<cl::u16>(len);
            memcpy(pnew, p, len * sizeof(cl::u16));
            static auto img = cl::build_raw<sil::image_t<cl::u16>>(1024, 1024, 1, pnew);
            if (!app->image0) {
                app->image0 = cl::build_raw<ui::image_view<cl::u16>>(512, 512, img);
            }
            else {
                init(app->image0, 512, 512, img);
            }

            switch (index) {
            case 90:
                recycle(app->image1);
                app->image1 = clone(old_image);
                break;
            case 180:
                recycle(app->image2);
                app->image2 = clone(old_image);
                break;
            case 270:
                recycle(app->image3);
                app->image3 = clone(old_image);
                break;
            //default:
            //    cl::dealloc(old_image);
            }
            app->index = index;
        }
    }

    void update(control::runtime_data_t* data, app_t* app)
    {
        //std::unique_lock(data->mutex);

        //auto fpd = cl::get<control::fpd::fpd_t>(data->objects, "fpd");
        //assert(fpd);

        //auto hvg = cl::get<control::hvg::hvg_t>(data->objects, "hvg");
        //assert(hvg);

        data->cbct_mode = app->cbct_mode;
        data->resolution = app->resolution;
        data->slice_dist = app->slice_dist;
    }

    void run(app_t* app)
    {
        // Main loop
        while (!is_close(app->win))
        {
            new_frame(app->win);

            update(app, control::get_runtime_data());

            render(app->win);

            process_event(app->win);
            draw(app->win);

            update(control::get_runtime_data(), app);
        }
    }
}
#endif // !CONSOLE_UI_APP_IMPLEMENTED
#endif // CONSOLE_UI_APP_IMPLEMENTATION
