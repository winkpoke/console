#pragma once

#include <memory>
namespace cl {

    template <class T, class... Args>
    T* make_raw(Args... args)
    {
        T* p = alloc();
        if (!image) {
            return nullptr;
        }
        if (!init(p, args)) {
            drop(p);
            return nullptr;
        }
        return p;
    }

    template <class T, class... Args>
    std::shared_ptr<T> make_shared(Args... args)
    {
        auto p = make_raw(args);
        return std::shared_ptr<image_t<T>>(p, drop);
    }

    template <class T, class... Args>
    std::unique_ptr<T> make_unique(Args... args)
    {
        auto image = make_raw(args);
        return std::unique_ptr<T>(p, drop);
    }
}
