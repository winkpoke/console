#pragma once

#include <memory>
#include <array>

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
        if (!p) {
            return nullptr;
        }
        if (!T::init(p, args...)) {
            T::drop(p);
            return nullptr;
        }
        return p;
    }

    template <class T, class... Args>
    std::shared_ptr<T> build_shared(Args... args)
    {
        auto p = build_raw<T>(args...);
        return std::shared_ptr<T>(p, T::drop);
    }

    template <class T, class... Args>
    std::unique_ptr<T, decltype(&T::drop)> build_unique(Args... args)
    {
        auto p = build_raw<T>(args...);
        return std::unique_ptr<T, decltype(&T::drop)>(p, T::drop);
    }

    // error handing:
    // error class shall have following fields:
    // 1.  enum e_error
    // 2.  const char* [] desc
    // 3.  static int last_error
    //template <class E>
    //void set_error(E::error_t e)
    //{
    //    E::last_error = e;
    //}

    //template <class E>
    //E::error_t last_error()
    //{
    //    return E::last_error;
    //}

    //template <class E>
    //const char* error_str(E::error_t e)
    //{
    //    return E::desc[e];
    //}
}

namespace cl { namespace math {
    
} } // namespace cl::math
