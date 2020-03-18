#ifndef _CBCT_CONTROL_INCLUDE_H_
#define _CBCT_CONTROL_INCLUDE_H_

#include <sstream>
#include <string>

#define NOMINMAX
#include <windows.h>

#include "mod/fpd/fpd.h"
#include "mod/hvg/hvg.h"
#include "control/config.hxx"

#include "control/websocket.hxx"

namespace mod::cbct::control {

    struct geometry_t {
        cl::f64 sid;
        cl::f64 sdd;
        cl::i32 n_projections;
        cl::f64 first_angle;
        cl::f64 arc;
        cl::f64 proj_offset_x;
        cl::f64 proj_offset_y;
        cl::f64 out_of_plane_angle;
        cl::f64 in_plane_angle;
        cl::f64 source_offset_x;
        cl::f64 source_offset_y;
    };

    bool init(geometry_t* p);
    static void drop(geometry_t* p) {}


    struct reconstruction_parameter_t {
        static const size_t STR_LEN = 1024;
        char hnd_path[STR_LEN];
        char output_path[STR_LEN];
        char output_file[STR_LEN];
        cl::f64 spacing[3];
        int dimension[3];
        bool use_gpu;
    };
    std::string to_string(const reconstruction_parameter_t* p);

    enum class cbct_mode_t {
        HEAD,
        LUNG,
        ABDOMINAL,
        CUSTOM
    };

    enum class resolution_t {
        //_128X128,
        //_256X256,
        //_384X384,
        _512X512,
        _768X768,
        _1024x1024
    };
    static const char* cbct_mode_list[] = { "Head", "Lung", "Abdominal", "Custom" };
    static const char* resolution_list[] = { /*"128x128", "256x256", "384x384", */"512x512", "768x768", "1024x1024" };

    template <class F = fpd::control::fpd_t, 
              class H = hvg::control::hvg_t, 
              class W = websocket::websocket_t>
    struct cbct_impl_t {
        cl::shared_ptr<F>       fpd;
        cl::shared_ptr<H>       hvg;
        cl::shared_ptr<W>       socket;
        cbct_mode_t             mode;
        resolution_t            resolution;
        cl::f32                 slice_dist;
        geometry_t              geo;
    };

    using cbct_t = cbct_impl_t<>;
    using cbct_dummy_t = cbct_impl_t<fpd::control::fpd_dummy_t>;

    template<class F, class H, class W>
    bool init(cbct_impl_t<F, H, W>* p, cl::shared_ptr<F> fpd, cl::shared_ptr<H> hvg)
    {
        assert(p);
        new(&p->fpd)shared_ptr<F>(fpd);
        new(&p->hvg)shared_ptr<H>(hvg);
        new(&p->socket)shared_ptr<W>(cl::build_shared<W>("ws://172.17.214.17:3000/ws"));

        p->mode = cbct_mode_t::CUSTOM;
        p->resolution = resolution_t::_512X512;
        p->slice_dist = 2.5f;
        return init(&p->geo);
        // return true;
    }

    template <class F, class H, class W>
    void drop(cbct_impl_t<F, H, W>* p)
    {
        if (p) {
            p->fpd.~shared_ptr();
            p->hvg.~shared_ptr();
            p->socket.~shared_ptr();
        }
    }

    template <class F, class H, class W>
    void set_mode(cbct_impl_t<F, H, W>*p, cbct_mode_t mod)
    {
        assert(p);
        p->mode = mod;
    }

    template <class F, class H, class W>
    void set_resolution(cbct_impl_t<F, H, W>* p, resolution_t resolution)
    {
        assert(p);
        p->resolution = resolution;
    }

    template <class F, class H, class W>
    void connect_to_fpd(cbct_impl_t<F, H, W>* p)
    {
        assert(p);
        assert(p->fpd);

        mod::fpd::control::connect(p->fpd.get());
    }

