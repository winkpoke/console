#ifndef _CONTROL_FPD_H_
#define _CONTROL_FPD_H_

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
        };
        const static char* status_strs[];
        
        status_e status;
        modal::scan_t* scan;
    };
    using status_e = fpd_t::status_e;
    bool init(fpd_t* fpd, int width, int height);
    void drop(fpd_t* fpd);

    std::string to_string(fpd_t::status_e status);

    bool connect(fpd_t* fpd);

    bool disconnect(fpd_t* fpd);
}

#endif // !_CONTROL_FPD_H_

#ifdef CONTROL_FPD_IMPLEMENTATION
#ifndef CONTROL_FPD_IMPLEMENTED
#define CONTROL_FPD_IMPLEMENTED

namespace control::fpd {
    // Implementations
    using status_e = fpd_t::status_e;
    const char* fpd_t::status_strs[] = { "unconnected", "connecting", "ready", "acquiring", "error" };

    bool init(fpd_t* fpd, int width, int height)
    {
        if (!fpd) {
            return false;
        }

        fpd->status = fpd_t::status_e::FPD_UNCONNECTED;
        
        constexpr cl::usize n_images = 360;
        // const char* raw_data_path = R"(C:\Projects\CBCT\data\headneck_1024x1024\headneck_360_1024.raw)";
        const char* raw_data_path = R"(C:\Projects\CBCT\data\headneck_1024x1024\raw\headneck_360_1024.raw)";
        auto raw_data = cl::alloc<modal::scan_t::pixel_t>(width * height * n_images);
       
        
        cl::f64 angles[n_images];
        for (int i = 1; i <= n_images; ++i) {
            char file_name[1024];
            sprintf(file_name, "%s.%03d", raw_data_path, i);
            FILE* fp = fopen(file_name, "rb");
            ptrdiff_t shift = width * height * (i - 1);
            cl::usize n_read = fread(raw_data + shift, sizeof(modal::scan_t::pixel_t), width * height, fp);
            fclose(fp);
            angles[i - 1] = static_cast<cl::f64>(i);
        }

        fpd->scan = cl::build_raw<modal::scan_t>(width, height, 0.471, 0.417, n_images, angles, raw_data);
        // fpd->scan = cl::build_raw<modal::scan_t>(width, height, 0.417, 0.417, 360);
        // assert(fpd->scan);

        return true;
    }

    void drop(fpd_t* fpd)
    {
        if (fpd) {
            drop(fpd->scan);
            fpd->scan = nullptr;
        }
        disconnect(fpd);
        free(fpd);
    }

    std::string to_string(fpd_t::status_e status)
    {
        return fpd_t::status_strs[static_cast<int>(status)];
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
            cl::usize& index = scan.index;
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

    bool disconnect(fpd_t* fpd)
    {
        if (fpd && (fpd->status != status_e::FPD_UNCONNECTED || fpd->status != status_e::FPD_CONNECTING)) {
            fpd::fp_stop_acquire();
            fpd::Deinit();
            return true;
        }
        return false;
    }
}

#endif // !CONTROL_FPD_IMPLEMENTED
#endif // CONTROL_FPD_IMPLEMENTATION
