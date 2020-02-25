#ifndef CONSOLE_INCLUDE_CONTROL_H
#define CONSOLE_INCLUDE_CONTROL_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"


#include "hvg/hvg.hxx"
#include "fpd/fpd.hxx"
#include "runtime_data.hxx"

#include "modal/modal.h"

namespace control {
    using namespace spdlog;

    // FPD
    void drop_fpd();
    
    // HVG
    void connect_to_hvg();
    void drop_hvg();

    void setup_patient();

    // status check
    bool is_ready_setup_patient();
    bool is_exposure_ready();

    // connect to upstream server via websocket
    void connect_to_upstream_server();

    // accessors
    // modal::app_stat_t* get_data();

    // functions
    bool init();
    void drop();
}
#endif   // CONSOLE_INCLUDE_CONTROL_H

#ifdef CONSOLE_CONTROL_IMPLEMENTATION
#ifndef CONSOLE_CONTROL_IMPLEMENTED
#define CONSOLE_CONTROL_IMPLEMENTED

#include "runtime_data.hxx"
#include "module/patient/control.hxx"

namespace control {
    // FPD

    void connect_to_fpd()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);
        std::shared_ptr<control::fpd::fpd_t> p = cl::get<control::fpd::fpd_t>(d->objects, "fpd");
        fpd::connect(p.get());
    }

    // HVG

    void connect_to_hvg()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        const int port = 3;
        const int baud = 19200;
        auto hvg = cl::get<hvg::hvg_t>(d->objects, "hvg");
        assert(hvg);

        hvg::connect(hvg.get(), port, baud, "8N2");
    }

    void setup_patient()
    {

    }


    bool is_ready_setup_patient()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        auto fpd = cl::get<fpd::fpd_t>(d->objects, "fpd");
        assert(fpd);

        auto hvg = cl::get<hvg::hvg_t>(d->objects, "hvg");
        assert(hvg);

        return hvg->status == hvg::status_e::HVG_READY && 
               fpd->status == fpd::status_e::FPD_READY;
    }

    bool is_exposure_ready()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        auto fpd = cl::get<fpd::fpd_t>(d->objects, "fpd");
        assert(fpd);

        auto hvg = cl::get<hvg::hvg_t>(d->objects, "hvg");
        assert(hvg);

        return hvg->status == hvg::status_e::HVG_READY && 
               fpd->status == fpd::status_e::FPD_READY;
    }

    bool init()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        cl::mount(d->objects, cl::build_shared<hvg::hvg_t>(70.f, 5.f, nullptr), "hvg", "0.0.1");
        cl::mount(d->objects, cl::build_shared<fpd::fpd_t>(3072, 3072), "fpd", "0.0.1");

        // patient
        int w1, h1;
        unsigned char* img1 = stbi_load("resources\\images\\patient.png", &w1, &h1, NULL, 4);
        auto p = cl::build_raw<modal::patient_t>();
        strncpy(p->name, u8"张三", sizeof(p->name));
        strncpy(p->id, "209845", sizeof(p->id));
        p->age = 65;
        p->gender = modal::gender_e::MALE;
        strncpy(p->category, "H&N", sizeof(p->category));
        strncpy(p->site, u8"海吉亚", sizeof(p->site));
        p->portrait = cl::build_raw<sil::image_t<cl::u8>>(w1, h1, 4, img1);
        auto patient = cl::build_shared<control::patient_t>(p);
        cl::mount(d->objects, patient, "patient", "0.01");
        return true;
    }

    void drop()
    {
        auto p = get_runtime_data();
        if (p) {
            cl::unmount_all(p->objects);
            drop(p);
        }
    }

    void connect_to_upstream_server()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        websocket::websocket_t* s = d->socket;
        SPDLOG_INFO("Connecting to server: ", s->url);
        if (!websocket::connect(s)) {
            SPDLOG_ERROR("Fail to connect to the server.");
            return;
        }
        SPDLOG_INFO("Successully connected to the server.");
        SPDLOG_INFO("Handshake with server...");
        websocket::on_recv_text(s, [](const char* msg) {
            SPDLOG_INFO("Message recieved: {}", msg);
            }
        );
        websocket::send(s, "<HELLO");
    }
}
#endif // !CONSOLE_CONTROL_IMPLEMENTED

#endif   //CONSOLE_CONTROL_IMPLEMENTATION