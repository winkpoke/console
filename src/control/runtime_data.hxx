#ifndef MODAL_RUNTIME_DATA_INCLUDE_H
#define MODAL_RUNTIME_DATA_INCLUDE_H

#include <memory>

#include <librealsense2/rs.hpp>

#include "def.h"
#include "modal/scan.hxx"

#include "control/websocket.hxx"
#include "fpd/fpd.hxx"


namespace hvg { struct context_t; }

namespace control {
    struct runtime_data_t {
        // FPD 
        fpd_status_t fpd;
        modal::scan_t* scan = NULL;

        // HVG 
        hvg_status_t hvg;
        float kv;
        float mAs;
        cbct_mode_t cbct_mode;
        int serial_port;
        int serail_baud;
        hvg::context_t* hvg_context;

        // Reconstruction 
        resolution_t resolution;
        float slice_dist;

        // Upstream server
        websocket::websocket_t* socket;

        // window

        // Camera
        //rs2::pipeline camera;

        static cl::unique_ptr<runtime_data_t> _this;
    };

    bool init(runtime_data_t* d);
    void drop(runtime_data_t* d);

    runtime_data_t* get_data();
}

#endif    // MODAL_RUNTIME_DATA_INCLUDE_H

#ifdef MODAL_RUNTIME_DATA_IMPLEMENTATION
#ifndef MODAL_RUNTIME_DATA_IMPLEMENTATED
#define MODAL_RUNTIME_DATA_IMPLEMENTATED

namespace control {
    bool init(runtime_data_t* d) {
        d->fpd = FPD_UNCONNECTED;
        d->hvg = HVG_UNCONNECTED;
        d->kv = 70.0f;
        d->mAs = 5.0f;
        d->cbct_mode = CUSTOM;
        d->serial_port = 3;
        d->serail_baud = 19200;
        d->hvg_context = NULL;
        d->resolution = _512X512;
        d->slice_dist = 2.5f;

        //app->scan = scan::alloc();
        d->scan = cl::build_raw<modal::scan_t>(3072, 3072);

        // websocket
        d->socket = cl::build_raw<websocket::websocket_t>("ws://172.17.95.188:3000/ws");

        // Configure and start the pipeline
        //app->camera.start();

        return true;
    }

    void drop(runtime_data_t* d) {
        drop(d->scan);
        websocket::drop(d->socket);
    }

    cl::unique_ptr<runtime_data_t> runtime_data_t::_this = cl::build_unique<runtime_data_t>(drop);

    runtime_data_t* get_data()
    {
        return runtime_data_t::_this.get();
    }
}
#endif // !MODAL_RUNTIME_DATA_IMPLEMENTATED
#endif   // MODAL_RUNTIME_DATA_IMPLEMENTATION
