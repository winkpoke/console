#ifndef _CONTROL_FPD_H_
#define _CONTROL_FPD_H_

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "cl.h"
#include "sil.h"

#include "iray.hxx"

#include "modal/scan.hxx"

namespace mod::fpd::control {
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
    
    /*std::map<status_e, const char*> status = { {status_e::FPD_UNCONNECTED, "unconnected"} };*/

    bool init(fpd_t* fpd, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res);
    void drop(fpd_t* fpd);

    fpd_t::status_e get_status(fpd_t* fpd);
    void set_status(fpd_t* fpd, status_e status);
    std::string to_string(fpd_t::status_e status);

    modal::scan_t* get_scan(fpd_t* fpd);

    bool connect(fpd_t* fpd);

    bool disconnect(fpd_t* fpd);
}

#endif // !_CONTROL_FPD_H_

#ifdef CONTROL_FPD_IMPLEMENTATION
#ifndef CONTROL_FPD_IMPLEMENTED
#define CONTROL_FPD_IMPLEMENTED

#include <filesystem>

#include "control/config.hxx"
#include "mod/hnd/hnd.hxx"

namespace mod::fpd::control {
    // Implementations
    using status_e = fpd_t::status_e;
    const char* fpd_t::status_strs[] = { "unconnected", "connecting", "ready", "acquiring", "error" };

    bool init(fpd_t* fpd, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res)
    {
        if (!fpd) {
            return false;
        }

        fpd->status = fpd_t::status_e::FPD_UNCONNECTED;   
        constexpr cl::usize n_images = 360;
        fpd->scan = cl::build_raw<modal::scan_t>(width, height, x_res, y_res, n_images);
        assert(fpd->scan);

        return true;
    }

    void drop(fpd_t* fpd)
    {
        if (fpd) {
            cl::recycle(fpd->scan);
        }
        disconnect(fpd);
    }
    
    fpd_t::status_e get_status(fpd_t* fpd)
    {
        assert(fpd);
        return fpd->status;
    }
    void set_status(fpd_t* fpd, status_e status)
    {
        assert(fpd);
        fpd->status = status;
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
        if (!mod::fpd::control::fp_init()) {
            SPDLOG_ERROR("Failed to initialize FPD!");
            fpd->status = status_e::FPD_ERROR;
            return false;
        }
        mod::fpd::control::fp_set_callback_image_recieved([=](int width, int height, int byte_per_pixel, void* data)
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
            auto& index = scan.index;
            const auto w = scan.width;
            const auto h = scan.height;

            if (w != width || h != height) {
                SPDLOG_ERROR("Binning must be {} x {}. Other binnings are not supported.", w, h);
                return;
            }

            //const size_t size = sizeof(modal::scan_t::pixel_t) * width * height;
            //const ptrdiff_t shift = size * ((size_t)index + 1);

            using namespace std::chrono;
            auto t0 = steady_clock::now();
            SPDLOG_TRACE("Image recieved: Width - {:d} Height - {:d} BPP - {:d}\n", width, height, byte_per_pixel);
            const auto idx = index + 1;
            SPDLOG_TRACE("Start copying image {:d} ...", idx);
            // memcpy(scan.images + shift, data, size);
            push_data(&scan, static_cast<modal::scan_t::pixel_t*>(data), idx);
            auto t1 = steady_clock::now();
            SPDLOG_TRACE("Complete copying image {:d} in {:d} ms...", idx, duration_cast<milliseconds>(t1 - t0).count());

            using namespace std::chrono;
            namespace fs = std::filesystem;
            auto t2 = steady_clock::now();

            auto image = cl::build_raw<sil::image_t<cl::u16>>(w, h, 1, modal::get_data_at(&scan, scan.index));

            auto config = ::control::get_config();
            fs::path output_path(config->output_folder);
            char tmp[1024];
            sprintf(tmp, "%03d.hnd", static_cast<cl::i32>(scan.index));
            auto hnd_file_name = output_path / tmp;
            auto hnd = cl::build_raw<mod::hnd::modal::hnd_t>(image, 0.417, 0.417, 1.0);
            int n = hnd::control::write_to_file(hnd, hnd_file_name.string().c_str());

            cl::dealloc(image);  // dealloc image without releasing the internal data

            auto t3 = steady_clock::now();
            SPDLOG_INFO("FPD: process image in {:d} ms", duration_cast<milliseconds>(t3 - t2).count());

            //index++;
            if (idx >= 359) {
                // TODO acquiring completed
                fpd->status = status_e::FPD_READY;
            }
            //scan.angles[index] = index;
        });
        mod::fpd::control::fp_start_acquire();
        fpd->status = status_e::FPD_READY;
        return true;
    }

    bool disconnect(fpd_t* fpd)
    {
        if (fpd && (fpd->status != status_e::FPD_UNCONNECTED || fpd->status != status_e::FPD_CONNECTING)) {
            mod::fpd::control::fp_stop_acquire();
            mod::fpd::control::Deinit();
            return true;
        }
        return false;
    }

    modal::scan_t* get_scan(fpd_t* fpd)
    {
        assert(fpd);
        return fpd->scan;
    }
}

#endif // !CONTROL_FPD_IMPLEMENTED
#endif // CONTROL_FPD_IMPLEMENTATION
