#ifndef CONSOLE_INCLUDE_UI_H 
#define CONSOLE_INCLUDE_UI_H

#include <memory>

#include "cl.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "rs232.h"

// About OpenGL function loaders: modern OpenGL doesn't have a standard header file and requires individual function pointers to be loaded manually.
// Helper libraries are often used for this purpose! Here we are supporting a few common ones: gl3w, glew, glad.
// You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>    // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>    // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>  // Initialize with gladLoadGL()
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

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

#define CONSOLE_DATA_IMPLEMENTATION
#define CONSOLE_MODAL_IMPLEMENTATION
#include "modal.h"

#define WINDOW_IMPLEMENTATION
#include "window.h"

namespace ui
{
    static int w, h;
    static unsigned char* img = stbi_load("resources\\images\\img.jpg", &w, &h, NULL, 4);

    static int w1, h1;
    static unsigned char* img1 = stbi_load("resources\\images\\patient.png", &w1, &h1, NULL, 4);

    image::image_view<unsigned char> g_image_widget[4];

    image::image_view<cl::u8> g_image_patient;

    //-----------------------------------------------------------------------------
// [SECTION] Example App: Debug Log / ShowExampleAppLog()
//-----------------------------------------------------------------------------

// Usage:
//  static ExampleAppLog my_log;
//  my_log.AddLog("Hello %d world\n", 123);
//  my_log.Draw("title");
    struct ExampleAppLog
    {
        ImGuiTextBuffer     Buf;
        ImGuiTextFilter     Filter;
        ImVector<int>       LineOffsets;        // Index to lines offset. We maintain this with AddLog() calls, allowing us to have a random access on lines
        bool                AutoScroll;     // Keep scrolling if already at the bottom

        ExampleAppLog()
        {
            AutoScroll = true;
            Clear();
        }

        void    Clear()
        {
            Buf.clear();
            LineOffsets.clear();
            LineOffsets.push_back(0);
        }

        void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
        {
            int old_size = Buf.size();
            va_list args;
            va_start(args, fmt);
            Buf.appendfv(fmt, args);
            va_end(args);
            for (int new_size = Buf.size(); old_size < new_size; old_size++)
                if (Buf[old_size] == '\n')
                    LineOffsets.push_back(old_size + 1);
        }

        void    Draw(const char* title, bool* p_open = NULL)
        {
            if (!ImGui::Begin(title, p_open))
            {
                ImGui::End();
                return;
            }

            // Options menu
            if (ImGui::BeginPopup("Options"))
            {
                ImGui::Checkbox("Auto-scroll", &AutoScroll);
                ImGui::EndPopup();
            }

            // Main window
            if (ImGui::Button("Options"))
                ImGui::OpenPopup("Options");
            ImGui::SameLine();
            bool clear = ImGui::Button("Clear");
            ImGui::SameLine();
            bool copy = ImGui::Button("Copy");
            ImGui::SameLine();
            Filter.Draw("Filter", -100.0f);

            ImGui::Separator();
            ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

            if (clear)
                Clear();
            if (copy)
                ImGui::LogToClipboard();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const char* buf = Buf.begin();
            const char* buf_end = Buf.end();
            if (Filter.IsActive())
            {
                // In this example we don't use the clipper when Filter is enabled.
                // This is because we don't have a random access on the result on our filter.
                // A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
                // especially if the filtering function is not trivial (e.g. reg-exp).
                for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    if (Filter.PassFilter(line_start, line_end))
                        ImGui::TextUnformatted(line_start, line_end);
                }
            }
            else
            {
                // The simplest and easy way to display the entire buffer:
                //   ImGui::TextUnformatted(buf_begin, buf_end);
                // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward to skip non-visible lines.
                // Here we instead demonstrate using the clipper to only process lines that are within the visible area.
                // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them on your side is recommended.
                // Using ImGuiListClipper requires A) random access into your data, and B) items all being the  same height,
                // both of which we can handle since we an array pointing to the beginning of each line of text.
                // When using the filter (in the block of code above) we don't have random access into the data to display anymore, which is why we don't use the clipper.
                // Storing or skimming through the search result would make it possible (and would be recommended if you want to search through tens of thousands of entries)
                ImGuiListClipper clipper;
                clipper.Begin(LineOffsets.Size);
                while (clipper.Step())
                {
                    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                    {
                        const char* line_start = buf + LineOffsets[line_no];
                        const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                        ImGui::TextUnformatted(line_start, line_end);
                    }
                }
                clipper.End();
            }
            ImGui::PopStyleVar();

            if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);

