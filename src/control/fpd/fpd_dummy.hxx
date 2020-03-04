#ifndef _FPD_DUMMY_CONTROL_INCLUDE_H_
#define _FPD_DUMMY_CONTROL_INCLUDE_H_

#include <chrono>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "fpd.hxx"

namespace control::fpd {
    struct fpd_dummy_t {
        fpd_t* fpd;
        //cl::u16* image_buf;
    };

    using status_e = fpd_t::status_e;

    bool init(fpd_dummy_t* dummy, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res);
    void drop(fpd_dummy_t* dummy);
}
#endif //!_FPD_DUMMY_CONTROL_INCLUDE_H_

#ifdef FPD_DUMMY_CONTROL_IMPLEMENTATION
#ifndef FPD_DUMMY_CONTROL_IMPLEMENTED
#define FPD_DUMMY_CONTROL_IMPLEMENTED

//#include <omp.h>

namespace control::fpd {
    bool init(fpd_dummy_t* dummy, cl::usize width, cl::usize height, cl::f64 x_res, cl::f64 y_res)
    {
        assert(dummy);

        using namespace std::chrono;

        auto t0 = steady_clock::now();

        dummy->fpd = cl::build_raw<fpd_t>(width, height, x_res, y_res);

        auto t1 = steady_clock::now();
        SPDLOG_TRACE("Dummy FPD: memory allocation in {:d} ms", duration_cast<milliseconds>(t1 - t0).count());

        constexpr cl::usize n_images = 360;
        const char* raw_data_path = R"(C:\Projects\CBCT\data\headneck_1024x1024\raw\headneck_360_1024.raw)";
        auto raw_data = cl::alloc<modal::scan_t::pixel_t>(width * height * n_images);

        auto t2 = steady_clock::now();

        cl::f64 angles[n_images];

        // openMP does not support two phase name lookup and neet to turn it off /Zc:twoPhase -
        //#pragma omp parallel for
        for (int i = 1; i <= n_images; ++i) {
            char file_name[1024];
            sprintf(file_name, "%s.%03d", raw_data_path, i);
            FILE* fp = fopen(file_name, "rb");
            ptrdiff_t shift = width * height * (i - 1);
            cl::usize n_read = fread(raw_data + shift, sizeof(modal::scan_t::pixel_t), width * height, fp);
            if (n_read != width * height)
                abort();
            fclose(fp);
            angles[i - 1] = static_cast<cl::f64>(i);
        }
        auto t3 = steady_clock::now();
        SPDLOG_TRACE("Dummy FPD: read in 360 images in {:d} ms", duration_cast<milliseconds>(t3 - t2).count());
        dummy->fpd->scan = cl::build_raw<modal::scan_t>(width, height, x_res, y_res, n_images, angles, raw_data);

        return true;
    }

    void drop(fpd_dummy_t* dummy)
    {
        if (dummy) {
            cl::recycle(dummy->fpd);
            //cl::dealloc(dummy->image_buf);
        }
    }
}

#endif // !FPD_DUMMY_CONTROL_IMPLEMENTATION
#endif //FPD_DUMMY_CONTROL_IMPLEMENTATION
