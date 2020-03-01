#pragma once

#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <new>

#include "sil.h"

using image_u16_t = sil::image_t<cl::u16>;
struct hnd_header_t;

extern "C" {

    uint32_t addition(uint32_t a, uint32_t b);

    uint8_t* encode(uint8_t* img,
        uintptr_t width,
        uintptr_t height,
        uintptr_t channel,
        uintptr_t* size);

    uintptr_t func(const image_u16_t* image);

    hnd_header_t* hnd_header_build();

    void hnd_header_drop(hnd_header_t* ptr);

    void hnd_header_set_angle(hnd_header_t* ptr, double angle);

    void hnd_header_set_height(hnd_header_t* ptr, uint32_t height);

    void hnd_header_set_width(hnd_header_t* ptr, uint32_t width);

    void hnd_header_set_x_res(hnd_header_t* ptr, double x_res);

    void hnd_header_set_y_res(hnd_header_t* ptr, double y_res);

    uint8_t* hnd_header_to_raw(hnd_header_t* ptr);

    void hnd_header_raw_drop(uint8_t* ptr);

} // extern "C"
