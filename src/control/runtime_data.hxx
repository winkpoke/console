#ifndef MODAL_RUNTIME_DATA_INCLUDE_H
#define MODAL_RUNTIME_DATA_INCLUDE_H

#include <memory>
#include <any>
#include <mutex>
#include <shared_mutex>

#include "cl.h"
#include "modal/scan.hxx"

#include "control/websocket.hxx"
#include "mod/fpd/control/fpd.hxx"
#include "mod/hvg/control/hvg.hxx"


namespace hvg { struct context_t; }

namespace control {
    struct runtime_data_t {
        std::shared_mutex mutex;
        cl::runtime_object_t* objects;
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
        return true;
    }

    void drop(runtime_data_t* d) 
    {
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
