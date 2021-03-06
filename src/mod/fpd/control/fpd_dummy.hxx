#ifndef _FPD_DUMMY_CONTROL_INCLUDE_H_
#define _FPD_DUMMY_CONTROL_INCLUDE_H_

#include <chrono>
#include <filesystem>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "fpd.hxx"

namespace mod::fpd::control {
    struct fpd_dummy_t {
        fpd_t* fpd;
        cl::timer_t* timer;
        std::function<void(void)> callback;
    };

    using status_e = fpd_t::status_e;

    bool init(fpd_dummy_t* dummy, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res);
    void drop(fpd_dummy_t* dummy);

    bool connect(fpd_dummy_t* dummy);
    bool disconnect(fpd_dummy_t* dummy);

    fpd_t::status_e get_status(fpd_dummy_t* fpd);
    void set_status(fpd_dummy_t* fpd, status_e status);

    modal::scan_t* get_scan(fpd_dummy_t* fpd);

}
#endif //!_FPD_DUMMY_CONTROL_INCLUDE_H_

#ifdef FPD_DUMMY_CONTROL_IMPLEMENTATION
#ifndef FPD_DUMMY_CONTROL_IMPLEMENTED
#define FPD_DUMMY_CONTROL_IMPLEMENTED

//#include <omp.h>
#include <fstream>

#include "mod/hnd/hnd.hxx"
#include "control/config.hxx"

namespace mod::fpd::control {

    static void read_raw_data(fpd_dummy_t* dummy)
    {
        assert(dummy);

        const auto width = dummy->fpd->scan->width;
        const auto height = dummy->fpd->scan->height;

        using namespace std::chrono;
        auto t0 = steady_clock::now();

        std::filesystem::path raw_data_path;
        auto config = ::control::get_config();
        if (config) {
            raw_data_path = std::filesystem::path(config->raw_data_folder) / config->raw_data_basename;
        } else {
            // not be able to read config.toml
            // use data_path.txt instead
            const char* data_path = "data_path.txt";
            std::fstream s(data_path, std::fstream::in);
            s >> raw_data_path;
        }
        
        constexpr cl::usize n_images = 360;
        for (int i = 1; i <= n_images; ++i) {
            char file_name[1024];
            sprintf(file_name, "%s.%03d", raw_data_path.string().c_str(), i);
            FILE* fp = fopen(file_name, "rb");
            cl::usize n_read = fread(get_data_at(dummy->fpd->scan, i - 1), sizeof(modal::scan_t::pixel_t), width * height, fp);
            if (n_read != width * height)
                abort();
            fclose(fp);
        }
        auto t1 = steady_clock::now();
        SPDLOG_TRACE("Dummy FPD: read in 360 images in {:d} ms", duration_cast<milliseconds>(t1 - t0).count());
    }

    bool init(fpd_dummy_t* dummy, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res)
    {
        assert(dummy);

        using namespace std::chrono;
        auto t0 = steady_clock::now();
        dummy->fpd = cl::build_raw<fpd_t>(width, height, x_res, y_res);
        auto t1 = steady_clock::now();
        SPDLOG_TRACE("Dummy FPD: memory allocation in {:d} ms", duration_cast<milliseconds>(t1 - t0).count());

        constexpr cl::usize n_images = 360;
        cl::f64 angles[n_images];
        for (int i = 1; i <= n_images; ++i) {
            angles[i - 1] = static_cast<cl::f64>(i);
        }

        read_raw_data(dummy);

        // initialize the callback function
        new(&dummy->callback)std::function<void(void)>;
        dummy->callback = [=]() {
            //modal::scan_t& scan = *dummy->fpd->scan;
            auto scan = mod::fpd::control::get_scan(dummy);
            if (scan->index < 359) {
                scan->index++;
                using namespace std::chrono;
                namespace fs = std::filesystem;
                auto t0 = steady_clock::now();

                const auto w = scan->width;
                const auto h = scan->height;
                auto image = cl::build_raw<sil::image_t<cl::u16>>(w, h, 1, modal::get_data_at(scan, scan->index));

                auto config = ::control::get_config();
                fs::path output_path(config->output_folder);
                char tmp[1024];
                sprintf(tmp, "%03d.hnd", static_cast<cl::i32>(scan->index));
                auto hnd_file_name = output_path / tmp;
                auto hnd = cl::build_raw<mod::hnd::modal::hnd_t>(image, 0.417, 0.417, 1.0);
                int n = hnd::control::write_to_file(hnd, hnd_file_name.string().c_str());

                cl::dealloc(image);  // dealloc image without releasing the internal data

                auto t1 = steady_clock::now();
                //SPDLOG_INFO("Dummy FPD: process image in {:d} ms", duration_cast<milliseconds>(t1 - t0).count());
            }
        };

        return true;
    }

    bool connect(fpd_dummy_t* dummy)
    {
        using status_e = fpd_t::status_e;
        assert(dummy);

        dummy->fpd->status = status_e::FPD_CONNECTING;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        dummy->fpd->status = status_e::FPD_READY;
        return true;
    }

    bool disconnect(fpd_dummy_t* dummy)
    {
        return true;
    }

    void drop(fpd_dummy_t* dummy)
    {
        if (dummy) {
            // timer need to be destroyed first
            cl::clear_interval(dummy->timer);
            dummy->timer = nullptr;

            cl::recycle(dummy->fpd);
            dummy->callback.~function<void(void)>();
        }
    }

    fpd_t::status_e get_status(fpd_dummy_t* dummy)
    {
        assert(dummy);
        assert(dummy->fpd);
        return dummy->fpd->status;
    }

    void set_status(fpd_dummy_t* dummy, status_e status)
    {
        assert(dummy);
        assert(dummy->fpd);
        dummy->fpd->status = status;
    }

    modal::scan_t* get_scan(fpd_dummy_t* dummy)
    {
        assert(dummy && dummy->fpd);
        return dummy->fpd->scan;
    }
}

#endif // !FPD_DUMMY_CONTROL_IMPLEMENTATION
#endif //FPD_DUMMY_CONTROL_IMPLEMENTATION
