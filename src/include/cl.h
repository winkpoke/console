#ifndef _COMMON_LIBRARY_H_
#define _COMMON_LIBRARY_H_

#include <stdint.h>

#include <cassert>
#include <iostream>
#include <memory>
#include <array>
#include <string>
#include <map>
#include <list>
#include <functional>
#include <thread>
#include <future>
#include <mutex>
#include <shared_mutex>
#include <set>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

namespace cl {
    // type define 
    typedef int8_t      i8;
    typedef int16_t     i16;
    typedef int32_t     i32;
    typedef int64_t     i64;
    typedef uint8_t     u8;
    typedef uint16_t    u16;
    typedef uint32_t    u32;
    typedef uint64_t    u64;
    typedef uint64_t    usize;
    typedef float       f32;
    typedef double      f64;

    // data lifetime management
#if !defined(NDEBUG)
    static std::map<void*, std::string> g_allocation;
#endif 

    template <class T>
    inline T* alloc()
    {
        constexpr usize s = sizeof(T);
        const auto p = (T*)calloc(1, s);
#if !defined(NDEBUG)
        if (p) {
            g_allocation.insert(std::make_pair(p, typeid(T).name()));
            SPDLOG_DEBUG(u8"allocator: successfully allocated {:d} bytes for {:s} at {:p}", s, typeid(T).name(), (void*)p);
        }
        else {
            SPDLOG_ERROR("allocator: failed to allocate {:d} bytes for {:s} ", s, typeid(T).name());
        }
#endif 
        return p;
    }

    template <class T>
    inline T* alloc(usize n)
    {
        constexpr size_t s = sizeof(T);
        const auto p = (T*)calloc(n, s);
#if !defined(NDEBUG)
        if (p) {
            g_allocation.insert(std::make_pair(p, typeid(T).name()));
            SPDLOG_DEBUG(u8"allocator: successfully allocated {:d} {:s} with {:d} bytes each at {:p}", n, typeid(T).name(), s, (void*)p);
        }
        else {
            SPDLOG_ERROR("allocator: failed to allocate {:d} {:s} with {:d} bytes each ", n, typeid(T).name(), s);
        }
#endif 
        return p;

    }

    template<class T>
    void dealloc(T* p)
    {
#if !defined(NDEBUG)
        if (p) {
            g_allocation.erase(p);
            SPDLOG_DEBUG(u8"deallocator: deallocate {:s} at {:p}", typeid(T).name(), (void*)p);
        }
        else {
            SPDLOG_ERROR("deallocator: deallocate a NULL pointer with type of {:s} ", typeid(T).name());
        }
#endif 
        free(p);
    }

    template <class T, class... Args>
    T* build_raw(Args... args)
    {
        const auto p = (T*)alloc<T>();
        if (!p) {
            return nullptr;
        }
        if (!init(p, args...)) {
            drop(p);
            return nullptr;
        }
        return p;
    }

    template <class T, class... Args>
    T* build_raw(bool(*init)(T*, Args...), void(*drop)(T*), Args... args)
    {
        auto p = (T*)alloc<T>();
        memset(p, 0, sizeof(T));
        if (!p) {
            return nullptr;
        }
        if (!init(p, args...)) {
            drop(p);
            return nullptr;
        }
        return p;
    }

    template <class T>
    using unique_ptr = std::unique_ptr<T, void(*)(T*)>;

    template <class T>
    using shared_ptr = std::shared_ptr<T>;

    template <class T, class... Args>
    cl::unique_ptr<T> build_unique(Args... args)
    {
        auto p = build_raw<T>(args...);
        auto f = [](T* p)
        {
            drop(p);
        };
        return cl::unique_ptr<T>(p, f);
    }

    template <class T, class... Args>
    cl::shared_ptr<T> build_shared(Args... args)
    {
        auto p = build_raw<T>(args...);
        auto f = [](T* p)
        {
            drop(p);
        };
        return std::shared_ptr<T>(p, f);
    }

    template <class T, class... Args>
    std::unique_ptr<T, void(*)(T*)> build_unique(void(*drop)(T*), Args... args)
    {
        auto p = build_raw<T>(args...);
        return std::unique_ptr<T, decltype(drop)>(p, drop);
    }

    template <class T, class... Args>
    std::shared_ptr<T> build_shared(void(*drop)(T*), Args... args)
    {
        auto p = build_raw<T>(args...);
        return std::shared_ptr<T>(p, drop);
    }


