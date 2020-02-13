#pragma once


#include "imgui.h"
#include "modal/patient.hxx"
#include "ui/image.h"

namespace ui {
    struct app_t;
    namespace  renders {

        bool render_patient_info_window(app_t* app)
        {
            assert(app);

            modal::patient_t* p = app->patient;
            assert(p);

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoTitleBar;

            static bool p_open;
            ImGui::Begin("##patient_info", &p_open, window_flags);
            ImGui::Columns(3, 0, false);
            ImGui::SetColumnWidth(0, 80);
            if (p->portrait)
            {
                auto portrait = cl::build_unique<image_view<cl::u8>>(ui::drop, 50, 73, std::shared_ptr<sil::image_t<cl::u8>>(p->portrait));
                render(portrait.get());
            }
            ImGui::NextColumn();
            ImGui::Text("Name:     "); ImGui::SameLine(); if (p) ImGui::Text(p->name);
            ImGui::Text("ID:       "); ImGui::SameLine(); if (p)ImGui::Text(p->id);
            ImGui::Text("Age:      "); ImGui::SameLine(); if (p) ImGui::Text(std::to_string(p->age).c_str());
            ImGui::NextColumn();
            ImGui::Text("Gender:   "); ImGui::SameLine(); if (p) ImGui::Text(modal::to_string(p->gender).c_str());
            ImGui::Text("Category: "); ImGui::SameLine(); if (p) ImGui::Text(p->category);
            ImGui::Text("Site:     "); ImGui::SameLine(); if (p) ImGui::Text(p->site);
            ImGui::Columns(1);
            ImGui::End();
            return true;
        }
    }
}
