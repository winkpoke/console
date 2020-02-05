#ifndef CONSOLE_INCLUDE_CONTROL_H
#define CONSOLE_INCLUDE_CONTROL_H

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"


#include "hvg.h"
#include "flat_panel.h"

#include "modal/modal.h"

namespace control {
    using namespace spdlog;

    // FPD
    void callback_image_recieved(int width, int height, int byte_per_pixel, void* data);
    void connect_to_fpd();
    void drop_fpd();
    
    // HVG

    bool hand_shake(int retry = 2);

    bool set_hvg_exposure_parameters(float kv, float mAs, char focus, float fate, float fps, int retry = 2);

    int get_hvg_status(int retry = 2);

    void connect_to_hvg();
    void drop_hvg();

    void setup_patient();

    int exposure_callback(const char* msg, int len);

    void exposure();

    // status check
    bool is_ready_setup_patient();
    bool is_exposure_ready();

    // connect to upstream server via websocket
    void connect_to_upstream_server();

    // functions
    bool init();
    void drop();
}
#endif   // CONSOLE_INCLUDE_CONTROL_H

#ifdef CONSOLE_CONTROL_IMPLEMENTATION
#ifndef CONSOLE_CONTROL_IMPLEMENTED
#define CONSOLE_CONTROL_IMPLEMENTED

namespace control {
    // FPD
    void callback_image_recieved(int width, int height, int byte_per_pixel, void* data)
    {
        if (modal::g_app_stat.hvg != HVG_EXPOSURE) {
            // do nothing if HVG is not in exposure state
            return;
        }

        if (modal::g_app_stat.scan == NULL) {
            SPDLOG_ERROR("CBCT scan is not created during exposure.");
            return;
        }

        modal::scan_t& scan = *modal::g_app_stat.scan;
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

        //scan.images = (modal::scan::scan_t::pixel_t*)data;
    }

    void connect_to_fpd()
    {
        fpd_status_t& status = modal::g_app_stat.fpd;

        status = FPD_CONNECTING;
        if (!fpd::fp_init()) {
            info("Failed to initialize FPD!");
            status = FPD_ERROR;
        }
        else {
            fpd::fp_set_callback_image_recieved(callback_image_recieved);
            fpd::fp_start_acquire();
            status = FPD_READY;
        }
    }

    // HVG

    bool hand_shake(int retry)
    {
        SPDLOG_INFO("Hand shake with HVG.");

        hvg::context_t*& context = modal::g_app_stat.hvg_context;

        char msg[1024] = { 0 };
        int count = 0;
        while (retry-- >= 0) {
            SPDLOG_TRACE("try - {:d}", count++);
            int condition = 0;
            int n = hvg::send(context, "<IFV", msg, &condition, 1000);
            if (condition != 0) {
                SPDLOG_DEBUG("condition changed: {:.8X}", condition);
            }
            if (n <= 0 || strncmp(msg, ">IFV 1", n) != 0) {
                SPDLOG_TRACE("err: {:d} - {}: {}", n, msg, hvg::last_error_str<hvg::error_t>());
                SPDLOG_TRACE("len = {:d}; pos = {:d}, buf = {}", context->len, context->pos, context->buf);
                continue;
            }
            SPDLOG_TRACE("ok: {:d} - {}", n, msg);
            return true;
        }
        return false;
    }

    bool set_hvg_exposure_parameters(float kv, float mAs, char focus, float fate, float fps, int retry)
    {
        SPDLOG_INFO("Set HVG exposure parameters");
        hvg::context_t*& context = modal::g_app_stat.hvg_context;

        char msg[1024] = { 0 };
        int count = 0;
        float mA = mAs / (fate / fps / 1000);
        //char focus = 'L';
        while (retry-- >= 0) {
            SPDLOG_DEBUG("try - {:d}", count++);
            char param[1024] = { 0 };
            sprintf(param, "0 %.1f %0.2f %.3f 0000 %c %.1f", kv, mAs, mA, focus, fps);
            char cmd[1024];
            char rtv[1024];
            sprintf(cmd, "<ES3 %s", param);
            sprintf(rtv, ">VS3 %s", param);
            //printf("\n%s\n", rtv);
            int condition = 0;
            int n = hvg::send(context, cmd, msg, &condition, 5000);
            if (condition != 0) {
                printf("condition changed: %.8X", condition);
            }
            if (n <= 0 || strncmp(msg, rtv, n) != 0) {
                SPDLOG_DEBUG("error [{:d}] {} :: n {} :: {}", hvg::last_error<hvg::error_t>(), hvg::last_error_str<hvg::error_t>(), n, msg);
                SPDLOG_DEBUG("len = {:d}; pos = {:d}, buf = {}", context->len, context->pos, context->buf);
                if (retry < 0) {
                    break;
                }
                continue;
            }
            debug("ok: {:d} - {}", n, msg);
            return true;
        }
        return false;
    }

