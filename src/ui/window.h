#ifndef _INCLUDE_WINDOW_H
#define _INCLUDE_WINDOW_H

#include <memory>
#include <array>
#include <list>
#include <functional>

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

namespace ui {
    using namespace cl;
    using namespace std;

    struct window_t {
        using render_fun = std::function<bool(window_t*)>;
        using key_event_fun = std::function<bool(window_t*, int k)>;
        
        GLFWwindow* wnd;
        GLFWmonitor* monitor;
        int x, y, w, h;
        array<float, 4> background_color;
        list<render_fun> renders;
        list<key_event_fun> key_events;
    };
    bool init(window_t* win, int x, int y, int w, int h, array<float, 4> background_color = { 0.45f, 0.55f, 0.60f, 1.00f });
    void drop(window_t* win);

    bool is_fullscreen(window_t* win);

    void set_fullscreen(window_t* win, bool fullscreen);

    void process_event(window_t* win);
    void on_key_event(window_t* win, int key);
    void new_frame(window_t* win);
    void draw(window_t* win);
    void render(window_t* win);
    bool is_close(window_t* win);
}

#endif // _INCLUDE_WINDOW_H

#ifdef WINDOW_IMPLEMENTATION
#ifndef WINDOW_IMPLEMENTED
#define WINDOW_IMPLEMENTED
namespace ui {
    void set_fullscreen(window_t* win, bool fullscreen)
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

    bool is_fullscreen(window_t* win)
    {
        return glfwGetWindowMonitor(win->wnd) != nullptr;
    }

    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "Glfw Error %d: %s\n", error, description);
    }

    bool init(window_t* win, int x, int y, int w, int h, array<float, 4> background_color)
    {
        if (!win) {
            return false;
        }

        win->x = x;
        win->y = y;
        win->w = w;
        win->h = h;

        win->background_color = background_color;

        new(&win->renders)(list<window_t::render_fun>);
        new(&win->key_events)(list<window_t::key_event_fun>);

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

    void drop(window_t* win)
    {
        if (win) {
            if (win->wnd) {
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplGlfw_Shutdown();
                ImGui::DestroyContext();

                glfwDestroyWindow(win->wnd);
                glfwTerminate();
            }
        }
    }

    bool is_close(window_t* win)
    {
        if (!win || !win->wnd) {
            return true;
        }
        return glfwWindowShouldClose(win->wnd);
    }

    void render(window_t* win)
    {
        if (!win) {
            return;
        }

        // Custom renders
        for (auto r = win->renders.crbegin(); r != win->renders.crend(); ++r) {
            if ((*r)(win) == false) {
                break;
            }
        }
    }

    void process_event(window_t* win)
    {
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
            on_key_event(win, i);
        }
    }

    void on_key_event(window_t* win, int key)
    {
        for (auto r = win->key_events.crbegin(); r != win->key_events.crend(); ++r) {
            if ((*r)(win, key) == false) {
                break;
            }
        }
    }

    void new_frame(window_t* win)
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void draw(window_t* win)
    {
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(win->wnd, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(win->background_color[0], win->background_color[1], win->background_color[2], win->background_color[3]);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(win->wnd);
        glfwPollEvents();
    }

    //void 
}
#endif // !WINDOW_IMPLEMENTED
#endif // WINDOW_IMPLEMENTATION