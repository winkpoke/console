#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "def.h"
#include "ui/app.hxx"
#include "control/control.hxx"
#include "control/runtime_data.hxx"
#include "mod/fpd/control/fpd.hxx"

namespace ui {
    namespace renders {
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

        bool render_status_window(app_t* app)
        {
            auto runtime = control::get_runtime_data();
            assert(runtime);
            auto hvg = cl::get<control::hvg::hvg_t>(runtime->objects, "hvg");
            assert(hvg);

            static bool show_demo_window = false;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove     | 
                                            ImGuiWindowFlags_NoResize   |
                                            ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoTitleBar;
            static bool* p_open;
            ImGui::Begin("##Status", p_open, window_flags);                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("FPD status  ... ");
            ImGui::SameLine();
            ImGui::Text(mod::fpd::control::to_string(app->fpd_status).c_str());

            ImGui::AlignTextToFramePadding();
            ImGui::Text("HVG status  ... ");
            ImGui::SameLine();
            ImGui::Text(control::hvg::to_string(app->hvg_status).c_str());

            ImGui::SameLine();
            if (ImGui::Button("Connect")) {
                app->fpd_status = (mod::fpd::control::status_e)((static_cast<int>(app->fpd_status) + 1) % 4);
            }

            ImGui::Separator();
            ImGui::NewLine();
            ImGui::NewLine();
            ImGui::Text("CBCT Parameters:");
            ImGui::AlignTextToFramePadding();
            {
                // kV slider
                ImGui::SetNextItemWidth(270);
                ImGui::SliderFloat("##kV", &app->kv, 50.0f, 100.0f, "%.1f");
                ImGui::SameLine(0, 5);
                if (ImGui::Button("-##kV")) {
                    if (app->kv > 50.0) {
                        app->kv -= 0.1;
                    }
                }
                ImGui::SameLine(0, 5);
                if (ImGui::Button("+##kV")) {
                    if (app->kv < 100.0) {
                        app->kv += 0.1;
                    }
                }
                ImGui::SameLine(0, 10); ImGui::Text("kV");
                ImGui::SameLine(); HelpMarker("50.0 ~ 100.0 kV");
                control::hvg::set_kv(hvg.get(), app->kv);
            }

            {
                // mAs slider
                ImGui::SetNextItemWidth(270);
                ImGui::SliderFloat("##mAs", &app->mAs, 0.0f, 10.0f, "%.1f");
                ImGui::SameLine(0, 5);
                if (ImGui::Button("-##mAs")) {
                    app->mAs -= 0.1;
                }
                ImGui::SameLine(0, 5);
                if (ImGui::Button("+##mAs")) {
                    app->mAs += 0.1;
                }
                ImGui::SameLine(0, 10); ImGui::Text("mAs"); 
                ImGui::SameLine(); HelpMarker("mAs Slider 0 ~ 10 mAs");
                control::hvg::set_mAs(hvg.get(), app->mAs);
            }

            ImGui::SetNextItemWidth(338);
            ImGui::Combo("CBCT Mode", (int*)&app->cbct_mode,
                cbct_mode_list, IM_ARRAYSIZE(cbct_mode_list));

            //const char* traj_items[] = { "Full", "Half" };
            const char* traj_items[] = { "Full"};
            static int traj_item_current = 0; // If the selection isn't within 0..count, Combo won't display a preview
            ImGui::SetNextItemWidth(338);
            ImGui::Combo("Trajectory", &traj_item_current, traj_items, IM_ARRAYSIZE(traj_items));
            ImGui::NewLine(); 
            ImGui::NewLine();
            ImGui::Separator();
            ImGui::NewLine();
            ImGui::NewLine();
            ImGui::Text("Reconstruction:");

            ImGui::SetNextItemWidth(338);
            ImGui::Combo("Reconstruction Volume", (int*)&app->resolution,
                resolution_list, IM_ARRAYSIZE(resolution_list));
            {
                // Slice distance slider
                ImGui::SetNextItemWidth(270);
                ImGui::SliderFloat("##slicedist", &app->slice_dist, 1.0f, 5.0f, "%.1f");
                ImGui::SameLine(0, 5);
                if (ImGui::Button("-##slicedist")) {
                    app->slice_dist -= 0.5;
                }
                ImGui::SameLine(0, 5);
                if (ImGui::Button("+##slicedist")) {
                    app->slice_dist += 0.5;
                }
                ImGui::SameLine(0, 10); ImGui::Text("Slice distance (mm)");
                ImGui::SameLine(); HelpMarker("");
            }
            ImGui::NewLine();
            ImGui::NewLine();
            ImGui::Separator();
            ImGui::NewLine();
            ImGui::NewLine();

            //ImGui::Text("Control:");
            ImGui::SameLine(50);
            ImGui::Button("Setup Patient", ImVec2(150, 50));
            ImGui::SameLine();
            ImGuiButtonFlags exposure_button_flag = ImGuiButtonFlags_None; // = ImGuiButtonFlags_Disabled;
            if (!control::is_exposure_ready()) {
                exposure_button_flag = ImGuiButtonFlags_Disabled;
            }

            if (ImGui::ButtonEx("Exposure", ImVec2(150, 50), exposure_button_flag)) {
                control::exposure();
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
    }
}
