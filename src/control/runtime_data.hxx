#ifndef MODAL_RUNTIME_DATA_INCLUDE_H
#define MODAL_RUNTIME_DATA_INCLUDE_H

#include <memory>
#include <any>
#include <mutex>
#include <shared_mutex>

#include "cl.h"
#include "modal/scan.hxx"

#include "control/websocket.hxx"
#include "fpd/fpd.hxx"
#include "hvg/hvg.hxx"


namespace hvg { struct context_t; }

namespace control {
    struct runtime_data_t {
        std::shared_mutex mutex;

        cl::runtime_object_t* objects;
        cbct_mode_t cbct_mode;

        // Reconstruction 
        resolution_t resolution;
        float slice_dist;

        // Upstream server
        websocket::websocket_t* socket;

        // Camera
        //rs2::pipeline camera;

        static runtime_data_t* _this;
    };

    bool init(runtime_data_t* d);
    void drop(runtime_data_t* d);


    runtime_data_t* get_runtime_data();
}

#endif    // MODAL_RUNTIME_DATA_INCLUDE_H

#ifdef MODAL_RUNTIME_DATA_IMPLEMENTATION
#ifndef MODAL_RUNTIME_DATA_IMPLEMENTATED
#define MODAL_RUNTIME_DATA_IMPLEMENTATED

namespace control {
    bool init(runtime_data_t* d) {
        new(&d->mutex)std::shared_mutex();
        std::unique_lock(d->mutex);
        d->objects = cl::build_raw<cl::runtime_object_t>();
        d->cbct_mode = CUSTOM;
        d->resolution = _512X512;
        d->slice_dist = 2.5f;

        // websocket
        d->socket = cl::build_raw<websocket::websocket_t>("ws://172.17.214.17:3000/ws");

        // Configure and start the pipeline
        //app->camera.start();

        return true;
    }

    void drop(runtime_data_t* d) 
    {
        // d->fpd is unique_ptr and would be destroyed automatically
        cl::recycle(d->socket);
        cl::recycle(d->objects);
        d->~runtime_data_t();
    }

    runtime_data_t* runtime_data_t::_this = cl::build_raw<runtime_data_t>();

    runtime_data_t* get_runtime_data()
    {
        if (!runtime_data_t::_this) {
            runtime_data_t::_this = cl::build_raw<runtime_data_t>();
        }
        return runtime_data_t::_this;
    }

}
#endif // !MODAL_RUNTIME_DATA_IMPLEMENTATED
#endif // MODAL_RUNTIME_DATA_IMPLEMENTATION
