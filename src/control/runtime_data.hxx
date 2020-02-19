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

    struct runtime_wrapper_t {
        using drop_fun_t = void(*)(void*);
        std::string name;
        std::string version;
        std::shared_mutex mutex;
        void* object;
        drop_fun_t drop;
        virtual ~runtime_wrapper_t() = default;
    };

    void init(runtime_wrapper_t* p, std::string name, std::string);

    template <class T>
    void drop(void* p)
    {
        drop(static_cast<T*>(p));
    }

    //template <class T>
    //struct runtime_wrapper_impl_t : public runtime_wrapper_t {
    //    T* object;
    //    virtual ~runtime_wrapper_impl_t() { drop(object); }
    //};

    //std::string get_name(const runtime_wrapper_t* obj)
    //{
    //    assert(obj);
    //    return obj->name;
    //}

    class RuntimeWrapperBase {
    public:
        RuntimeWrapperBase(std::string name, std::string version)
            :_name(name), _version(version) {}
        virtual ~RuntimeWrapperBase() = default;
        std::string get_name() { return _name; }
        std::shared_mutex& get_mutex() { return _mutex; }
    private:
        std::string _name;
        std::string _version;
        std::shared_mutex _mutex;
    };

    template <class T>
    class RuntimeWrapper : public RuntimeWrapperBase {
    public:
        RuntimeWrapper(T* data, std::string name, std::string version)
            :_data(data), RuntimeWrapperBase(name, version)
        {
            //_data.reset(data.release());
        }
        virtual ~RuntimeWrapper() { drop(_data); }
        
        T* get() { return _data; }      

    private:
        // cl::unique_ptr<T> _data;
        T* _data;

    };

    //template <class T>
    //RuntimeWrapperBase* build_runtime_wrapper(T* obj, std::string name, std::string version)
    //{
    //    
    //}


    struct runtime_data_t {
        std::shared_mutex mutex;

        std::map<std::string, std::unique_ptr<RuntimeWrapperBase>> runtime_data;

        cl::runtime_object_t* runtime_data2;

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
    bool register_data(runtime_data_t* p, cl::unique_ptr<T> d, std::string str, std::string ver)
    {
        assert(p);
        std::scoped_lock(p->mutex);
        auto iter = p->runtime_data.find(str);
        if (iter != p->runtime_data.end()) {
            return false;
        }

        p->runtime_data[str] = std::unique_ptr<RuntimeWrapperBase>(new RuntimeWrapper<T>(d.release(), str, ver));
        return true;
    }

    template <class T>
    T* get(runtime_data_t* p, std::string str)
    {
        assert(p);
        std::shared_lock(p->mutex);
        auto iter = p->runtime_data.find(str);
        if (iter != p->runtime_data.end()) {
            T* ptr = dynamic_cast<RuntimeWrapper<T>*>(p->runtime_data[str].get())->get();
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
        new(&d->mutex)std::shared_mutex();
        std::unique_lock(d->mutex);
        new(&d->runtime_data)std::map<std::string, std::unique_ptr<RuntimeWrapperBase>>();

        d->runtime_data2 = cl::build_raw<cl::runtime_object_t>();
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
        drop(d->runtime_data2);
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
