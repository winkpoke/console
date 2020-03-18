#ifndef _CBCT_UI_INCLUDE_H_
#define _CBCT_UI_INCLUDE_H_

#include "ui/image.h"
#include "control.hxx"

namespace mod::cbct::ui {
    struct cbct_t {
        mod::fpd::control::status_e fpd_status;
        mod::hvg::control::status_e hvg_status;
        float kv;
        float mAs;
        control::cbct_mode_t cbct_mode;

        cl::i64 index;
        ::ui::image_view<cl::u16>* image0;
        ::ui::image_view<cl::u16>* image3;
        ::ui::image_view<cl::u16>* image1;
        ::ui::image_view<cl::u16>* image2;

        // Reconstruction 
        control::resolution_t resolution;
        float slice_dist;

        cl::f32 sid;
        cl::f32 sdd;
        cl::f32 proj_offset_x;
        cl::f32 proj_offset_y;
        cl::f32 source_offset_x;
        cl::f32 source_offset_y;
        cl::f32 out_of_plane_angle;
        cl::f32 in_plane_angle;
    };

    bool init(cbct_t* cbct, control::cbct_dummy_t* cbct_control);

    void drop(ui::cbct_t* cbct);

    bool render(ui::cbct_t* cbct);

    void update(ui::cbct_t* dst, control::cbct_dummy_t* src);
    void update(cl::runtime_object_t* dst, cl::runtime_object_t* src);
}

#endif //!_CBCT_UI_INCLUDE_H_

#ifdef CBCT_UI_IMPLEMENTATION
#ifndef CBCT_UI_IMPLEMENTED
#define CBCT_UI_IMPLEMENTED

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "cl.h"

#include "def.h"
#include "control/runtime_data.hxx"

namespace mod::cbct::ui {
    bool init(cbct_t* cbct, control::cbct_dummy_t* cbct_control)
    {
        assert(cbct && cbct_control);

        update(cbct, cbct_control);
        return true;
    }

    void drop(ui::cbct_t* cbct)
    {
        if (cbct) {
            cl::recycle(cbct->image0);
            cl::recycle(cbct->image1);
            cl::recycle(cbct->image2);
            cl::recycle(cbct->image3);
        }
    }

    void update(ui::cbct_t* dst, control::cbct_dummy_t* src)
    {
        assert(dst && src);
        dst->fpd_status = fpd::control::get_status(src->fpd.get());
        dst->hvg_status = hvg::control::get_status(src->hvg.get());
        dst->kv = hvg::control::get_kv(src->hvg.get());
        dst->mAs = hvg::control::get_mAs(src->hvg.get());
        dst->cbct_mode = src->mode;
        dst->resolution = src->resolution;
        dst->slice_dist = src->slice_dist;

        ::modal::scan_t* scan = fpd::control::get_scan(src->fpd.get());
        const cl::i64 index = (cl::i64)::modal::len(scan) - 1;
        if (dst->index != index && index >= 0) {
            auto old_image = dst->image0;
            assert(index < 360 && index >= 0);
            auto p = ::modal::get_data_at(scan, index);
            const cl::usize len = 1024 * 1024;
            static auto pnew = cl::alloc<cl::u16>(len);
            memcpy(pnew, p, len * sizeof(cl::u16));
            static auto img = cl::build_raw<sil::image_t<cl::u16>>(1024, 1024, 1, pnew);
            if (!dst->image0) {
                dst->image0 = cl::build_raw<::ui::image_view<cl::u16>>(512, 512, img);
            }
            else {
                init(dst->image0, 512, 512, img);
            }

            switch (index) {
            case 90:
                cl::recycle(dst->image1);
                dst->image1 = clone(old_image);
                break;
            case 180:
                cl::recycle(dst->image2);
                dst->image2 = clone(old_image);
                break;
            case 270:
                cl::recycle(dst->image3);
                dst->image3 = clone(old_image);
                break;
                //default:
                //    cl::dealloc(old_image);
            }
            dst->index = index;
        }

        dst->sid = static_cast<cl::f32>(src->geo.sid);
        dst->sdd = static_cast<cl::f32>(src->geo.sdd);

        dst->proj_offset_x = static_cast<cl::f32>(src->geo.proj_offset_x);
        dst->proj_offset_y = static_cast<cl::f32>(src->geo.proj_offset_y);
        dst->source_offset_x = static_cast<cl::f32>(src->geo.source_offset_x);
        dst->source_offset_y = static_cast<cl::f32>(src->geo.source_offset_y);
        dst->out_of_plane_angle = static_cast<cl::f32>(src->geo.out_of_plane_angle);
        dst->in_plane_angle = static_cast<cl::f32>(src->geo.in_plane_angle);
    }

