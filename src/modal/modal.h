#ifndef CONSOLE_INCLUDE_MODAL_H
#define CONSOLE_INCLUDE_MODAL_H

#include <memory>

#include <librealsense2/rs.hpp>

#include "def.h"
#include "control/websocket.hxx"
#include "scan.hxx"


namespace hvg { struct context_t; }

namespace modal {
    const char* fpd_status_list[] = { "unconnected", "connecting", "ready", "error" };
    const char* hvg_status_list[] = { "unconnected", "connecting", "ready", "exposure", "error" };
    const char* cbct_mode_list[] = { "Head", "Lung", "Abdominal", "Custom" };
    const char* resolution_list[] = { "128x128", "256x256", "384x384", "512x512", "768x768" };

    struct app_stat_t {
        // FPD 
        fpd_status_t fpd;
        scan_t* scan = NULL;

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
        rs2::pipeline camera;
    };

    static app_stat_t g_app_stat;

    bool save();
    bool load();

    bool init();
    void drop();
}

#endif    // CONSOLE_INCLUDE_MODAL_H

#ifdef CONSOLE_MODAL_IMPLEMENTATION
#ifndef CONSOLE_MODAL_IMPLEMENTED
#define CONSOLE_MODAL_IMPLEMENTED

namespace modal {
    bool init(app_stat_t* app) {
        app->fpd = FPD_UNCONNECTED;
        app->hvg = HVG_UNCONNECTED;
        app->kv = 70.0f;
        app->mAs = 5.0f;
        app->cbct_mode = CUSTOM;
        app->serial_port = 3;
        app->serail_baud = 19200;
        app->hvg_context = NULL;
        app->resolution = _512X512;
        app->slice_dist = 2.5f;

        //app->scan = scan::alloc();
        app->scan = cl::build_raw<modal::scan_t>(3072, 3072);

        // websocket
        app->socket = cl::build_raw<websocket::websocket_t>("ws://172.17.95.188:3000/ws");

        // Configure and start the pipeline
        app->camera.start();

        return true;
    }

    void drop(app_stat_t* app) {
        drop(app->scan);
        websocket::drop(app->socket);
    }

    bool init()
    {
        return init(&g_app_stat);
    }

    void drop()
    {
        drop(&g_app_stat);
    }

    app_stat_t& get_app_stat() {
        return g_app_stat;
    }
}
#endif // !CONSOLE_MODAL_IMPLEMENTED


#endif   // CONSOLE_MODAL_IMPLEMENTATION