            ImGui::EndChild();
            ImGui::End();
        }
    };

    // Demonstrate creating a simple log window with basic filtering.
    bool render_maintenance_window(window::window_t* win)
    {
        static bool open = true;
        static bool* p_open = &open;

        static ExampleAppLog log;

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
                    log.AddLog("received %i bytes: %s\n", d - str - 1, str);
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
            log.AddLog("HVG Command: %s\n", buf);
            static char send_buf[4096];
            sprintf(send_buf, "%s\r\n", buf);
            RS232_cputs(port, send_buf);
        }

        ImGui::End();

        // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
        log.Draw(hvg_command_window_title, p_open);

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

    bool render_status_window(window::window_t* win)
    {
        static bool show_demo_window = false;

        ImGuiWindowFlags window_flags = 0; // ImGuiWindowFlags_NoMove;
        static bool* p_open;
        ImGui::Begin("##Status", p_open, window_flags);                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("FPD status  ... ");
        ImGui::SameLine();
        ImGui::Text(data::fpd_status_list[data::g_app_stat.fpd]);

        ImGui::AlignTextToFramePadding();
        ImGui::Text("HVG status  ... ");
        ImGui::SameLine();
        ImGui::Text(data::hvg_status_list[data::g_app_stat.hvg]);

        ImGui::SameLine();
        if (ImGui::Button("Connect")) {
            data::g_app_stat.fpd = (fpd_status_t)((data::g_app_stat.fpd + 1) % 4);
        }

        ImGui::Separator();
        ImGui::Text("CBCT Parameters:");
        ImGui::AlignTextToFramePadding();
        ImGui::SliderFloat("##kV", &data::g_app_stat.kv, 50.0f, 100.0f, "%.1f");
        ImGui::SameLine(0, 5);
        if (ImGui::Button("-")) {
            data::g_app_stat.kv -= 0.1;
        }
        ImGui::SameLine(0, 5);
        if (ImGui::Button("+")) {
            data::g_app_stat.kv += 0.1;
        }
        ImGui::SameLine(); HelpMarker("50.0 ~ 100.0 kV");
        ImGui::SliderFloat("mAs", &data::g_app_stat.mAs, 0.0f, 10.0f, "%.1f");

        ImGui::Combo("CBCT Mode", (int*)&data::g_app_stat.cbct_mode,
            data::cbct_mode_list, IM_ARRAYSIZE(data::cbct_mode_list));

        const char* traj_items[] = { "Full", "Half" };
        static int traj_item_current = 1; // If the selection isn't within 0..count, Combo won't display a preview
        ImGui::Combo("Trajectory", &traj_item_current, traj_items, IM_ARRAYSIZE(traj_items));

        ImGui::Separator();
        ImGui::Text("Reconstruction:");

        ImGui::Combo("Reconstruction Volume", (int*)&data::g_app_stat.resolution,
            data::resolution_list, IM_ARRAYSIZE(data::resolution_list));

        ImGui::SliderFloat("Slice Distance", &data::g_app_stat.slice_dist, 1.0f, 5.0f);
        ImGui::Separator();
        ImGui::NewLine();
        ImGui::NewLine();

        //ImGui::Text("Control:");
        ImGui::SameLine(50);
        ImGui::Button("Setup Patient", ImVec2(100, 50));
        ImGui::SameLine(180);
        static ImGuiButtonFlags exposure_button_flag = ImGuiButtonFlags_Disabled;
        if (modal::is_exposure_ready()) {
            exposure_button_flag &= ~ImGuiButtonFlags_Disabled;
        }
        if (ImGui::ButtonEx("Exposure", ImVec2(100, 50), exposure_button_flag)) {
            modal::exposure();
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

    bool render_image_window(window::window_t* win)
    {
        static bool p_open;
        ImGui::Begin("##Image", &p_open, ImGuiWindowFlags_NoTitleBar);

        image::render(&g_image_widget[0]);
        ImGui::SameLine();
        image::render(&g_image_widget[1]);
        image::render(&g_image_widget[2]);
        ImGui::SameLine();
        image::render(&g_image_widget[3]);
        ImGui::End();
        return true;
    }

    bool render_patient_info_window(window::window_t* win)
    {
        static bool p_open;
        ImGui::Begin("##patient_info", &p_open, ImGuiWindowFlags_NoTitleBar);
        ImGui::Columns(3, 0, false);
        ImGui::SetColumnWidth(0, 80);
        image::render(&g_image_patient); 
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

    bool process_camera_data(window::window_t* win)
    {
        rs2::pipeline& p = data::g_app_stat.camera;
        // Block program until frames arrive
        rs2::frameset frames = p.wait_for_frames();

        // Try to get a frame of a depth image
        rs2::depth_frame depth = frames.get_depth_frame();

        // Get the depth frame's dimensions
        float width = depth.get_width();
        float height = depth.get_height();

        // Query the distance from the camera to the object in the center of the image
        float dist_to_center = depth.get_distance(width / 2, height / 2);

        // Print the distance
        std::cout << "The camera is facing an object " << dist_to_center << " meters away \r";

        return true;
    }

    bool init(window::window_t* win)
    {
        if (!win) {
            return false;
        }

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("src/ThirdParty/imgui/misc/fonts/Roboto-Medium.ttf", 14.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);

        // auto image = sil::make_shared<unsigned char>(w, h, 4, img);
        auto image = cl::build_shared<sil::image_t<unsigned char>>(w, h, 4, img);
        image::image_view<unsigned char>::init(&g_image_widget[0], 512, 512, image);
        image::image_view<unsigned char>::init(&g_image_widget[1], 512, 512, image);
        image::image_view<unsigned char>::init(&g_image_widget[2], 512, 512, image);
        image::image_view<unsigned char>::init(&g_image_widget[3], 512, 512, image);

        auto patient = cl::build_shared<sil::image_t<cl::u8>>(w1, h1, 4, img1);
        image::image_view<cl::u8>::init(&g_image_patient, 50, 73, patient);

        return true;
    }


    void render(window::window_t* win)
    {
        window::render(win);
    }

    void drop()
    {}
}
#endif   // CONSOLE_UI_IMPLEMENTATION
