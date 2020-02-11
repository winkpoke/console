#ifndef CONSOLE_INCLUDE_CONTROL_H
#define CONSOLE_INCLUDE_CONTROL_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"


#include "hvg/siemens.hxx"
#include "fpd/iray.hxx"

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

        fpd::connect(d->fpd.get());
    }

    // HVG

    void connect_to_hvg()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        const int port = 3;
        const int baud = 19200;

        hvg::connect(d->hvg.get(), port, baud, "8N2");
    }

    void setup_patient()
    {

    }


    bool is_ready_setup_patient()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        return d->hvg->status == hvg::status_e::HVG_READY && 
               d->fpd->status == fpd::status_e::FPD_READY;
    }

    bool is_exposure_ready()
    {
        runtime_data_t* d = get_runtime_data();
        assert(d);

        return d->hvg->status == hvg::status_e::HVG_READY && 
               d->fpd->status == fpd::status_e::FPD_READY;
    }

    bool init()
    {
        return true;
    }

    void drop()
    {
        auto p = get_runtime_data();
        if (p) {
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