#ifndef _CONTROL_HVG_H_
#define _CONTROL_HVG_H_

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "siemens.hxx"

namespace control::hvg {
    struct hvg_t {
        enum class status_e {
            HVG_UNCONNECTED,
            HVG_CONNECTING,
            HVG_READY,
            HVG_EXPOSURE,
            HVG_ERROR
        };
        const static char* status_strs[];

        status_e status;
        float kv;
        float mAs;
        context_t* context;
    };

    bool init(hvg_t* hvg, float kv, float mAs, std::unique_ptr<context_t> context);

    void drop(hvg_t* hvg);

    bool hand_shake(hvg_t* hvg, int retry = 2);

    bool set_exposure_parm(hvg_t* hvg, float kv, float mAs, char focus, float fate, float fps, int retry = 2);

    int query_status(hvg_t* hvg, int retry = 2);

    bool connect(hvg_t* hvg, int port, int baud, const char* mode);

    int exposure_callback(const char* msg, int len);

    void exposure(hvg_t* hvg);
}

#endif // !_CONTROL_HVG_H_

#ifdef CONTROL_HVG_IMPLEMENTATION
#ifndef CONTROL_HVG_IMPLEMENTED
#define CONTROL_HVG_IMPLEMENTED

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"


namespace control::hvg {
    
    const char* status_strs[] = { "unconnected", "connecting", "ready", "exposure", "error" };

    using status_e = hvg_t::status_e;

    bool init(hvg_t* hvg, float kv, float mAs, std::unique_ptr<context_t> context)
    {
        if (!hvg || !context) {
            return false;
        }
        hvg->status = status_e::HVG_UNCONNECTED;
        hvg->kv = kv;
        hvg->mAs = mAs;
        hvg->context = context.release();
        return true;
    }

    void drop(hvg_t* hvg)
    {
        if (hvg) {
            if (hvg::close(hvg->context) == FAILURE) {
                SPDLOG_ERROR("{}", last_error_str<error_t>());
            }
            close(hvg->context);
            hvg->~hvg_t();
            free(hvg);
        }
    }

    bool hand_shake(hvg_t* hvg, int retry)
    {
        SPDLOG_INFO("Hand shake with HVG.");

        if (!hvg || !hvg->context) {
            return false;
        }

        char msg[1024] = { 0 };
        int count = 0;
        while (retry-- >= 0) {
            SPDLOG_TRACE("try - {:d}", count++);
            int condition = 0;
            int n = send(hvg->context, "<IFV", msg, &condition, 1000);
            if (condition != 0) {
                SPDLOG_DEBUG("condition changed: {:.8X}", condition);
            }
            if (n <= 0 || strncmp(msg, ">IFV 1", n) != 0) {
                SPDLOG_TRACE("err: {:d} - {}: {}", n, msg, last_error_str<hvg::error_t>());
                SPDLOG_TRACE("len = {:d}; pos = {:d}, buf = {}", hvg->context->len, hvg->context->pos, hvg->context->buf);
                continue;
            }
            SPDLOG_TRACE("ok: {:d} - {}", n, msg);
            return true;
        }
        return false;
    }

    bool set_exposure_parm(hvg_t* hvg, float kv, float mAs, char focus, float fate, float fps, int retry)
    {
        SPDLOG_INFO("Set HVG exposure parameters");

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
            int n = send(hvg->context, cmd, msg, &condition, 5000);
            if (condition != 0) {
                printf("condition changed: %.8X", condition);
            }
            if (n <= 0 || strncmp(msg, rtv, n) != 0) {
                SPDLOG_DEBUG("error [{:d}] {} :: n {} :: {}", last_error<error_t>(), last_error_str<error_t>(), n, msg);
                SPDLOG_DEBUG("len = {:d}; pos = {:d}, buf = {}", hvg->context->len, hvg->context->pos, hvg->context->buf);
                if (retry < 0) {
                    break;
                }
                continue;
            }
            SPDLOG_DEBUG("ok: {:d} - {}", n, msg);
            return true;
        }
        return false;
    }

    int query_status(hvg_t* hvg, int retry)
    {
        char msg[1024] = { 0 };
        while (retry-- >= 0) {
            int condition = 0;
            int n = send(hvg->context, "<GST", msg, &condition, 1000);
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

    bool connect(hvg_t* hvg, int port, int baud, const char* mode)
    {
        hvg->status = status_e::HVG_CONNECTING;
        // Init RS232
        hvg->context = open(port, baud, mode);
        if (hvg->context == NULL) {
            SPDLOG_ERROR(last_error_str<error_t>());
            hvg->status = status_e::HVG_ERROR;
            return false;
        }
        else {
            if (!hand_shake(hvg, 2)) {
                hvg->status = status_e::HVG_ERROR;
                return false;
            }

            if (!set_exposure_parm(hvg, 100.0f, 0.5f, 'L', 300, 6, 2)) {
                hvg->status = status_e::HVG_ERROR;
                return false;
            }
            hvg->status = status_e::HVG_READY;
            SPDLOG_DEBUG(">GST {:.8X}", query_status(hvg));
            return true;
        }
    }

    int exposure_callback(const char* msg, int len)
    {
        SPDLOG_DEBUG("in HVG callback");
        if (strncmp(msg, ">EPA", 4) == 0) {
            SPDLOG_DEBUG(msg);
            return callback_return_t::CONTINUE;
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

    void exposure(hvg_t* hvg)
    {
        char msg[1024];
        int condition = 0;
        int n = hvg::send(hvg->context, "<SXP 0 0 0", NULL, &condition, 5000);
        hvg->status = status_e::HVG_EXPOSURE;
        n = hvg::send(hvg->context, "<SXP 1 0 1", exposure_callback);
    }
}

#endif // CONTROL_HVG_IMPLEMENTED
#endif // !CONTROL_HVG_IMPLEMENTATION