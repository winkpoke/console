#pragma once

#include <memory>
namespace cl {
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
}
