#ifndef _PATIENT_UI_INCLUDE_H_
#define _PATIENT_UI_INCLUDE_H_

#include "imgui.h"
#include "cl.h"
#include "ui/image.h"
#include "control.hxx"

namespace ui {
    struct patient_t {
        char name[256];
        char id[256];
        cl::u8 age;
        modal::gender_e gender;
        char category[256];
        char site[256];
        image_view<cl::u8>* image;

        std::shared_mutex mutex;
        bool dirty;
    };

    bool init(patient_t* p, control::patient_t* modal);
    void drop(patient_t* p);

    bool is_dirty(patient_t* p);

    void update(ui::patient_t* dst, control::patient_t* src);
    void update_ui_data(ui::patient_t* dst, control::patient_t* src);
    void update_ui_data(cl::runtime_object_t* dst, cl::runtime_object_t* src);
    void update_control_data(control::patient_t* dst, ui::patient_t* src);

    bool render(ui::patient_t* p);
}

#endif // !_PATIENT_UI_INCLUDE_H_

#ifdef PATIENT_UI_IMPLEMENTATION
#ifndef PATIENT_UI_IMPLEMENTED
#define PATIENT_UI_IMPLEMENTED

namespace ui {

    bool init(patient_t* p, control::patient_t* control)
    {
        assert(p);
        assert(control);
        new(&p->mutex)std::shared_mutex;
        std::shared_lock lk1(control->mutex, std::defer_lock);
        std::unique_lock lk2(p->mutex, std::defer_lock);
        std::lock(lk1, lk2);
        update(p, control);
        return true;
    }

    void drop(patient_t* p)
    {
        assert(p);
        if (p) {
            p->mutex.~shared_mutex();
            if (p->image) {
                drop(p->image);
            }
            free(p);
        }
    }

    bool is_dirty(patient_t* p)
    {
        assert(p);
        std::shared_lock lk(p->mutex);
        return p->dirty;
    }

    void update(ui::patient_t* dst, control::patient_t* src) 
    {
        assert(dst);
        assert(src);
        strncpy(dst->name, src->modal->name, sizeof(modal::patient_t::name));
        strncpy(dst->id, src->modal->id, sizeof(modal::patient_t::id));
        strncpy(dst->category, src->modal->category, sizeof(modal::patient_t::category));
        strncpy(dst->site, src->modal->site, sizeof(modal::patient_t::site));
        dst->age = src->modal->age;
        dst->gender = src->modal->gender;
        if (dst->image) {
            drop(dst->image);
            dst->image = nullptr;
        }

        if (src->modal->portrait) {
            dst->image = cl::build_raw<image_view<cl::u8>>(50, 73, std::shared_ptr<sil::image_t<cl::u8>>(src->modal->portrait));
        }
    }

    void update_ui_data(ui::patient_t* dst, control::patient_t* src)
    {
        assert(dst);
        assert(src);
        std::shared_lock lk1(src->mutex, std::defer_lock);
        std::unique_lock lk2(dst->mutex, std::defer_lock);
        std::lock(lk1, lk2);

        if (control::is_dirty(src)) {
            update(dst, src);
            clear_dirty(src);
        }
    }

    void update_ui_data(cl::runtime_object_t* dst, cl::runtime_object_t* src)
    {
        auto control = cl::get<control::patient_t>(src, "patient");
        assert(control);
        auto ui = cl::get<ui::patient_t>(dst, "patient");
        assert(ui);

        update_ui_data(ui.get(), control.get());
    }

    void update_control_data(control::patient_t* dst, ui::patient_t* src)
    {

    }

    bool render(ui::patient_t* p)
    {
        assert(p);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar;

        static bool p_open;
        ImGui::Begin("##patient_info", &p_open, window_flags);
        ImGui::Columns(3, 0, false);
        ImGui::SetColumnWidth(0, 80);
        if (p->image)
        {
            render(p->image);
        }
        ImGui::NextColumn();
        ImGui::Text("Name:     "); ImGui::SameLine(); if (p) ImGui::Text(p->name);
        ImGui::Text("ID:       "); ImGui::SameLine(); if (p) ImGui::Text(p->id);
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

#endif // !PATIENT_UI_IMPLEMENTED
#endif PATIENT_UI_IMPLEMENTATION