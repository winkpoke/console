#ifndef CONSOLE_UI_APP_H
#define CONSOLE_UI_APP_H

namespace ui {
    static int w, h;
    static unsigned char* img = stbi_load("resources\\images\\img.jpg", &w, &h, NULL, 4);

    static int w1, h1;
    static unsigned char* img1 = stbi_load("resources\\images\\patient.png", &w1, &h1, NULL, 4);

    image_view<unsigned char> g_image_widget[4];

    image_view<cl::u8> g_image_patient;

    //enum error_t {
    //    OK,
    //    ERR_STATUS
    //};

    //enum fpd_status_t {
    //    FPD_UNCONNECTED,
    //    FPD_CONNECTING,
    //    FPD_READY,
    //    FPD_ERROR
    //};

    //enum hvg_status_t {
    //    HVG_UNCONNECTED,
    //    HVG_CONNECTING,
    //    HVG_READY,
    //    HVG_EXPOSURE,
    //    HVG_ERROR
    //};

    //enum cbct_mode_t {
    //    HEAD,
    //    LUNG,
    //    ABDOMINAL,
    //    CUSTOM
    //};

    //enum resolution_t {
    //    _128X128,
    //    _256X256,
    //    _384X384,
    //    _512X512,
    //    _768X768
    //};


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
        render(app->win);
    }

    void update(app_t* app)
    {

    }

    bool render_maintenance_window(app_t* app)
    {
        static bool open = true;
        static bool* p_open = &open;

        static auto log = cl::build_unique<log_t>(drop);

        static int port = 2;
        static unsigned char sbuf[4096];
        int n = RS232_PollComport(port, sbuf, 4095);
        static char str[4096];
        if (n > 0) {
            sbuf[n] = 0;   /* always put a "null" at the end of a string! */
            char* s = (char*)sbuf;
            static char* d = str;
            while (n-- > 0) {
                if (*s == '\n' && *(d - 1) == '\r') {
                    *(d - 1) = 0;
                    ui::add_log(log.get(), "received %i bytes: %s\n", d - str - 1, str);
                    d = str;
                    s++;
                }
                else {
                    *d++ = *s++;
                }
            }
        }

        // For the demo: add a debug button _BEFORE_ the normal log window contents
        // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
        // Most of the contents of the window will be added by the log.Draw() call.
        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        static char hvg_command_window_title[256] = "HVG Command";
        ImGui::Begin(hvg_command_window_title, p_open);
        const char* items[] = { "ES3", "IFV" };
        static const char* item_current = items[0];
        static int item_current_3 = -1; // If the selection isn't within 0..count, Combo won't display a preview
        ImGui::Combo("Predefined commands", &item_current_3, items, IM_ARRAYSIZE(items));

        static char buf[256] = "";
        ImGui::InputText("HVG Command", buf, 256);

        if (ImGui::Button("Send")) {
            ui::add_log(log.get(), "HVG Command: %s\n", buf);
            static char send_buf[4096];
            sprintf(send_buf, "%s\r\n", buf);
            RS232_cputs(port, send_buf);
        }

        ImGui::End();

        // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
        ui::draw(log.get(), hvg_command_window_title, p_open);
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

    bool render_image_window(app_t* app)
    {
        static bool p_open;
        ImGui::Begin("##Image", &p_open, ImGuiWindowFlags_NoTitleBar);

        render(&g_image_widget[0]);
        ImGui::SameLine();
        render(&g_image_widget[1]);
        render(&g_image_widget[2]);
        ImGui::SameLine();
        render(&g_image_widget[3]);
        ImGui::End();
        return true;
    }

    bool render_patient_info_window(app_t* app)
    {
        static bool p_open;
        ImGui::Begin("##patient_info", &p_open, ImGuiWindowFlags_NoTitleBar);
        ImGui::Columns(3, 0, false);
        ImGui::SetColumnWidth(0, 80);
        render(&g_image_patient);
        ImGui::NextColumn();
        ImGui::Text("Name:     Zhang San");
        ImGui::Text("ID:       209845");
        ImGui::Text("Age:      65");
        ImGui::NextColumn();
        ImGui::Text("Gender:   M");
        ImGui::Text("Category: H&N");
        ImGui::Text("Site:     Hospital 1");
        ImGui::Columns(1);

        ImGui::End();
        return true;
    }

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