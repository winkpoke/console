#ifndef _MOD_HND_MODAL_INCLUDE_H_
#define _MOD_HND_MODAL_INCLUDE_H_

#include "cl.h"
#include "sil.h"
#include "hnd.h"

namespace mod::hnd::modal {
    struct hnd_t {
        static const cl::usize HEADER_LEN = 1024;
        hnd_header_t* header;
        cl::usize data_len;     // length of the compressed data
        cl::u8* data;
    };

    bool init(hnd_t* p);

    template <class T>
    bool init(hnd_t* p, sil::image_t<T>* img, double x_res, double y_res, double angle)
    {
        assert(p);
        assert(img);
        p->header = hnd_header_build();
        assert(p->header);
        hnd_header_set_width(p->header, img->width);
        hnd_header_set_height(p->header, img->height);
        hnd_header_set_x_res(p->header, x_res);
        hnd_header_set_y_res(p->header, y_res);
        hnd_header_set_angle(p->header, angle);
        cl::usize hnd_data_size = 0;
        constexpr cl::usize s = sizeof(T);
        static_assert(s == 2 || s == 4, 
            "only be able to handle images with 16/32 bits pixel data");
        cl::u8* hnd_data = encode((const uint8_t*)sil::get_data(img), img->width, img->height, s, (uintptr_t*)&hnd_data_size);

        p->data_len = hnd_data_size;
        p->data = hnd_data;

        return true;
    }
    
    void drop(hnd_t* p);
}

namespace mod::hnd::control {
    cl::usize write_to_file(mod::hnd::modal::hnd_t* p, const char* path);

    //struct hnd_t

}

#endif //!_MOD_HND_MODAL_INCLUDE_H_

#ifdef MOD_HND_MODAL_IMPLEMENTATION
#ifndef MOD_HND_MODAL_IMPLEMENTED
#define MOD_HND_MODAL_IMPLEMENTED

namespace mod::hnd::modal {
    bool init(hnd_t* p)
    {
        assert(p);
        p->header = hnd_header_build();
        return true;
    }

    void drop(hnd_t* p)
    {
        assert(p);
        if (p) {
            hnd_header_drop(p->header);
            cl::dealloc(p->data);
        }
    }
}

namespace mod::hnd::control {
    cl::usize write_to_file(mod::hnd::modal::hnd_t* p, const char* path)
    {
        assert(p);

        using hnd_t = mod::hnd::modal::hnd_t;

        FILE* fpw = fopen(path, "wb+");
        if (fpw == nullptr) {
            return 0;
        }

        char* header_buf = (char*)hnd_header_to_raw(p->header);
        cl::usize n = 0;
        n = fwrite(header_buf, sizeof(cl::u8), hnd_t::HEADER_LEN, fpw);
        if (n < hnd_t::HEADER_LEN) {
            // write error 
            return 0;
        }

        n = fwrite(p->data, sizeof(cl::u8), p->data_len, fpw);
        if (n < p->data_len) {
            // write error
            return 0;
        }
        fflush(fpw);
        fclose(fpw);

        hnd_header_raw_drop((uint8_t*)header_buf);
        return hnd_t::HEADER_LEN + p->data_len;
    }
}

#endif // !MOD_HND_MODAL_IMPLEMENTATION
#endif //MOD_HND_MODAL_IMPLEMENTATION