    template <class F, class H, class W>
    void connect_to_hvg(cbct_impl_t<F, H, W>* p)
    {
        assert(p);
        assert(p->fpd);

        const int port = 3;
        const int baud = 19200;
        const char* mode = "8N2";

        mod::hvg::control::connect(p->hvg.get(), port, baud, mode);
    }

    template <class F, class H, class W>
    void connect_to_upstream_server(cbct_impl_t<F, H, W>* p)
    {
        assert(p);
        assert(p->socket);
        SPDLOG_INFO("Connecting to server: ", p->socket->url);
        if (!websocket::connect(p->socket.get())) {
            SPDLOG_WARN("Fail to connect to the server.");
            return;
        }
        SPDLOG_INFO("Successully connected to the server.");
        SPDLOG_INFO("Handshake with server...");
        websocket::on_recv_text(p->socket.get(), [](const char* msg) {
            SPDLOG_INFO("Message recieved: {}", msg);
            }
        );
        websocket::send(p->socket.get(), "<HELLO");
    }

    template <class H, class W>
    void exposure(cbct_impl_t<mod::fpd::control::fpd_dummy_t, H, W>* p)
    {
        assert(p);
        assert(p->fpd);
        assert(p->hvg);

        mod::fpd::control::set_status(p->fpd.get(), mod::fpd::control::status_e::FPD_ACQUIRE);
        p->fpd->timer = cl::set_interval(p->fpd->callback, 166);
        // the exposure takes 60s and stop the timer then
        auto stopper = [](cl::timer_t** t, cl::shared_ptr<mod::fpd::control::fpd_dummy_t> dummy) {
            std::this_thread::sleep_for(std::chrono::seconds(70));
            cl::clear_timeout(*t);
            *t = nullptr;
            assert(dummy);
            dummy->fpd->status = mod::fpd::control::status_e::FPD_READY;
            rewind(dummy->fpd->scan);
        };

        std::thread t(stopper, &p->fpd->timer, p->fpd);
        t.detach();
    }

    template <class F, class H, class W>
    bool is_exposure_ready(cbct_impl_t<F, H, W>* p)
    {
        assert(p);
        assert(p->fpd && p->hvg);

        return fpd::control::get_status(p->fpd.get()) == fpd::control::status_e::FPD_READY;
    }


    static auto stov(const char* src, char* out[])
    {
        std::string myText(src);
        std::istringstream iss(myText);
        std::string token;
        int i = 0;
        while (std::getline(iss, token, ' '))
        {
            if (token != "") {
                out[i] = (char*)malloc(sizeof(char) * token.length());
                if (out[i] == nullptr) {
                    return 0;
                }
                strcpy(out[i], token.c_str());
                ++i;
            }
        }
        return i;
    }

