#ifndef CONSOLE_INCLUDE_DATA_H
#define CONSOLE_INCLUDE_DATA_H

#include <memory>

#include <librealsense2/rs.hpp>
#include "websocket.h"

enum error_t {
    OK,
    ERR_STATUS
};

enum fpd_status_t {
    FPD_UNCONNECTED,
    FPD_CONNECTING,
    FPD_READY,
    FPD_ERROR
};

enum hvg_status_t {
    HVG_UNCONNECTED,
    HVG_CONNECTING,
    HVG_READY,
    HVG_EXPOSURE,
    HVG_ERROR
};

enum cbct_mode_t {
    HEAD,
    LUNG,
    ABDOMINAL,
    CUSTOM
};

enum resolution_t {
    _128X128,
    _256X256,
    _384X384,
    _512X512,
    _768X768
};

namespace hvg { class context_t; }

namespace data {
    namespace scan {
        struct scan_t {
            typedef unsigned short pixel_t;
            static const int N_IMAGES = 360;

            int id;
            int width;
            int height;
            pixel_t* images;
            float angles[N_IMAGES];           // in degree
            int index;
        };

        void init(scan_t* scan, int width, int height);
        scan_t* alloc();
        void drop(scan_t* scan);
        scan_t::pixel_t* n_image(scan_t* scan, int n);
    };

    const char* fpd_status_list[] = { "unconnected", "connecting", "ready", "error" };
    const char* hvg_status_list[] = { "unconnected", "connecting", "ready", "exposure", "error" };
    const char* cbct_mode_list[] = { "Head", "Lung", "Abdominal", "Custom" };
    const char* resolution_list[] = { "128x128", "256x256", "384x384", "512x512", "768x768" };

    struct app_stat_t {
        // FPD 
        fpd_status_t fpd;
        scan::scan_t* scan = NULL;

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
        websocket::websocket_t socket;

        // window

        // Camera
        rs2::pipeline camera;
    };

    static app_stat_t g_app_stat;

    bool init();
    void drop();
}

#endif    // CONSOLE_INCLUDE_DATA_H

#ifdef CONSOLE_DATA_IMPLEMENTATION
namespace data {
    namespace scan {
        void init(scan_t* scan, int width, int height)
        {
            scan->index = -1;
            scan->width = width;
            scan->height = height;
            int xx = sizeof(int);
            scan->images = (scan_t::pixel_t*)calloc(scan->N_IMAGES, scan->width * scan->height * sizeof(scan_t::pixel_t));
            if (scan->images == NULL) {
                // error handling
            }
        }

        scan_t* alloc()
        {
            scan_t* scan = (scan_t*)malloc(sizeof(scan_t));
            if (scan == NULL) {
                // error handling
            }
            return scan;
        }

        void drop(scan_t* scan)
        {
            if (scan->images != NULL) {
                free(scan->images);
            }
            else {
                // error handling
            }
        }

        scan_t::pixel_t* n_image(scan_t* scan, int n)
        {
            if (n >= scan_t::N_IMAGES || n < 0) {
                return NULL;
            }
            return scan->images + (size_t)scan->width * scan->height * sizeof(scan_t::pixel_t) * n;
        }
    }
    bool init()
    {
        g_app_stat.fpd = FPD_UNCONNECTED;
        g_app_stat.hvg = HVG_UNCONNECTED;
        g_app_stat.kv = 70.0f;
        g_app_stat.mAs = 5.0f;
        g_app_stat.cbct_mode = CUSTOM;
        g_app_stat.serial_port = 3;
        g_app_stat.serail_baud = 19200;
        g_app_stat.hvg_context = NULL;
        g_app_stat.resolution = _512X512;
        g_app_stat.slice_dist = 2.5f;

        g_app_stat.scan = scan::alloc();
        scan::init(g_app_stat.scan, 3072, 3072);

        // Configure and start the pipeline
        g_app_stat.camera.start();

        return true;
    }

    void drop()
    {
        scan::drop(g_app_stat.scan);
    }
}

#endif   // CONSOLE_DATA_IMPLEMENTATION
