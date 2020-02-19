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

namespace control {
    // FPD

    void connect_to_fpd()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);
        // control::fpd::fpd_t* p = control::get<control::fpd::fpd_t>(d, "fpd");
        std::shared_ptr<control::fpd::fpd_t> p = cl::get<control::fpd::fpd_t>(d->runtime_data2, "fpd");
        fpd::connect(p.get());
    }

    // HVG

    void connect_to_hvg()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        const int port = 3;
        const int baud = 19200;
        auto hvg = get<hvg::hvg_t>(d, "hvg");
        assert(hvg);

        hvg::connect(hvg, port, baud, "8N2");
    }

    void setup_patient()
    {

    }


    bool is_ready_setup_patient()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        auto fpd = cl::get<fpd::fpd_t>(d->runtime_data2, "fpd");
        assert(fpd);

        auto hvg = get<hvg::hvg_t>(d, "hvg");
        assert(hvg);

        return hvg->status == hvg::status_e::HVG_READY && 
               fpd->status == fpd::status_e::FPD_READY;
    }

    bool is_exposure_ready()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        auto fpd = cl::get<fpd::fpd_t>(d->runtime_data2, "fpd");
        assert(fpd);

        auto hvg = get<hvg::hvg_t>(d, "hvg");
        assert(hvg);

        return hvg->status == hvg::status_e::HVG_READY && 
               fpd->status == fpd::status_e::FPD_READY;
    }

    bool init()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        // fpd::connect(d->fpd.get());
        //auto fpd = cl::build_unique<fpd::fpd_t>(3072, 3072);
        auto hvg = cl::build_unique<hvg::hvg_t>(70.0f, 5.0f, nullptr);
        //register_data(d, move(fpd), "fpd", "0.0.1");
        register_data(d, move(hvg), "hvg", "0.0.1");
        cl::mount(d->runtime_data2, cl::build_shared<fpd::fpd_t>(3072, 3072), "fpd", "0.01");
        return true;
    }

    void drop()
    {
        auto p = get_runtime_data();
        if (p) {
            //unregister_data(p, "fpd");
            unmount(p->runtime_data2, "fpd");
            unregister_data(p, "hvg");
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