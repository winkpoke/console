#include <memory>

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "rs232.h"

// logging
//#define SPDLOG_NO_FILE_LOC
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include <stdio.h>
#include <thread>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

#include "cl.h"

#include "console.h"


int main(int, char**)
{
    spdlog::set_level(spdlog::level::trace); // Set global log level to debug
    SPDLOG_INFO("CBCT Console starting ...");

    //// CBCT init
    data::init();
    std::thread connect_to_fpd(modal::connect_to_fpd);
    //std::thread connect_to_hvg(modal::connect_to_hvg);
    modal::connect_to_upstream_server();

    auto win = cl::build_raw<window::window_t>(300, 300, 1024, 768);

    if (win) {
        ui::init(win);
        win->renders.push_back(&ui::render_status_window);
        win->renders.push_back(&ui::render_maintenance_window);
        win->renders.push_back(&ui::render_image_window);
        win->renders.push_back(&ui::render_patient_info_window);
        win->renders.push_back(&ui::process_camera_data);
        ui::render(win);
        window::drop(win);
    }

    connect_to_fpd.join();
    ////connect_to_hvg.join();

    modal::drop();
    data::drop();

    return 0;
}
