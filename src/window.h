#ifndef _INCLUDE_WINDOW_H
#define _INCLUDE_WINDOW_H

#include <memory>

#include "cl.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

struct window_t {
    GLFWwindow* wnd;
    GLFWmonitor* monitor;
    int x, y, w, h;
};


namespace window {
    struct window_t {
        GLFWwindow* wnd;
        GLFWmonitor* monitor;
        int x, y, w, h;

        static bool init(window_t* win, int x, int y, int w, int h);
        static void drop(window_t* win);
    };

    //template <class... Args>
    //window_t* make_raw(Args... args)
    //{
    //    window_t* win = (window_t*)cl::alloc<window_t>();
    //    if (!win) {
    //        return nullptr;
    //    }
    //    if (!window_t::init(win, args...)) {
    //        window_t::drop(win);
    //        return nullptr;
    //    }
    //    return win;
    //}

    template <class... Args>
    std::unique_ptr<window_t, decltype(&window_t::drop)> make_unique(Args... args)
    {
        auto win = cl::build_raw(args...);
        return std::unique_ptr<window_t, decltype(&window_t::drop)>(win, window_t::drop);
    }

    template <class... Args>
    std::shared_ptr<window_t> make_shared(Args... args)
    {
        auto win = cl::build_raw<window_t>(args...);
        return std::shared_ptr<window_t>(win, window_t::drop);
    }

    template <class T>
    bool is_fullscreen(T* win)
    {
        return glfwGetWindowMonitor(win->wnd) != nullptr;
    }

    template <class T>
    void set_fullscreen(T* win, bool fullscreen)
    {
        if (is_fullscreen(win) == fullscreen)
            return;

        if (fullscreen)
        {
            // backup window position and window size
            glfwGetWindowPos(win->wnd, &win->x, &win->y);
            glfwGetWindowSize(win->wnd, &win->w, &win->h);

            // get resolution of monitor
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

            // switch to full screen
            glfwSetWindowMonitor(win->wnd, win->monitor, 0, 0, mode->width, mode->height, 0);
        }
        else
        {
            // restore last window size and position
            glfwSetWindowMonitor(win->wnd, nullptr, win->x, win->y, win->w, win->h, 0);
        }

        //_updateViewport = true;
    }
}

#endif // _INCLUDE_WINDOW_H

#ifdef WINDOW_IMPLEMENTATION

namespace window {
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    bool window_t::init(window_t* win, int x, int y, int w, int h)
    {
        // window_t* win = this;

        if (!win) {
            return false;
        }

        win->x = x;
        win->y = y;
        win->w = w;
        win->h = h;

        // Setup window
        glfwSetErrorCallback(glfw_error_callback);
        if (!glfwInit())
            return false;

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
        win->wnd = glfwCreateWindow(w, h, "Techtron CBCT Console", NULL, NULL);
        //GLFWwindow* window = glfwCreateWindow(1920, 1080, "Dear ImGui GLFW+OpenGL3 example", glfwGetPrimaryMonitor(), NULL);
        if (win->wnd == NULL)
            return false;
        glfwMakeContextCurrent(win->wnd);
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
            return false;
        }

        win->monitor = glfwGetPrimaryMonitor();

        if (!win->monitor) {
            return false;
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(win->wnd, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        return true;
    }

    void window_t::drop(window_t* win)
    {
        if (win) {
            if (win->wnd) {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplGlfw_Shutdown();
                ImGui::DestroyContext();

                glfwDestroyWindow(win->wnd);
                glfwTerminate();
            }
            free(win);
        }
    }
}

#endif // WINDOW_IMPLEMENTATION