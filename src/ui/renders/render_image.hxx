#pragma once

#include "imgui.h"

#include "ui/image.h"

namespace ui {
    namespace renders {
        ui::image_view<unsigned char> g_image_widget[4];

        bool render_image_window(struct app_t* app)
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoTitleBar;
            static bool p_open;
            ImGui::Begin("##Image", &p_open, window_flags);

            render(&g_image_widget[0]);
            ImGui::SameLine();
            render(&g_image_widget[1]);
            render(&g_image_widget[2]);
            ImGui::SameLine();
            render(&g_image_widget[3]);
            ImGui::End();
            return true;
        }
    }
}