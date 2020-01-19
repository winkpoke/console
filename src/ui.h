#ifndef CONSOLE_INCLUDE_UI_H 
#define CONSOLE_INCLUDE_UI_H

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

namespace ui
{
    GLFWwindow* init();
    void render(GLFWwindow* window);
    void drop(GLFWwindow* window);
}

#endif // CONSOLE_INCLUDE_UI_H

#ifdef CONSOLE_UI_IMPLEMENTATION

#define CONSOLE_DATA_IMPLEMENTATION
#define CONSOLE_MODAL_IMPLEMENTATION
#include "modal.h"
namespace ui
{
    static int w, h;
    static unsigned char* img = stbi_load("resources\\images\\img.jpg", &w, &h, NULL, 4);

    image::mono_image_window<unsigned char> g_image_widget[4];

    // Demonstrate creating a simple log window with basic filtering.
    static void render_maintenance_window(bool* p_open)
    {
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

    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    GLFWwindow* init()
    {
        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return NULL;

        // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
        const char* glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
        const char* glsl_version = "#version 130";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
        GLFWwindow* window = glfwCreateWindow(1280, 720, "Techtron CBCT Console", NULL, NULL);
        //GLFWwindow* window = glfwCreateWindow(1920, 1080, "Dear ImGui GLFW+OpenGL3 example", glfwGetPrimaryMonitor(), NULL);
        if (window == NULL)
            return NULL;
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
        bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
        bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
        bool err = gladLoadGL() == 0;
#else
        bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
        if (err)
        {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
            return NULL;
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Load Fonts
        // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
        // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
        // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
        // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
        // - Read 'misc/fonts/README.txt' for more instructions and details.
        // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
        io.Fonts->AddFontDefault();
        //io.Fonts->AddFontFromFileTTF("src/ThirdParty/imgui/misc/fonts/Roboto-Medium.ttf", 14.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
        //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
        //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
        //IM_ASSERT(font != NULL);

        auto image = sil::make_shared<unsigned char>(w, h, 4, img);
        image::init(&g_image_widget[0], 700, 512, image);
        image::init(&g_image_widget[1], 700, 512, image);
        image::init(&g_image_widget[2], 700, 512, image);
        image::init(&g_image_widget[3], 700, 512, image);

        return window;
    }

    static void render_status_window()
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
        //ImGui::Checkbox("Another Window", &show_another_window);

        //ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        //if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        //    counter++;
        //ImGui::SameLine();
        //ImGui::Text("counter = %d", counter);

        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }

    static void render_image_window()
    {
        //static bool p_open;
        //ImGui::Begin("##Image", &p_open, ImGuiWindowFlags_NoTitleBar);
        //// ImGui::Text("Image");
        //// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
        //data::scan::scan_t::pixel_t* image = data::scan::n_image(data::g_app_stat.scan, max(0, data::g_app_stat.scan->index));
        ////data::scan::scan_t::pixel_t* image = data::g_app_stat.scan->images;
        //if (image != NULL) {
        //    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 3072, 3072, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, image);
        //}
        //else {
        //    // error handling
        //}
        //GLenum err = glGetError();
        //if (err != GL_NO_ERROR) {


        //    // error handling
        //}
        //ImGui::Image((void*)(intptr_t)texture, ImVec2(1024, 1024));
        //ImGui::End();
    }

    void render(GLFWwindow* window)
    {

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
        /*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);*/

        // Our state
        static bool show_another_window = true;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
            glfwPollEvents();

            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            render_status_window();
            if (show_another_window) {
                render_maintenance_window(&show_another_window);
            }

            //render_image_window();
            {
                static bool p_open;
                ImGui::Begin("##Image", &p_open, ImGuiWindowFlags_NoTitleBar);
                //// ImGui::Text("Image");
                //// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
                //data::scan::scan_t::pixel_t* image = data::scan::n_image(data::g_app_stat.scan, max(0, data::g_app_stat.scan->index));
                ////data::scan::scan_t::pixel_t* image = data::g_app_stat.scan->images;
                //if (image != NULL) {
                //    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 3072, 3072, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, image);
                //}
                //else {
                //    // error handling
                //}
                //GLenum err = glGetError();
                //if (err != GL_NO_ERROR) {


                //    // error handling
                //}
                //ImGui::Image((void*)(intptr_t)texture, ImVec2(1024, 1024));

                image::render(&g_image_widget[0]);
                ImGui::SameLine();
                image::render(&g_image_widget[1]);
                image::render(&g_image_widget[2]);
                ImGui::SameLine();
                image::render(&g_image_widget[3]);
                ImGui::End();
            }

            // Rendering
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }

    void drop(GLFWwindow* window)
    {
        if (window) {
            // Cleanup
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            glfwDestroyWindow(window);
            glfwTerminate();
        }
    }

}
#endif   // CONSOLE_UI_IMPLEMENTATION
