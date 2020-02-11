#ifndef _CONSOLE_SCAN_H_
#define _CONSOLE_SCAN_H_

#include "cl.h"

namespace modal {
    struct scan_t {
        typedef unsigned short pixel_t;
        static const int N_IMAGES = 360;

        int id;
        int width;
        int height;
        float angles[N_IMAGES];           // in degree
        int index;
        pixel_t* images;
    };

    bool init(scan_t* scan, int width, int height);
    void drop(scan_t* scan);
    scan_t::pixel_t* n_image(scan_t* scan, int n);


    // Implementations
    bool init(scan_t* scan, int width, int height);

    void drop(scan_t* scan);

    scan_t::pixel_t* n_image(scan_t* scan, int n);
}

#endif // !_CONSOLE_SCAN_H_


#ifdef MODAL_SCAN_IMPLEMENTATION
#ifndef MODAL_SCAN_IMPLEMENTED
#define MODAL_SCAN_IMPLEMENTED
namespace modal {
    // Implementations
    bool init(scan_t* scan, int width, int height)
    {
        scan->index = -1;
        scan->width = width;
        scan->height = height;
        //int xx = sizeof(int);
        scan->images = (scan_t::pixel_t*)calloc(scan->N_IMAGES, scan->width * scan->height * sizeof(scan_t::pixel_t));
        if (scan->images == NULL) {
            // error handling
            return false;
        }
        return true;
    }

    void drop(scan_t* scan)
    {
        if (scan->images != NULL) {
            free(scan->images);
        }
        free(scan);
    }

    scan_t::pixel_t* n_image(scan_t* scan, int n)
    {
        if (n >= scan_t::N_IMAGES || n < 0) {
            return NULL;
        }
        return scan->images + (size_t)scan->width * scan->height * sizeof(scan_t::pixel_t) * n;
    }
}
#endif // !MODAL_SCAN_IMPLEMENTED
#endif // MODAL_SCAN_IMPLEMENTATION

