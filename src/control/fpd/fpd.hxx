#ifndef _CONSOLE_FPD_H_
#define _CONSOLE_FPD_H_

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "cl.h"
#include "def.h"

#include "iray.hxx"

#include "modal/scan.hxx"

namespace control::fpd {
    struct fpd_t {
        enum class status_e {
            FPD_UNCONNECTED,
            FPD_CONNECTING,
            FPD_READY,
            FPD_ACQUIRE,
            FPD_ERROR
        } status;
        modal::scan_t* scan;
    };

    bool init(fpd_t* fpd, int width, int height);
    void drop(fpd_t* fpd);

    bool connect(fpd_t* fpd);
}

#endif // !_CONSOLE_FPD_H_

#ifdef CONTROL_FPD_IMPLEMENTATION
#ifndef CONTROL_FPD_IMPLEMENTED
#define CONTROL_FPD_IMPLEMENTED

namespace control::fpd {
    // Implementations
    bool init(fpd_t* fpd, int width, int height)
    {
        if (!fpd) {
            return false;
        }

        fpd->status = fpd_t::status_e::FPD_UNCONNECTED;
        fpd->scan = cl::build_raw<modal::scan_t>(width, height);
        assert(fpd->scan);

        return true;
    }

    void drop(fpd_t* fpd)
    {
        if (fpd) {
            // drop(fpd->scan);
        }
        free(fpd);
    }

    bool connect(fpd_t* fpd)
    {
        using status_e = fpd_t::status_e;
        assert(fpd);

        fpd->status = status_e::FPD_CONNECTING;
        if (!control::fpd::fp_init()) {
            SPDLOG_ERROR("Failed to initialize FPD!");
            fpd->status = status_e::FPD_ERROR;
            return false;
        }
        else {
            control::fpd::fp_set_callback_image_recieved([=](int width, int height, int byte_per_pixel, void* data)
            {
                assert(fpd);
                if (fpd->status != status_e::FPD_ACQUIRE) {
                    // do nothing if HVG is not in exposure state
                    return;
                }

                if (fpd->scan == NULL) {
                    SPDLOG_ERROR("CBCT scan is not created during exposure.");
                    return;
                }

                modal::scan_t& scan = *fpd->scan;
                int& index = scan.index;
                const int w = scan.width;
                const int h = scan.height;

                if (w != width || h != height) {
                    SPDLOG_ERROR("Binning must be {} x {}. Other binnings are not supported.", w, h);
                    return;
                }

                const size_t size = sizeof(modal::scan_t::pixel_t) * width * height;
                const ptrdiff_t shift = size * ((size_t)index + 1);

                SPDLOG_TRACE("Image recieved: Width - {:d} Height - {:d} BPP - {:d}\n", width, height, byte_per_pixel);
                SPDLOG_TRACE("Start copying image {:d} ...", index + 1);
                memcpy(scan.images + shift, data, size);
                SPDLOG_TRACE("Complete copying image {:d} ...", index + 1);

                index++;
                scan.angles[index] = index;
            });
            control::fpd::fp_start_acquire();
            fpd->status = status_e::FPD_READY;
            return true;
        }
    }
}

#endif // !CONTROL_FPD_IMPLEMENTED
#endif // CONTROL_FPD_IMPLEMENTATION
