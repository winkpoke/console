#ifndef _COMMON_LIBRARY_H_
#define _COMMON_LIBRARY_H_

#include <memory>
#include <array>
#include <string>
#include <map>
#include <list>
#include <functional>
#include <thread>
#include <future>

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

    //template <class T, class... Args>
    //T* build_raw(Args... args)
    //{
    //    auto p = (T*)alloc<T>();
    //    if (!p) {
    //        return nullptr;
    //    }
    //    if (!T::init(p, args...)) {
    //        T::drop(p);
    //        return nullptr;
    //    }
    //    return p;
    //}

    //template <class T, class... Args>
    //std::shared_ptr<T> build_shared(Args... args)
    //{
    //    auto p = build_raw<T>(args...);
    //    return std::shared_ptr<T>(p, T::drop);
    //}

    //template <class T, class... Args>
    //std::unique_ptr<T, decltype(&T::drop)> build_unique(Args... args)
    //{
    //    auto p = build_raw<T>(args...);
    //    return std::unique_ptr<T, decltype(&T::drop)>(p, T::drop);
    //}

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

    template <class T, class... Args>
    std::unique_ptr<T, std::function<void(T*)>> build_unique(std::function<void(T*)> drop, Args... args)
    {
        auto p = build_raw<T>(args...);
        return std::unique_ptr<T, decltype(drop)>(p, drop);
    }

    template <class T, class... Args>
    std::shared_ptr<T> build_shared(std::function<void(T*)> drop, Args... args)
    {
        auto p = build_raw<T>(args...);
        return std::shared_ptr<T>(p, drop);
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
    
    template <class T>
    std::unique_ptr<T, std::function<void(T*)>> build_unique(std::function<void(T*)> drop, T* p)
    {
        assert(p);
        return std::unique_ptr<T, decltype(drop)>(p, drop);
    }

    template <class T>
    std::shared_ptr<T> build_shared(std::function<void(T*)> drop, T* p)
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
}

namespace cl { namespace math {
    
} } // namespace cl::math
#endif // !_COMMON_LIBRARY_H_