    void update(cl::runtime_object_t* dst, cl::runtime_object_t* src)
    {
        auto ui = cl::get<ui::cbct_t>(dst, "cbct");
        assert(ui);

        auto control = cl::get<control::cbct_dummy_t>(src, "cbct");
        assert(control);

        update(ui.get(), control.get());
    }

    bool render_image_window(cbct_t* cbct)
    {
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar;
        static bool p_open;
        ImGui::Begin("##Image", &p_open, window_flags);

        if (cbct->image0) {
            render(cbct->image0);
        }

        ImGui::SameLine();
        if (cbct->image1) {
            render(cbct->image1);
        }
        if (cbct->image2) {
            render(cbct->image2);
        }
        ImGui::SameLine();
        if (cbct->image3) {
            render(cbct->image3);
        }
        ImGui::End();
        return true;
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

    static bool render_status_window(cbct_t* cbct)
    {
        auto runtime = ::control::get_runtime_data();
        assert(runtime);

        auto hvg = cl::get<mod::hvg::control::hvg_t>(runtime->objects, "hvg");
        assert(hvg);

        auto cbct_control = cl::get<cbct::control::cbct_dummy_t>(runtime->objects, "cbct");
        assert(cbct_control);

        static bool show_demo_window = false;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar;
        static bool* p_open;
        ImGui::Begin("##Status", p_open, window_flags);                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("FPD status  ... "); ImGui::SameLine();
        ImGui::Text(mod::fpd::control::to_string(cbct->fpd_status).c_str());

        ImGui::AlignTextToFramePadding();
        ImGui::Text("HVG status  ... "); ImGui::SameLine();
        ImGui::Text(mod::hvg::control::to_string(cbct->hvg_status).c_str());

        //ImGui::SameLine();
        //if (ImGui::Button("Connect")) {
        //    cbct->fpd_status = (mod::fpd::control::status_e)((static_cast<int>(cbct->fpd_status) + 1) % 4);
        //}

        ImGui::Separator();
        ImGui::NewLine(); ImGui::NewLine();
        ImGui::Text("CBCT Parameters:");
        ImGui::AlignTextToFramePadding();

        auto draw_slider = [](const char* label, cl::f32* v, cl::f32 step, cl::f32 low, cl::f32 upper, 
                              cl::f32 w, cl::f32 s0, cl::f32 s1, const char* format, const char* unit, 
                              const char* tip, bool show = true) 
        {
            ImGui::SetNextItemWidth(w);
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, !show);
            ImGui::SliderFloat(label, v, low, upper, format);
            ImGui::SameLine(0, s0);
            char temp[256];
            strcpy(temp, "-");
            if (ImGui::Button(strcat(temp, label))) {
                if (*v > low) {
                    *v -= step;
                }
            }
            ImGui::SameLine(0, s0);
            strcpy(temp, "+");
            if (ImGui::Button(strcat(temp, label))) {
                if (*v < upper) {
                    *v += step;
                }
            }
            ImGui::SameLine(0, s1); ImGui::Text(unit);
            ImGui::SameLine(); 
            ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false); 
            HelpMarker(tip);
        };

        bool show = true;
        if (!control::is_exposure_ready(cbct_control.get())) {
            show = false;
        }
        // kV slider
        //ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        draw_slider("##kV", &cbct->kv, 0.1f, 50.0f, 100.0f, 270.f, 5.f, 10.f, "%.1f", "kV", "50.0 ~ 100.0 kV", show);
        cl::dispatch(hvg::control::set_kv, hvg.get(), cbct->kv);
        //ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);
        
        // mAs slicer
        draw_slider("##mAs", &cbct->mAs, 0.1f, 50.0f, 100.0f, 270.f, 5.f, 10.f, "%.1f", "mAs", "0 ~ 10.0 mAs", show);
        cl::dispatch(hvg::control::set_mAs, hvg.get(), cbct->mAs);

        ImGui::SetNextItemWidth(338);
        ImGui::Combo("CBCT Mode", (int*)&cbct->cbct_mode,
            control::cbct_mode_list, IM_ARRAYSIZE(control::cbct_mode_list));
        cl::dispatch<void>(control::set_mode, cbct_control.get(), cbct->cbct_mode);

        const char* traj_items[] = { "Full" };
        static int traj_item_current = 0; // If the selection isn't within 0..count, Combo won't display a preview
        ImGui::SetNextItemWidth(338);
        ImGui::Combo("Trajectory", &traj_item_current, traj_items, IM_ARRAYSIZE(traj_items));
        ImGui::NewLine(); ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine(); ImGui::NewLine();
        ImGui::Text("Reconstruction:");

