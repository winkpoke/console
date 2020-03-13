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
    };

    bool init(cbct_t* cbct, control::cbct_t* cbct_control);

    void drop(ui::cbct_t* cbct);

    bool render(ui::cbct_t* cbct);

    void update(ui::cbct_t* dst, control::cbct_t* src);
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
    bool init(cbct_t* cbct, control::cbct_t* cbct_control)
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

    void update(ui::cbct_t* dst, control::cbct_t* src)
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
    }

    void update(cl::runtime_object_t* dst, cl::runtime_object_t* src)
    {
        auto ui = cl::get<ui::cbct_t>(dst, "cbct");
        assert(ui);

        auto control = cl::get<control::cbct_t>(src, "cbct");
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

        auto cbct_control = cl::get<cbct::control::cbct_t>(runtime->objects, "cbct");
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
                              cl::u32 w, cl::u32 s0, cl::u32 s1, const char* format, const char* unit, 
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
        draw_slider("##kV", &cbct->kv, 0.1, 50.0, 100.0, 270, 5, 10, "%.1f", "kV", "50.0 ~ 100.0 kV", show);
        cl::dispatch(hvg::control::set_kv, hvg.get(), cbct->kv);
        //ImGui::PushItemFlag(ImGuiItemFlags_Disabled, false);

        // mAs slicer
        draw_slider("##mAs", &cbct->mAs, 0.1, 50.0, 100.0, 270, 5, 10, "%.1f", "kV", "0 ~ 10.0 mAs", show);
        cl::dispatch(hvg::control::set_mAs, hvg.get(), cbct->mAs);

        ImGui::SetNextItemWidth(338);
        ImGui::Combo("CBCT Mode", (int*)&cbct->cbct_mode,
            cbct_mode_list, IM_ARRAYSIZE(cbct_mode_list));

        //const char* traj_items[] = { "Full", "Half" };
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
            resolution_list, IM_ARRAYSIZE(resolution_list));

        // Slice distance slider
        draw_slider("##slicedist", &cbct_control->slice_dist, 0.5, 1.0, 5.0, 270, 5, 10, "%.1f", "Slice distance (mm)", "", show);

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

        ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine(); ImGui::NewLine();
        ImGui::NewLine(); ImGui::NewLine();
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
