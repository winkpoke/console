#ifndef _CONSOLE_SCAN_H_
#define _CONSOLE_SCAN_H_

#include "cl.h"
#include "sil.h"

namespace modal {
    struct scan_t {
        using pixel_t = cl::u16;
        static const int N_IMAGES = 360;

        cl::usize id;
        cl::usize width;
        cl::usize height;
        cl::f64 x_res;          // pixel resolution along X direction
        cl::f64 y_res;          // pixel resolution along Y direction
        //cl::usize n_images;
        cl::i64 index;        // current image index
        cl::f64* angles;        // in degree
        pixel_t* images;
        cl::i64 capacity;

        //std::list<std::pair<cl::f64, sil::image_t<pixel_t>>>
    };

    bool init(scan_t* scan, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res, cl::usize n_images);
    bool init(scan_t* scan, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res, cl::usize n_images, 
              const cl::f64* angles, scan_t::pixel_t* raw_data);
    void drop(scan_t* scan);

    cl::usize capacity(const scan_t* scan);
    cl::usize len(const scan_t* scan);
    bool is_empty(const scan_t* scan);
    void rewind(scan_t* scan);
    scan_t::pixel_t* get_data_at(scan_t* scan, int n);
    cl::i64 push_data(scan_t* scan, scan_t::pixel_t* data, cl::f64 angle);
}

#endif // !_CONSOLE_SCAN_H_


#ifdef MODAL_SCAN_IMPLEMENTATION
#ifndef MODAL_SCAN_IMPLEMENTED
#define MODAL_SCAN_IMPLEMENTED
namespace modal {
    bool init(scan_t* scan, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res, cl::usize capacity)
    {
        assert(scan);

        scan->index = -1;
        scan->width = width;
        scan->height = height;
        scan->x_res = x_res;
        scan->y_res = y_res;
        scan->capacity = capacity;
        scan->angles = cl::alloc<cl::f64>(scan->capacity);
        scan->images = cl::alloc<scan_t::pixel_t>(scan->capacity * scan->width * scan->height);
        if (scan->images == NULL) {
            // error handling
            return false;
        }
        return true;
    }

    bool init(scan_t* scan, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res, cl::usize size, 
              const cl::f64* angles, scan_t::pixel_t* raw_data)
    {
        assert(scan);
        assert(angles);
        assert(raw_data);

        scan->index = size - 1;
        scan->width = width;
        scan->height = height;
        scan->x_res = x_res;
        scan->y_res = y_res;
        scan->capacity = size;
        scan->angles = cl::alloc<cl::f64>(scan->capacity);
        memcpy(scan->angles, angles, size * sizeof(cl::f64));
        scan->images = raw_data;

        return true;
    }

    void drop(scan_t* scan)
    {
        if (scan) {
            cl::dealloc(scan->images);
            cl::dealloc(scan->angles);
        }
    }

    cl::usize capacity(const scan_t* scan)
    {
        assert(scan);
        return scan->capacity;
    }

    cl::usize len(const scan_t* scan)
    {
        assert(scan);
        if (scan->index < 0) {
            return 0;
        }
        else {
            return static_cast<cl::usize>(scan->index + 1);
        }
    }

    bool is_empty(const scan_t* scan)
    {
        assert(scan);
        return scan->index == -1;
    }

    void rewind(scan_t* scan)
    {
        assert(scan);
        scan->index = -1;
    }

    scan_t::pixel_t* get_data_at(scan_t* scan, int n)
    {
        assert(scan);
        if (n >= capacity(scan) || n < 0) {
            return NULL;
        }
        return scan->images + (size_t)scan->width * scan->height * n;
    }
    
    cl::i64 push_data(scan_t* scan, scan_t::pixel_t* data, cl::f64 angle)
    {
        assert(scan);
        if (scan->index + 1 >= scan->capacity) {
            return -1;
        }
        auto ptr = get_data_at(scan, scan->index + 1);
        if (ptr == nullptr) {
            return -1;
        }

        scan->index++;
        memcpy((void*)ptr, (void*)data, scan->width * scan->height * sizeof(scan_t::pixel_t));

        scan->angles[scan->index] = angle;

        return scan->index;
    }
}
#endif // !MODAL_SCAN_IMPLEMENTED
#endif // MODAL_SCAN_IMPLEMENTATION

