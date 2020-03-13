#ifndef _CBCT_CONTROL_INCLUDE_H_
#define _CBCT_CONTROL_INCLUDE_H_

#include "mod/fpd/fpd.h"
#include "mod/hvg/hvg.h"

#include "control/websocket.hxx"

namespace mod::cbct::control {

    enum class cbct_mode_t {
        HEAD,
        LUNG,
        ABDOMINAL,
        CUSTOM
    };

    enum class resolution_t {
        _128X128,
        _256X256,
        _384X384,
        _512X512,
        _768X768
    };

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
        return true;
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
            SPDLOG_ERROR("Fail to connect to the server.");
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
            std::this_thread::sleep_for(std::chrono::seconds(60));
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
}

#endif //!_CBCT_CONTROL_INCLUDE_H_

#ifdef CBCT_CONTROL_IMPLEMENTATION
#ifndef CBCT_CONTROL_IMPLEMENTED
#define CBCT_CONTROL_IMPLEMENTED

namespace mod::cbct::control {


}

#endif // !CBCT_CONTROL_IMPLEMENTATION
#endif //CBCT_CONTROL_IMPLEMENTATION
