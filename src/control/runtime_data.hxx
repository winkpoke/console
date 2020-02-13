#ifndef MODAL_RUNTIME_DATA_INCLUDE_H
#define MODAL_RUNTIME_DATA_INCLUDE_H

#include <memory>
#include <any>
#include <mutex>
#include <shared_mutex>

// #include <librealsense2/rs.hpp>

#include "def.h"
#include "cl.h"
#include "modal/scan.hxx"

#include "control/websocket.hxx"
#include "fpd/fpd.hxx"
#include "hvg/hvg.hxx"


namespace hvg { struct context_t; }

namespace control {
    class RuntimeWrapper {
    public:
        //virtual void get() = 0;
        virtual ~RuntimeWrapper() = default;
    private:
        std::string _name;
    };

    template <class T>
    class RuntimeWrapperImpl : public RuntimeWrapper {
    public:
        RuntimeWrapperImpl(cl::unique_ptr<T> data):_data(move(data)),_mutex()
        {
            // _data.reset(data);
        }
        virtual ~RuntimeWrapperImpl() {}
        
        virtual T* get() { return _data.get(); }

        std::shared_mutex _mutex;

    private:
        cl::unique_ptr<T> _data;
    };


    struct runtime_data_t {
        std::shared_mutex mutex;

        std::map<std::string, std::unique_ptr<RuntimeWrapper>> runtime_data;

        //cl::unique_ptr<fpd::fpd_t> fpd;
        cl::unique_ptr<hvg::hvg_t> hvg;
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

    template <class T>
    bool register_data(runtime_data_t* p, std::string str, cl::unique_ptr<T> d)
    {
        assert(p);
        std::scoped_lock(p->mutex);
        auto iter = p->runtime_data.find(str);
        if (iter != p->runtime_data.end()) {
            return false;
        }

        // p->runtime_data[str] = std::unique_ptr<RuntimeWrapper>(new RuntimeWrapperImpl<T>(d.get()));
        p->runtime_data[str] = std::unique_ptr<RuntimeWrapper>(new RuntimeWrapperImpl<T>(std::move(d)));
        return true;
    }

    template <class T>
    T* get(runtime_data_t* p, std::string str)
    {
        assert(p);
        std::shared_lock(p->mutex);
        auto iter = p->runtime_data.find(str);
        if (iter != p->runtime_data.end()) {
            T* ptr = dynamic_cast<RuntimeWrapperImpl<T>*>(p->runtime_data[str].get())->get();
            return ptr;
        }
        else {
            return nullptr;
        }
    }

    bool unregister_data(runtime_data_t* p, std::string str);

    runtime_data_t* get_runtime_data();
}

#endif    // MODAL_RUNTIME_DATA_INCLUDE_H

#ifdef MODAL_RUNTIME_DATA_IMPLEMENTATION
#ifndef MODAL_RUNTIME_DATA_IMPLEMENTATED
#define MODAL_RUNTIME_DATA_IMPLEMENTATED

namespace control {
    bool init(runtime_data_t* d) {
        //d->hvg = HVG_UNCONNECTED;
        new(&d->mutex)std::shared_mutex();
        std::unique_lock(d->mutex);
        new(&d->runtime_data)std::map<std::string, std::unique_ptr<RuntimeWrapper>>();
        //d->fpd = cl::build_unique<fpd::fpd_t>(3072, 3072);
        d->hvg = cl::build_unique<hvg::hvg_t>(70.0f, 5.0f, nullptr);
        d->cbct_mode = CUSTOM;
        d->resolution = _512X512;
        d->slice_dist = 2.5f;

        // websocket
        d->socket = cl::build_raw<websocket::websocket_t>("ws://172.21.98.182:3000/ws");

        // Configure and start the pipeline
        //app->camera.start();

        return true;
    }

    void drop(runtime_data_t* d) 
    {
        // d->fpd is unique_ptr and would be destroyed automatically
        websocket::drop(d->socket);
        d->~runtime_data_t();
        free(d);
    }

    runtime_data_t* runtime_data_t::_this = cl::build_raw<runtime_data_t>();

    runtime_data_t* get_runtime_data()
    {
        if (!runtime_data_t::_this) {
            runtime_data_t::_this = cl::build_raw<runtime_data_t>();
        }
        return runtime_data_t::_this;
    }

    bool unregister_data(runtime_data_t* p, std::string str)
    {
        assert(p);
        std::scoped_lock(p->mutex);
        p->runtime_data.erase(str);
        return true;
    }
}
#endif // !MODAL_RUNTIME_DATA_IMPLEMENTATED
#endif // MODAL_RUNTIME_DATA_IMPLEMENTATION