    template <class F, class H, class W>
    void reconstruct(cbct_impl_t<F, H, W>* p)
    {
        SPDLOG_INFO("Start reconstruction ...");
        SPDLOG_INFO("Geometry:");
        SPDLOG_INFO("\t{:s}: {:f}", "sid", p->geo.sid);
        SPDLOG_INFO("\t{:s}: {:f}", "sdd", p->geo.sdd);
        SPDLOG_INFO("\t{:s}: {:d}", "n_projections", p->geo.n_projections);
        SPDLOG_INFO("\t{:s}: {:f}", "first_angle", p->geo.first_angle);
        SPDLOG_INFO("\t{:s}: {:f}", "arc", p->geo.arc);
        SPDLOG_INFO("\t{:s}: {:f}", "proj_offset_x", p->geo.proj_offset_x);
        SPDLOG_INFO("\t{:s}: {:f}", "proj_offset_y", p->geo.proj_offset_y);
        SPDLOG_INFO("\t{:s}: {:f}", "source_offset_x", p->geo.source_offset_x);
        SPDLOG_INFO("\t{:s}: {:f}", "source_offset_y", p->geo.source_offset_y);
        SPDLOG_INFO("\t{:s}: {:f}", "out_of_plane_angle", p->geo.out_of_plane_angle);
        SPDLOG_INFO("\t{:s}: {:f}", "in_plane_angle", p->geo.in_plane_angle);

        namespace fs = std::filesystem;
        auto config = ::control::get_config();
        fs::path hnd_path(config->output_folder);


        const char* dll_cuda_path = R"(libs\x64\rtkfdk_cuda.dll)";
        HINSTANCE h = ::LoadLibrary(dll_cuda_path);

        

        cl::f64 spacing = .0;
        cl::u32 dim = 0;
        switch (p->resolution) {
        case resolution_t::_512X512:
            spacing = 0.5;
            dim = 512;
            break;
        case resolution_t::_768X768:
            spacing = 0.3333;
            dim = 768;
            break;
        case resolution_t::_1024x1024:
            spacing = 0.25;
            dim = 1024;
            break;
        default:
            abort();
        }

        const cl::f64 spacing_y = p->slice_dist;
        const cl::i32 dim_y = static_cast<cl::i32>(300.0 / p->slice_dist);
        reconstruction_parameter_t para {
            "",           //char hnd_path[STR_LEN];
            "",           //char output_path[STR_LEN];
            R"(ct.mha)",                         //char output_file[STR_LEN];
            {spacing, spacing_y, spacing},       //cl::f64 spacing[3];
            {dim, dim_y, dim},                   //int dimension[3];
            true                                 //bool use_gpu;
        };

        auto out = hnd_path.string();
        strncpy(para.hnd_path, out.c_str(), out.length());
        strncpy(para.output_path, out.c_str(), out.length());
        auto args_cbct_cuda = to_string(&para);

        typedef int (*rtkfdk_f)(int argc, char** argv, const geometry_t geo);
        rtkfdk_f rtkfdk = (rtkfdk_f)::GetProcAddress(h, "rtkfdk");
        if (rtkfdk == nullptr) {
            SPDLOG_ERROR("Error: cannot get the function address.");
            return;
        }

        char* argv[256];
        int n = stov(args_cbct_cuda.c_str(), argv);

        int result = rtkfdk(n, argv, p->geo);
        if (result != EXIT_SUCCESS) {
            SPDLOG_DEBUG("Reconstruction failed.");
            ::FreeLibrary(h);
            return;
        }
        ::FreeLibrary(h);

        SPDLOG_INFO("Reconstruction accomplished ...");
    }
}

#endif //!_CBCT_CONTROL_INCLUDE_H_

#ifdef CBCT_CONTROL_IMPLEMENTATION
#ifndef CBCT_CONTROL_IMPLEMENTED
#define CBCT_CONTROL_IMPLEMENTED

namespace mod::cbct::control {

    bool init(geometry_t* p) {
        assert(p);

        p->sid = 849.0;
        p->sdd = 1614.0;
        p->n_projections = 360;
        p->first_angle = 55.0;
        p->arc = 360.0;
        p->proj_offset_x = .0;
        p->proj_offset_y = .0;
        p->out_of_plane_angle = .0;
        p->in_plane_angle = .0;
        p->source_offset_x = .0;
        p->source_offset_y = .0;
        return true;
    }

    std::string to_string(const reconstruction_parameter_t* p)
    {
        assert(p);
        namespace fs = std::filesystem;
        fs::path output(p->output_path);
        output /= p->output_file;

        std::stringstream spacing;
        spacing << p->spacing[0] << "," << p->spacing[1] << "," << p->spacing[2];

        std::stringstream dim;
        dim << p->dimension[0] << "," << p->dimension[1] << "," << p->dimension[2];

        std::string s(R"(reconstruction.exe -v -g geometry.xml --regexp .*.hnd)");
        s.append(" --path ")
            .append(p->hnd_path)
            .append(" --output ")
            .append(output.string().c_str())
            .append(" --spacing ")
            .append(spacing.str())
            .append(" --dimension ")
            .append(dim.str());

        if (p->use_gpu) {
            s.append(" --hardware cuda ");
        }

        return move(s);
    }
}

#endif // !CBCT_CONTROL_IMPLEMENTATION
#endif //CBCT_CONTROL_IMPLEMENTATION
