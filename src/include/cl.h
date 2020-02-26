#ifndef _COMMON_LIBRARY_H_
#define _COMMON_LIBRARY_H_

#include <cassert>
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

namespace cl {
    // type define 
    typedef char                i8;
    typedef short               i16;
    typedef int                 i32;
    typedef long long           i64;
    typedef unsigned char       u8;
    typedef unsigned short      u16;
    typedef unsigned int        u32;
    typedef unsigned long long  u64;
    typedef float               f32;
    typedef double              f64;

    // data lifetime management
    template <class T>
    T* alloc()
    {
        const size_t s = sizeof(T);
        return (T*)malloc(s);
    }


    template <class T, class... Args>
    T* build_raw(Args... args)
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

    template <class T, class... Args>
    T* build_raw(bool(*init)(T*, Args...), void(*drop)(T*), Args... args)
    {
        auto p = (T*)alloc<T>();
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
        free(p);
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