    int get_hvg_status(int retry)
    {
        hvg::context_t*& context = modal::g_app_stat.hvg_context;

        char msg[1024] = { 0 };
        while (retry-- >= 0) {
            int condition = 0;
            int n = hvg::send(context, "<GST", msg, &condition, 1000);
            if (n > 0) {
                int cond0 = 0;
                int cond1 = 0;
                int rtv = sscanf(msg, ">GST %x %x", &cond0, &cond1);
                if (rtv != 2) {
                    return -1;
                }
                return (cond0 << 16) | cond1;
            }
        }
        return -1;
    }

    void connect_to_hvg()
    {
        const int port = modal::g_app_stat.serial_port;
        const int baud = modal::g_app_stat.serail_baud;
        hvg_status_t& status = modal::g_app_stat.hvg;
        hvg::context_t*& context = modal::g_app_stat.hvg_context;


        status = HVG_CONNECTING;
        // Init RS232
        context = hvg::open(port, baud, "8N2");
        if (context == NULL) {
            error(hvg::last_error_str<hvg::error_t>());
            status = HVG_ERROR;
        }
        else {
            if (!hand_shake(2)) {
                status = HVG_ERROR;
                return;
            }

            if (!set_hvg_exposure_parameters(100.0f, 0.5f, 'L', 300, 6, 2)) {
                status = HVG_ERROR;
                return;
            }
            status = HVG_READY;
            debug(">GST {:.8X}", get_hvg_status());
            return;
        }
    }

    void setup_patient()
    {

    }

    //callback_return_t

    int exposure_callback(const char* msg, int len)
    {
        SPDLOG_DEBUG("in HVG callback");
        if (strncmp(msg, ">EPA", 4) == 0) {
            SPDLOG_DEBUG(msg);
            return hvg::callback_return_t::CONTINUE;
        }
        else if (strncmp(msg, ">SBY", 4) == 0) {
            SPDLOG_DEBUG("standby ...");
            return hvg::callback_return_t::BREAK;
        }
        else {
            SPDLOG_DEBUG(msg);
            return hvg::callback_return_t::CONTINUE;
        }
    }

    void exposure()
    {
        hvg::context_t*& context = modal::g_app_stat.hvg_context;
        char msg[1024];
        int condition = 0;
        int n = hvg::send(context, "<SXP 0 0 0", NULL, &condition, 5000);
        modal::g_app_stat.hvg = HVG_EXPOSURE;
        n = hvg::send(context, "<SXP 1 0 1", exposure_callback);
    }

    bool is_ready_setup_patient()
    {
        return modal::g_app_stat.hvg == HVG_READY && modal::g_app_stat.fpd == FPD_READY;
    }

    bool is_exposure_ready()
    {
        return modal::g_app_stat.hvg == HVG_READY && modal::g_app_stat.fpd == FPD_READY;
    }

    void drop_hvg()
    {
        hvg::context_t*& context = modal::g_app_stat.hvg_context;
        if (hvg::close(context) == hvg::FAILURE) {
            SPDLOG_ERROR("{}", hvg::last_error_str<hvg::error_t>());
        }
    }

    void drop_fpd()
    {
        fpd::fp_stop_acquire();
        fpd::Deinit();
    }

    bool init()
    {
        return true;
    }

    void drop()
    {
        drop_hvg();
        drop_fpd();
    }

    void connect_to_upstream_server()
    {
        websocket::websocket_t* s = modal::g_app_stat.socket;
        SPDLOG_INFO("Connecting to server: ", s->url);
        if (!websocket::connect(s)) {
            SPDLOG_ERROR("Fail to connect to the server.");
            return;
        }
        SPDLOG_INFO("Successully connected to the server.");
        SPDLOG_INFO("Handshake with server...");
        websocket::on_recv_text(s, [](const char* msg) {
            SPDLOG_INFO("Message recieved: {}", msg);
            }
        );
        websocket::send(s, "<HELLO");
    }
}
#endif // !CONSOLE_CONTROL_IMPLEMENTED

#endif   //CONSOLE_CONTROL_IMPLEMENTATION