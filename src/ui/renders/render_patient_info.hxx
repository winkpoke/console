#pragma once


#include "imgui.h"
#include "modal/patient.hxx"
#include "ui/image.h"

namespace ui {
    struct app_t;
    modal::patient_t g_patient;

    bool render_patient_info_window(app_t* app, const modal::patient_t* p)
    {
        static bool p_open;
        ImGui::Begin("##patient_info", &p_open, ImGuiWindowFlags_NoTitleBar);
        ImGui::Columns(3, 0, false);
        ImGui::SetColumnWidth(0, 80);
        auto portrait = build_unique<image_view<cl::u8>>(ui::drop, 50, 73, std::shared_ptr<sil::image_t<cl::u8>>(p->portrait));
        render(portrait.get());
        ImGui::NextColumn();
        ImGui::Text("Name:     "); ImGui::SameLine(); ImGui::Text(p->name.c_str());
        ImGui::Text("ID:       "); ImGui::SameLine(); ImGui::Text(p->id.c_str());
        ImGui::Text("Age:      "); ImGui::SameLine(); ImGui::Text(std::to_string(p->age).c_str());
        ImGui::NextColumn();
        ImGui::Text("Gender:   "); ImGui::SameLine(); ImGui::Text(modal::to_string(p->gender).c_str());
        ImGui::Text("Category: "); ImGui::SameLine(); ImGui::Text(p->category.c_str());
        ImGui::Text("Site:     "); ImGui::SameLine(); ImGui::Text(p->site.c_str());
        ImGui::Columns(1);

        ImGui::End();
        return true;
    }
}
