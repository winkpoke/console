#ifndef _HVG_SERIAL_H_
#define _HVG_SERIAL_H_

#include <future>
#include <functional>

namespace mod::hvg::control {
    constexpr size_t BUF_SIZE = 1024 * 4;
    struct hvg_serial_t {
        int port;
        int baud;
        char mode[4];
        int pos;
        int len;
        char buf[BUF_SIZE];
        enum status_t {
            PENDING,
            SENDING,
            SENT,
            RECIEVING,
            RECIEVED,      // not used
            TIMEOUT
        } status;

        using process_data_f = std::function<void(char* p, int n)>;
        process_data_f process_data;
    };

    bool init(hvg_serial_t* p, int port, int baud, const char* mode);
    void drop(hvg_serial_t* p);

    bool open(hvg_serial_t* p);
    bool close(hvg_serial_t* p);
    int send(hvg_serial_t* p, char* buf, int len);
    int recv(hvg_serial_t* p, char* buf, int len);
}

#endif // _HVG_SERIAL_H_

#ifdef HVG_SERIAL_IMPLEMENTATION
#ifndef _HVG_SERIAL_IMPLEMENTED_
#define _HVG_SERIAL_IMPLEMENTED_

namespace mod::hvg::control {
    bool init(hvg_serial_t* p, int port, int baud, const char* mode)
    {
        if (p == nullptr) {
            return false;
        }

        p->port = port;
        p->baud = baud;
        strncpy(context->mode, mode, 4);
        memset(p->buf, 0, BUF_SIZE);

        return true;
    }
}

#endif // _HVG_SERIAL_IMPLEMENTED_
#endif // HVG_SERIAL_IMPLEMENTATION