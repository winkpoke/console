#pragma once

// Simple Image Library

#include <memory>

namespace sil {
    using namespace std;

    template <class pixel_t>
    struct image_t {
        unsigned short channel;
        size_t height;
        size_t width;
        pixel_t* data;
    };

    template <class T>
    image_t<T>* alloc()
    {
        const size_t s = sizeof(image_t<T>);
        return (image_t<T>*)malloc(s);
    }

    template <class T>
    bool init(image_t<T>* image, size_t width, size_t height, unsigned short channel, T* data = nullptr)
    {
        if (image == NULL) {
            return false;
        }

        image->height = height;
        image->width = width;
        image->channel = channel;
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
    image_t<T>* make_raw(size_t width, size_t height, unsigned short channel, T* data = nullptr)
    {
        image_t<T>* image = (image_t<T>*)alloc<T>();
        if (!image) {
            return nullptr;
        }
        if (!init(image, width, height, channel, data)) {
            drop(image);
            return nullptr;
        }
        return image;
    }

    template <class T, class... Args>
    std::shared_ptr<image_t<T>> make_shared(Args... args)
    {
        auto p = make_raw(args...);
        return std::shared_ptr<image_t<T>>(p, drop<T>);
    }

    template <class T, class... Args>
    std::unique_ptr<image_t<T>> make_unique(Args... args)
    {
        auto p = make_raw(args...);
        return std::unique_ptr<image_t<T>>(p, drop<T>);
    }

    //template <class T>
    //shared_ptr<image_t<T>> make_shared(size_t width, size_t height, unsigned short channel, T* data = nullptr)
    //{
    //    auto image = make_raw(width, height, channel, data);
    //    shared_ptr<image_t<T>> p(image, drop<T>);
    //    return p;
    //}

    //template <class T>
    //unique_ptr<image_t<T>> make_unique(size_t width, size_t height, unsigned short channel, T* data = nullptr)
    //{
    //    auto image = make_raw(width, height, channel, data);
    //    return unique_ptr<image_t<T>>(image, drop<T>);
    //}
}