        ImGui::SetNextItemWidth(338);
        ImGui::Combo("Reconstruction Volume", (int*)&cbct->resolution,
            control::resolution_list, IM_ARRAYSIZE(control::resolution_list));
        cl::dispatch<void>(control::set_resolution, cbct_control.get(), cbct->resolution);

        // Slice distance slider
        draw_slider("##slicedist", &cbct_control->slice_dist, 0.5, 1.0, 5.0, 270, 5, 10, "%.1f", "Slice distance (mm)", "", true);

        if (ImGui::TreeNode("advanced settings:")) {
            auto p = cbct;
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::InputFloat("SID (mm)", &p->sid, 0, 0, "%0.2f"); ImGui::SameLine(); HelpMarker("Souce to Isocenter Distance (mm)");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::InputFloat("SDD (mm)", &p->sdd, 0, 0, "%0.2f"); ImGui::SameLine(); HelpMarker("Source to Detctor Distance (mm)");

            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::InputFloat("Projection offset X (mm)", &p->proj_offset_x, 0, 0, "%0.4f"); ImGui::SameLine(); HelpMarker("(mm)");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::InputFloat("Projection offset Y (mm)", &p->proj_offset_y, 0, 0, "%0.4f"); ImGui::SameLine(); HelpMarker("(mm)");

            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::InputFloat("Source offset X (mm)", &p->source_offset_x, 0, 0, "%0.4f"); ImGui::SameLine(); HelpMarker("(mm)");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::InputFloat("Source offset Y (mm)", &p->source_offset_y, 0, 0, "%0.4f"); ImGui::SameLine(); HelpMarker("(mm)");

            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::InputFloat("Out of plane angle (\xc2\xb0)", &p->out_of_plane_angle, 0, 0, "%0.4f");
            ImGui::SameLine();
            HelpMarker("A rotation of flat panel with rotation axis perpendicular to the gantry rotation axis " 
                       "and parallel to the flat panel. An increase in the value corresponds to a counter-clockwise "
                       "rotation of the flat panel as viewed from a positive value along the x axis towards the "
                       "isocenter. (degree)");
            ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.3f);
            ImGui::InputFloat("In plane angle (\xc2\xb0)", &p->in_plane_angle, 0, 0, "%0.4f");
            ImGui::SameLine();
            HelpMarker("A rotation of flat panel with rotation axis perpendicular to the flat panel. "
                       "An increase in the value of angle corresponds to a counter-clockwise rotation "
                       "of the flat panel as viewed from the source. (degree)");
            
            cbct_control->geo.sid = p->sid;
            cbct_control->geo.sdd = p->sdd;
            cbct_control->geo.proj_offset_x = p->proj_offset_x;
            cbct_control->geo.proj_offset_y = p->proj_offset_y;
            cbct_control->geo.source_offset_x = p->source_offset_x;
            cbct_control->geo.source_offset_y = p->source_offset_y;
            cbct_control->geo.out_of_plane_angle = p->out_of_plane_angle;
            cbct_control->geo.in_plane_angle = p->in_plane_angle;

            ImGui::TreePop();
        }

        ImGui::NewLine(); ImGui::NewLine();
        ImGui::Separator();
        ImGui::NewLine(); ImGui::NewLine();
        ImGui::SameLine(50);
        ImGui::Button("Setup Patient", ImVec2(150, 50));
        ImGui::SameLine();
        ImGuiButtonFlags exposure_button_flag = ImGuiButtonFlags_None; // = ImGuiButtonFlags_Disabled;
        if (!control::is_exposure_ready(cbct_control.get())) {
            exposure_button_flag = ImGuiButtonFlags_Disabled;
        }

        if (ImGui::ButtonEx("Exposure", ImVec2(150, 50), exposure_button_flag)) {
            cl::dispatch<void>(control::exposure, cbct_control.get());
        }
        ImGui::NewLine(); ImGui::SameLine(50);
        if (ImGui::Button("Reconstruction", ImVec2(150, 50))) {
            cl::dispatch<void>(control::reconstruct, cbct_control.get());
        }
        ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); 
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

        //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
        if (show_demo_window) {
            ImGui::ShowDemoWindow(&show_demo_window);
        }
        return true;
    }

    bool render(ui::cbct_t* cbct)
    {
        assert(cbct);
        render_status_window(cbct);
        render_image_window(cbct);
        return true;
    }
}

#endif // !CBCT_UI_IMPLEMENTATION
#endif //CBCT_UI_IMPLEMENTATION
