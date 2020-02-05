#pragma once

#include "ui/image.h"

#include "imgui.h"

namespace ui {
    struct app_t;
    image_view<cl::u8> g_image_patient;

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
}
