#ifndef _CONSOLE_SCAN_H_
#define _CONSOLE_SCAN_H_

#include "cl.h"

namespace modal {
    struct scan_t {
        using pixel_t = cl::u16;
        static const int N_IMAGES = 360;

        cl::usize id;
        cl::usize width;
        cl::usize height;
        cl::f64 x_res;          // pixel resolution along X direction
        cl::f64 y_res;          // pixel resolution along Y direction
        cl::usize n_images;
        cl::usize index;        // current image index
        cl::f64* angles;        // in degree
        pixel_t* images;
    };

    bool init(scan_t* scan, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res, cl::usize n_images);
    bool init(scan_t* scan, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res, cl::usize n_images, 
              const cl::f64* angles, scan_t::pixel_t* raw_data);
    void drop(scan_t* scan);

    scan_t::pixel_t* n_image(scan_t* scan, int n);

    void drop(scan_t* scan);

    scan_t::pixel_t* get_image_at(scan_t* scan, int n);
}

#endif // !_CONSOLE_SCAN_H_


#ifdef MODAL_SCAN_IMPLEMENTATION
#ifndef MODAL_SCAN_IMPLEMENTED
#define MODAL_SCAN_IMPLEMENTED
namespace modal {
    bool init(scan_t* scan, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res, cl::usize n_images)
    {
        assert(scan);

        scan->index = -1;
        scan->width = width;
        scan->height = height;
        scan->x_res = x_res;
        scan->y_res = y_res;
        scan->n_images = n_images;
        scan->angles = (cl::f64*)calloc(scan->n_images, sizeof(cl::f64));
        scan->images = (scan_t::pixel_t*)calloc(scan->n_images, scan->width * scan->height * sizeof(scan_t::pixel_t));
        if (scan->images == NULL) {
            // error handling
            return false;
        }
        return true;
    }

    bool init(scan_t* scan, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res, cl::usize n_images, 
              const cl::f64* angles, scan_t::pixel_t* raw_data)
    {
        assert(scan);
        assert(angles);
        assert(raw_data);

        scan->index = n_images;
        scan->width = width;
        scan->height = height;
        scan->x_res = x_res;
        scan->y_res = y_res;
        scan->n_images = n_images;
        scan->angles = (cl::f64*)calloc(scan->n_images, sizeof(cl::f64));
        memcpy(scan->angles, angles, n_images * sizeof(cl::f64));
        scan->images = raw_data;

        return true;
    }

    void drop(scan_t* scan)
    {
        if (scan) {
            free(scan->images);
            free(scan->angles);
        }
        free(scan);
    }

    scan_t::pixel_t* get_image_at(scan_t* scan, int n)
    {
        if (n >= scan_t::N_IMAGES || n < 0) {
            return NULL;
        }
        return scan->images + (size_t)scan->width * scan->height * sizeof(scan_t::pixel_t) * n;
    }
}
#endif // !MODAL_SCAN_IMPLEMENTED
#endif // MODAL_SCAN_IMPLEMENTATION

