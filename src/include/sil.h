#pragma once

// Simple Image Library

#include <memory>

#include "cl.h"

namespace sil {
    using namespace std;
    using namespace cl;
    
    struct drop_policy {
        struct strong {
            template <class T>
            static inline void drop(T* p)
            {
                cl::dealloc(p);
            }
        };
        struct weak {
            template <class T>
            static inline void drop(T* p)
            {
            }
        };
    };
    
    template <class pixel_t, class MS = sil::drop_policy::strong>
    struct image_t {
        u16 channel;
        cl::usize height;
        cl::usize width;
        pixel_t* data;
    };

    template <class T>
    bool init(image_t<T>* image, size_t width, size_t height, u16 channel, T* data)
    {
        if (image == NULL) {
            return false;
        }

        if (channel != 1 && channel != 3 && channel != 4) {
            return false;
        }

        image->channel = channel;

        image->height = height;
        image->width = width;

        if (data == nullptr) {
            image->data = cl::alloc<T>(width * height * channel);
        }
        else {
            image->data = data;
        }

        return true;
    }

    template <class T, class DP>
    bool init(image_t<T, sil::drop_policy::weak>* image, image_t<T, DP> another)
    {
        assert(!image || !another);
        if (image == NULL || another == NULL) {
            return false;
        }
        image->channel = another->channel;

        image->height = another->height;
        image->width = another->width;

        assert(!another->data);
        image->data = another->data;
    }

    template <class T, class DP>
    void drop(image_t<T, DP>* image)
    {
        if (image) {

            DP::drop(image->data);
            cl::dealloc(image);
        }
    }

    template <class T, class DP>
    image_t<T>* clone(image_t<T, DP>* image)
    {
        image_t<T>* p = cl::build_raw<image_t<T>>(image->width, image->height, image->channel, (T*)nullptr);
        size_t s = sizeof(T) * image->width * image->height * image->channel;
        memcpy(p->data, image->data, s);
        return p;
    }

    template <class T>
    cl::usize width(image_t<T>* p)
    {
        assert(p);
        return p->width;
    }

    template <class T>
    cl::usize height(image_t<T>* p)
    {
        assert(p);
        return p->height;
    }

    template <class T>
    cl::usize channel(image_t<T>* p)
    {
        assert(p);
        return p->channel;
    }
}

