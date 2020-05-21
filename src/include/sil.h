#ifndef _SIL_LIB_INCLUDE_H_
#define _SIL_LIB_INCLUDE_H_

// Simple Image Library

#include <memory>
#include <limits>
#include <algorithm>
#include <cmath>

#include "cl.h"

namespace sil {
    using namespace std;
    using namespace cl;
    
    template <class pixel_t>
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

        T* d = data;
        if (data == nullptr) {
            d = cl::alloc<T>(width * height * channel);
            if (!d) {
                return false;
            }
        }
        image->data = data;

        return true;
    }


    template <class T>
    void drop(image_t<T>* image)
    {
        if (image) {
            cl::dealloc(image->data);
        }
    }

    template <class T>
    image_t<T>* clone(image_t<T>* image)
    {
        if (!image) {
            return nullptr;
        }
        const cl::usize len = image->width * image->height * image->channel;
        T* image_data = cl::alloc<T>(len);
        if (!image_data) {
            return nullptr;
        }
        memcpy(image_data, image->data, len * sizeof(T));
        return cl::build_raw<image_t<T>>(image->width, image->height, image->channel, image_data);
    }

    template <class T>
    cl::usize get_width(image_t<T>* p)
    {
        assert(p);
        return p->width;
    }

    template <class T>
    cl::usize get_height(image_t<T>* p)
    {
        assert(p);
        return p->height;
    }

    template <class T>
    cl::usize get_channel(image_t<T>* p)
    {
        assert(p);
        return p->channel;
    }

    template <class T>
    const T* get_data(image_t<T>* p)
    {
        assert(p);
        return p->data;
    }

    template <class T>
    void winlev(image_t<T>* p, cl::i64 win, cl::i64 lev, image_t<cl::u8>** output)
    {
        assert(p);
        assert(p->channel == 1);  // only support 1 channel image

        const T lower = std::max(lev - win, std::numeric_limits<T>::min);
        const T upper = std::min(lev + win, std::numeric_limits<T>::max);

        assert(lower >= std::numeric_limits<T>::min);
        assert(upper <= std::numeric_limits<T>::max);

        output = cl::build_raw<image_t<cl::u8>>(p->width, p->height, 1);

        T* src = p->data;
        cl::u8* dst = output->date;
        const i32 k = 255.0 * (upper - lower);
        const cl::usize size = p->width * p->height;
        for (int i = 0; i < size; ++i) {
            dst[i] = std::round(k * src[i]);
        }
    }
}

#endif //!_SIL_LIB_INCLUDE_H_
