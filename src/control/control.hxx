#ifndef CONSOLE_INCLUDE_CONTROL_H
#define CONSOLE_INCLUDE_CONTROL_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

namespace control {
    using namespace spdlog;
    bool init();
    void drop();
}
#endif   // CONSOLE_INCLUDE_CONTROL_H

#ifdef CONSOLE_CONTROL_IMPLEMENTATION
#ifndef CONSOLE_CONTROL_IMPLEMENTED
#define CONSOLE_CONTROL_IMPLEMENTED

#include "runtime_data.hxx"
#include "mod/patient/control.hxx"
#include "mod/patient/patient.h"
#include "mod/hvg/hvg.h"
#include "mod/fpd/fpd.h"
#include "mod/cbct/cbct.h"
#include "runtime_data.hxx"

namespace control {
    // FPD

    //bool is_ready_setup_patient()
    //{
    //    runtime_data_t* d = get_runtime_data();
    //    assert(d);

    //    auto fpd = cl::get<mod::fpd::control::fpd_t>(d->objects, "fpd");
    //    assert(fpd);

    //    auto hvg = cl::get<mod::hvg::control::hvg_t>(d->objects, "hvg");
    //    assert(hvg);

    //    return hvg->status == mod::hvg::control::status_e::HVG_READY &&
    //           fpd->status == mod::fpd::control::status_e::FPD_READY;
    //}


    bool init()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        constexpr cl::usize FPD_WIDTH = 1024;
        constexpr cl::usize FPD_HEIGHT = 1024;
        constexpr cl::f64 FPD_X_RESOLUTION = 0.417;
        constexpr cl::f64 FPD_Y_RESOLUTION = 0.417;

        auto hvg = cl::build_shared<mod::hvg::control::hvg_t>(70.f, 5.f, nullptr);
        cl::mount(d->objects, hvg, "hvg", "0.0.1");

        //auto fpd = cl::build_shared<mod::fpd::control::fpd_t>(FPD_WIDTH, FPD_HEIGHT, FPD_X_RESOLUTION, FPD_Y_RESOLUTION);
        //cl::mount(d->objects, fpd, "fpd", "0.0.1");

        auto dummy_fpd = cl::build_shared<mod::fpd::control::fpd_dummy_t>(FPD_WIDTH, FPD_HEIGHT, FPD_X_RESOLUTION, FPD_Y_RESOLUTION);
        cl::mount(d->objects, dummy_fpd, "fpd_dummy", "0.0.1");

        // CBCT 
        auto cbct = cl::build_shared<mod::cbct::control::cbct_dummy_t>(dummy_fpd, hvg);
        cl::mount(d->objects, cbct, "cbct", "0.0.1");

        // patient
        int w1, h1;
        unsigned char* img1 = stbi_load("resources\\images\\patient.png", &w1, &h1, NULL, 4);
        auto p = cl::build_raw<mod::patient::modal::patient_t>(u8"李四", u8"9527", 65, mod::patient::modal::gender_e::MALE, u8"H&N", u8"重庆海吉亚医院");
        p->portrait = cl::build_raw<sil::image_t<cl::u8>>(w1, h1, 4, img1);
        auto patient = cl::build_shared<mod::patient::control::patient_t>(p);
        cl::mount(d->objects, patient, mod::patient::mod_name, mod::patient::mod_version);

        // OPS
        //cl::mount(d->objects, cl::build_shared<mod::ops::control::ops_t>(), mod::ops::mod_name, mod::ops::mod_version);

        //auto ops = cl::get<mod::ops::control::ops_t>(d->objects, mod::ops::mod_name);
        //mod::ops::control::start(ops.get());

        
        std::thread connect_to_fpd([=]() {
            mod::cbct::control::connect_to_fpd(cbct.get());
        });
        connect_to_fpd.detach();

        // std::thread connect_to_hvg([=](){
        //     mod::cbct::control::connect_to_hvg(cbct.get());
        // });
        // connect_to_hvg.detach();
        
        std::thread connect_to_upstream_server([=](){
            mod::cbct::control::connect_to_upstream_server(cbct.get());
        });
        connect_to_upstream_server.detach();
        
        return true;
    }

    void drop()
    {
        auto p = get_runtime_data();
        if (p) {
            cl::unmount_all(p->objects);
            cl::recycle(p);
        }
    }
}
#endif // !CONSOLE_CONTROL_IMPLEMENTED

#endif   //CONSOLE_CONTROL_IMPLEMENTATION