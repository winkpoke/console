#ifndef _FPD_DUMMY_CONTROL_INCLUDE_H_
#define _FPD_DUMMY_CONTROL_INCLUDE_H_

#include <chrono>

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
}
#endif //!_FPD_DUMMY_CONTROL_INCLUDE_H_

#ifdef FPD_DUMMY_CONTROL_IMPLEMENTATION
#ifndef FPD_DUMMY_CONTROL_IMPLEMENTED
#define FPD_DUMMY_CONTROL_IMPLEMENTED

//#include <omp.h>

namespace mod::fpd::control {

    static void read_raw_data(fpd_dummy_t* dummy)
    {
        assert(dummy);

        const auto width = dummy->fpd->scan->width;
        const auto height = dummy->fpd->scan->height;

        using namespace std::chrono;
        auto t0 = steady_clock::now();

        const char* raw_data_path = R"(C:\Projects\CBCT\data\headneck_1024x1024\raw\headneck_360_1024.raw)";
        constexpr cl::usize n_images = 360;
        for (int i = 1; i <= n_images; ++i) {
            char file_name[1024];
            sprintf(file_name, "%s.%03d", raw_data_path, i);
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
            modal::scan_t& scan = *dummy->fpd->scan;
            if (scan.index < 359) {
                scan.index++;
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
}

#endif // !FPD_DUMMY_CONTROL_IMPLEMENTATION
#endif //FPD_DUMMY_CONTROL_IMPLEMENTATION
