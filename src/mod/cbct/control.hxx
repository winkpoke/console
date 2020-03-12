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

    template <class fpd_t = fpd::control::fpd_t, 
              class hvg_t = hvg::control::hvg_t, 
              class websocket_t = websocket::websocket_t>
    struct cbct_t {
        cl::shared_ptr<fpd_t>       fpd;
        cl::shared_ptr<hvg_t>       hvg;
        cl::shared_ptr<websocket_t> socket;
        cbct_mode_t                 mode;
        resolution_t                resolution;
        cl::f64                     slice_dist;
    };

    template<class F, class H, class W>
    bool init(cbct_t<F, H, W>* p, cl::shared_ptr<F> fpd, cl::shared_ptr<H>* hvg)
    {
        assert(p);
        new(&p->fpd)shared_ptr<F>;
        new(&p->hvg)shared_ptr<H>;
        new(&p->socket)shared_ptr<W>;

        p->mode = cbct_mode_t::CUSTOM;
        p->resolution = resolution_t::_512X512;
        p->slice_dist = 2.5f;

        // websocket
        p->socket = cl::build_shared<W>("ws://172.17.214.17:3000/ws");
        return true;
    }

    template <class F, class H, class W>
    bool drop(cbct_t<F, H, W>* p)
    {
        if (p) {
            p->fpd.~shared_ptr();
            p->hvg.~shared_ptr();
            p->socket.~shared_ptr();
        }
    }

    template <class F, class H, class W>
    void connect_to_fpd(cbct_t<F, H, W>* p)
    {
        assert(p);
        assert(p->fpd);

        mod::fpd::control::connect(p->fpd.get());
    }

    template <class F, class H, class W>
    void connect_to_hvg(cbct_t<F, H, W>* p)
    {
        assert(p);
        assert(p->fpd);

        const int port = 3;
        const int baud = 19200;
        const char* mode = "8N2";

        mod::hvg::control::connect(p->hvg.get(), port, baud, mode);
    }

    template <class F, class H, class W>
    void connect_to_upstream_server(cbct_t<F, H, W>* p)
    {
        assert(p);
        assert(p->socket);
        SPDLOG_INFO("Connecting to server: ", p->socket->url);
        if (!websocket::connect(p->socket)) {
            SPDLOG_ERROR("Fail to connect to the server.");
            return;
        }
        SPDLOG_INFO("Successully connected to the server.");
        SPDLOG_INFO("Handshake with server...");
        websocket::on_recv_text(p->socket, [](const char* msg) {
            SPDLOG_INFO("Message recieved: {}", msg);
            }
        );
        websocket::send(p->socket, "<HELLO");
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