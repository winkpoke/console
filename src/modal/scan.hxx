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

    cl::usize len(scan_t* scan);
    bool empty(scan_t* scan);
    scan_t::pixel_t* get_image_at(scan_t* scan, int n);
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

    cl::usize capacity(scan_t* scan)
    {
        assert(scan);
        return scan->capacity;
    }

    cl::usize len(scan_t* scan)
    {
        assert(scan);
        return scan->index + 1;
    }

    bool empty(scan_t* scan)
    {
        assert(scan);
        return scan->index == -1;
    }

    scan_t::pixel_t* get_image_at(scan_t* scan, int n)
    {
        if (n >= scan_t::N_IMAGES || n < 0) {
            return NULL;
        }
        return scan->images + (size_t)scan->width * scan->height * n;
    }
}
#endif // !MODAL_SCAN_IMPLEMENTED
#endif // MODAL_SCAN_IMPLEMENTATION