    //template <class T, class... Args>
    //std::unique_ptr<T, std::function<void(T*)>> build_unique(std::function<void(T*)> drop, Args... args)
    //{
    //    auto p = build_raw<T>(args...);
    //    return std::unique_ptr<T, decltype(drop)>(p, drop);
    //}

    //template <class T, class... Args>
    //std::shared_ptr<T> build_shared(std::function<void(T*)> drop, Args... args)
    //{
    //    auto p = build_raw<T>(args...);
    //    return std::shared_ptr<T>(p, drop);
    //}

    template <class T>
    cl::unique_ptr<T> build_unique(T* p)
    {
        auto f = [](T* p)
        {
            drop(p);
        };
        return cl::unique_ptr<T>(p, f);
    }

    template <class T>
    cl::shared_ptr<T> build_shared(T* p)
    {
        auto f = [](T* p)
        {
            drop(p);
        };
        return std::shared_ptr<T>(p, f);
    }

    template <class T>
    std::unique_ptr<T, void(*)(T*)> build_unique(void(*drop)(T*), T* p)
    {
        assert(p);
        return std::unique_ptr<T, decltype(drop)>(p, drop);
    }

    template <class T>
    std::shared_ptr<T> build_shared(void(*drop)(T*), T* p)
    {
        assert(p);
        return std::shared_ptr<T>(p, drop);
    }
    
    template <class... Args>
    bool retry(int n, std::function<bool (Args...)> f, Args... args)
    {
        while (n-- > 0) {
            if (f(args)) {
                return true;
            }
        }
        return false;
    }

    class runtime_wrapper_base {
    public:
        runtime_wrapper_base(std::string name, std::string version)
            :_name(name), _version(version) {}
        virtual ~runtime_wrapper_base() = default;
        std::string get_name() { return _name; }
        std::shared_mutex& get_mutex() { return _mutex; }
    private:
        std::string _name;
        std::string _version;
        std::shared_mutex _mutex;
    };

    template <class T>
    class runtime_wrapper : public runtime_wrapper_base {
    public:
        runtime_wrapper(cl::shared_ptr<T> data, std::string name, std::string version)
            :_data(data), runtime_wrapper_base(name, version)
        {
            //_data.reset(data.release());
        }
        virtual ~runtime_wrapper() {  }
        cl::shared_ptr<T> get() { return _data; }
    private:
        cl::shared_ptr<T> _data;

    };

    struct runtime_object_t {
        std::map<std::string, std::unique_ptr<runtime_wrapper_base>> data;
        std::shared_mutex mutex;
    };

    static bool init(runtime_object_t* p)
    {
        new(&p->data)std::map<std::string, std::unique_ptr<runtime_wrapper_base>>();
        new(&p->mutex)std::shared_mutex();
        return true;
    }

    static void drop(runtime_object_t* p)
    {
        assert(p);
        p->data.~map<std::string, std::unique_ptr<runtime_wrapper_base>>();
        p->mutex.~shared_mutex();
        cl::dealloc(p);
    }

    template <class T>
    bool mount(runtime_object_t* d, cl::shared_ptr<T> p, std::string str, std::string ver)
    {
        assert(p);
        assert(d);
        std::scoped_lock<std::shared_mutex> lk(d->mutex);
        auto iter = d->data.find(str);
        if (iter != d->data.end()) {
            return false;
        }

        d->data[str] = std::unique_ptr<runtime_wrapper_base>(new runtime_wrapper<T>(p, str, ver));
        return true;
    }

    static bool unmount(runtime_object_t* d, std::string str)
    {
        std::scoped_lock<std::shared_mutex> lk(d->mutex);
        d->data.erase(str);
        return true;
    }

    static bool unmount_all(runtime_object_t* d)
    {
        std::scoped_lock<std::shared_mutex> lk(d->mutex);
        d->data.clear();
        return true;
    }

    template <class T>
    cl::shared_ptr<T> get(runtime_object_t* d, std::string str)
    {
        assert(d);
        std::shared_lock(d->mutex);
        auto iter = d->data.find(str);
        if (iter != d->data.end()) {
            runtime_wrapper_base* p = d->data[str].get();
            runtime_wrapper<T>* ptr = dynamic_cast<runtime_wrapper<T>*>(p);
            if (ptr != nullptr) {
                return ptr->get();
            }
        }
        return nullptr;
    }
}

namespace cl { namespace math {
    
} } // namespace cl::math
#endif // !_COMMON_LIBRARY_H_