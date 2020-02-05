#pragma once

// Simple Image Library

#include <memory>

#include "cl.h"

namespace sil {
    using namespace std;
    using namespace cl;

    //struct error_t {
    //    enum id {ERROR_NONE};

    //    static const char* const desc[] = {
    //        "hello",
    //    };
    //};

    template <class pixel_t>
    struct image_t {
        typedef pixel_t pixel_t;
        u16 channel;
        size_t height;
        size_t width;
        pixel_t* data;
    };

    template <class pixel_t>
    bool init(image_t<pixel_t>* image, size_t width, size_t height, u16 channel, pixel_t* data = nullptr);
    template <class pixel_t>
    void drop(image_t<pixel_t>* image);

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
            image->data = (T*)malloc(sizeof(T) * width * height * channel);
        }
        else {
            image->data = data;
        }

        return true;
    }

    template <class T>
    void drop(image_t<T>* image)
    {
        if (image) {
            if (image->data) {
                free(image->data);
            }
            free(image);
        }
    }

    template <class T>
    decltype(T::pixel_t) min_value()
    {
        return 10;
    }

    
}

