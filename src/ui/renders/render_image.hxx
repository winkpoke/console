#pragma once

#include "imgui.h"

#include "ui/image.h"

namespace ui {
    namespace renders {
        //ui::image_view<unsigned char> g_image_widget;

        bool render_image_window(struct app_t* app)
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoTitleBar;
            static bool p_open;
            ImGui::Begin("##Image", &p_open, window_flags);
            
            if (app->image0) {
                render(app->image0);
            }
            else {
                render(app->image1);
            }
            ImGui::SameLine();
            render(app->image1);
            render(app->image1);
            ImGui::SameLine();
            render(app->image1);
            ImGui::End();
            return true;
        }
    }
}