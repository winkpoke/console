#ifndef CONSOLE_UI_APP_H
#define CONSOLE_UI_APP_H

#include "def.h"
#include "ui/image.h"

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

        // Reconstruction 
        resolution_t resolution;
        float slice_dist;

        // Window
        struct window_t* win;
    };
    bool render_maintenance_window(app_t* app);
    bool render_image_window(app_t* app);
    bool render_patient_info_window(app_t* app);
    bool render_status_window(app_t* app);

    bool init(app_t* app);
    
    void drop(app_t* app);

    void run(app_t* app);

    void update(app_t* app);

    bool render_maintenance_window(app_t* app);

    static void HelpMarker(const char* desc);

    bool render_status_window(app_t* app);

    bool render_image_window(app_t* app);


    //bool process_camera_data(app_t* app)
    //{
    //    rs2::pipeline& p = app->camera;
    //    // Block program until frames arrive
    //    rs2::frameset frames = p.wait_for_frames();

    //    // Try to get a frame of a depth image
    //    rs2::depth_frame depth = frames.get_depth_frame();

    //    // Get the depth frame's dimensions
    //    float width = depth.get_width();
    //    float height = depth.get_height();

    //    // Query the distance from the camera to the object in the center of the image
    //    float dist_to_center = depth.get_distance(width / 2, height / 2);

    //    // Print the distance
    //    std::cout << "The camera is facing an object " << dist_to_center << " meters away \r";
    //    // printf("The camera is facing an object %0.4f meters away \r", dist_to_center);

    //    return true;
    //}

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

namespace ui
{
    bool init(app_t* app)
    {
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

        io.Fonts->AddFontDefault();
        // auto image = sil::make_shared<unsigned char>(w, h, 4, img);
        auto image = cl::build_shared<sil::image_t<unsigned char>>(sil::drop, w, h, 4, img);
        init(&g_image_widget[0], 512, 512, image);
        init(&g_image_widget[1], 512, 512, image);
        init(&g_image_widget[2], 512, 512, image);
        init(&g_image_widget[3], 512, 512, image);

        auto patient = cl::build_shared<sil::image_t<cl::u8>>(sil::drop, w1, h1, 4, img1);
        init(&g_image_patient, 50, 73, patient);

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

    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    bool render_status_window(app_t* app)
    {
        static bool show_demo_window = false;

        ImGuiWindowFlags window_flags = 0; // ImGuiWindowFlags_NoMove;
        static bool* p_open;
        ImGui::Begin("##Status", p_open, window_flags);                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("FPD status  ... ");
        ImGui::SameLine();
        ImGui::Text(modal::fpd_status_list[app->fpd]);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("HVG status  ... ");
        ImGui::SameLine();
        ImGui::Text(modal::hvg_status_list[app->hvg]);

        ImGui::SameLine();
        if (ImGui::Button("Connect")) {
            app->fpd = (fpd_status_t)((app->fpd + 1) % 4);
        }

        ImGui::Separator();
        ImGui::Text("CBCT Parameters:");
        ImGui::AlignTextToFramePadding();
        ImGui::SliderFloat("##kV", &app->kv, 50.0f, 100.0f, "%.1f");
        ImGui::SameLine(0, 5);
        if (ImGui::Button("-")) {
            app->kv -= 0.1;
        }
        ImGui::SameLine(0, 5);
        if (ImGui::Button("+")) {
            app->kv += 0.1;
        }
        ImGui::SameLine(); HelpMarker("50.0 ~ 100.0 kV");
        ImGui::SliderFloat("mAs", &app->mAs, 0.0f, 10.0f, "%.1f");

        ImGui::Combo("CBCT Mode", (int*)&app->cbct_mode,
            modal::cbct_mode_list, IM_ARRAYSIZE(modal::cbct_mode_list));

        const char* traj_items[] = { "Full", "Half" };
        static int traj_item_current = 1; // If the selection isn't within 0..count, Combo won't display a preview
        ImGui::Combo("Trajectory", &traj_item_current, traj_items, IM_ARRAYSIZE(traj_items));

        ImGui::Separator();
        ImGui::Text("Reconstruction:");

        ImGui::Combo("Reconstruction Volume", (int*)&app->resolution,
            modal::resolution_list, IM_ARRAYSIZE(modal::resolution_list));

        ImGui::SliderFloat("Slice Distance", &app->slice_dist, 1.0f, 5.0f);
        ImGui::Separator();
        ImGui::NewLine();
        ImGui::NewLine();

        //ImGui::Text("Control:");
        ImGui::SameLine(50);
        ImGui::Button("Setup Patient", ImVec2(100, 50));
        ImGui::SameLine(180);
        static ImGuiButtonFlags exposure_button_flag = ImGuiButtonFlags_Disabled;
        if (control::is_exposure_ready()) {
            exposure_button_flag &= ~ImGuiButtonFlags_Disabled;
        }
        if (ImGui::ButtonEx("Exposure", ImVec2(100, 50), exposure_button_flag)) {
            control::exposure();
        }

        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::NewLine();
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        return true;
    }
}
#endif // !CONSOLE_UI_APP_IMPLEMENTED
#endif // CONSOLE_UI_APP_IMPLEMENTATION
