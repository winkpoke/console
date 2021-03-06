#ifndef _HVG_SIEMENS_INCLUDE_H_
#define _HVG_SIEMENS_INCLUDE_H_

#include <time.h>
#include "imgui.h"

#include "rs232.h"

//// logging
//#include "spdlog/spdlog.h"
//#include "spdlog/sinks/stdout_sinks.h"

namespace mod::hvg::control {
    const int SUCCESS = 0;
    const int FAILURE = -1;
    //enum error_t {
    //    ERR_NONE,
    //    ERR_STATUS,
    //    ERR_COM_OPEN,
    //    ERR_COM_CLOSE,
    //    ERR_COM_STATUS,
    //    ERR_COM_CONTEXT_NULL,
    //    ERR_CMD_STATUS_PARSE,
    //    ERR_CMD_TIMEOUT,
    //};
    //static error_t g_error = ERR_NONE;
    //const char* error_description[] = {
    //    "no error",                       // ERR_NONE
    //    "HVG status is not ready",        // ERR_STATUS
    //    "cannot open COM port",           // ERR_COM_OPEN
    //    "cannot close COM port",          // ERR_COM_CLOSE
    //    "illegal COM status",             // ERR_COM_STATUS
    //    "COM context cannot be NULL",     // ERR_COM_CONTEXT_NULL
    //    "cannot parse status code",       // ERR_CMD_STATUS_PARSE
    //    "command execution timeout",      // ERR_CMD_TIMEOUT
    //}; // error_description

    //error_t last_error()
    //{
    //    return g_error;
    //}

    //const char* error_str(error_t e)
    //{
    //    return hvg::error_description[e];
    //}

    struct error_t {
        static enum id_t {
            ERR_NONE,
            ERR_STATUS,
            ERR_COM_OPEN,
            ERR_COM_CLOSE,
            ERR_COM_STATUS,
            ERR_COM_CONTEXT_NULL,
            ERR_CMD_STATUS_PARSE,
            ERR_CMD_TIMEOUT,
        } id;
        const static char* desc[];
        static id_t last_error;
    };
    
    // error handing:
    // error class shall have following fields:
    // 1.  enum id_t id
    // 2.  const char* [] desc
    // 3.  static int last_error

    template <class E>
    void set_error(typename E::id_t e)
    {
        E::last_error = e;
    }

    template <class E>
    typename E::id_t last_error()
    {
        return E::last_error;
    }

    template <class E>
    static inline const char* error_str(typename E::id_t e)
    {
        return E::desc[e];
    }

    template <class E>
    static inline const char* last_error_str()
    {
        return error_str<E>(last_error<E>());
    }

    static const char* COMMMAND_STOPS = "\r\n";
    constexpr size_t BUF_SIZE = 4096;

    struct context_t {
        enum status_t {
            PENDING,
            SENDING,
            SENT,
            RECIEVING,
            //RECIEVED,      // not used
            //TIMEOUT
        };
        int port;
        int baud;
        char mode[4];
        int pos;
        int len;
        char buf[BUF_SIZE];
        status_t status;
    };


    enum callback_return_t {
        CONTINUE,
        BREAK,
    };

    typedef int (*HVG_CALLBACK_SEND)(const char* msg, int n);

    context_t* open(int port, int baud, const char* mode);

    int close(context_t* context);

    int send(context_t* context, const char* command);

    int recv(context_t* context, char* msg, int timeout = 200 /* ms */);

    int send(context_t* context, const char* command, char* msg, int* condition, int timeout = 100 /* ms */);

    int send(context_t* context, const char* command, HVG_CALLBACK_SEND callback);

    int init_rs232(int port, int baud);
}
#endif // !_HVG_SIEMENS_INCLUDE_H_

#ifdef HVG_SIEMENS_IMPLEMENTATION
#ifndef HVG_SIEMENS_IMPLEMENTED
#define HVG_SIEMENS_IMPLEMENTED

#include "cl.h"

namespace mod::hvg::control {

    error_t::id_t error_t::last_error = error_t::ERR_NONE;
    const char* error_t::desc[] = {
    "no error",                       // ERR_NONE
    "HVG status is not ready",        // ERR_STATUS
    "cannot open COM port",           // ERR_COM_OPEN
    "cannot close COM port",          // ERR_COM_CLOSE
    "illegal COM status",             // ERR_COM_STATUS
    "COM context cannot be NULL",     // ERR_COM_CONTEXT_NULL
    "cannot parse status code",       // ERR_CMD_STATUS_PARSE
    "command execution timeout",      // ERR_CMD_TIMEOUT
    };

    static error_t error;

    context_t* open(int port, int baud, const char* mode)
    {
        if (RS232_OpenComport(port, baud, mode, 0))
        {
            // hvg::g_error = ERR_COM_OPEN;
            set_error<error_t>(error_t::ERR_COM_OPEN);
            return NULL;
        }
        context_t* context = (context_t*)malloc(sizeof(context_t));
        context->port = port;
        context->baud = baud;
        context->pos = 0;
        context->len = 0;
        memset(context->buf, 0, BUF_SIZE);
        context->status = context_t::PENDING;
        strncpy(context->mode, mode, 4);
        return context;
    }

    int close(context_t* context)
    {
        if (context != NULL) {
            cl::dealloc(context);
            return SUCCESS;
        }
        else {
            set_error<error_t>(error_t::ERR_COM_CONTEXT_NULL);
            return FAILURE;
        }
    }

    int send(context_t* context, const char* command)
    {
        if (context == NULL) {
            set_error<error_t>(error_t::ERR_COM_CONTEXT_NULL);
            return FAILURE;
        }

        // TODO: check command is valid

        const char* stops = COMMMAND_STOPS;
        static char send_buf[4096];
        const int port = context->port;
        context_t::status_t& status = context->status;
        context_t::status_t old_status = status;

        // check status: send only when status is PENDING
        if (status != context_t::PENDING) { 

            // hvg::g_error = ERR_COM_STATUS;
            set_error<error_t>(error_t::ERR_COM_STATUS);
            return FAILURE;
        }

        // always flush TX buffer before sending command
        RS232_flushTX(port);

        // Send command to HVG
        sprintf(send_buf, "%s%s", command, stops);
        status = context_t::SENDING;
        RS232_cputs(port, send_buf);
        status = context_t::SENT;
        status = context_t::PENDING;

        return SUCCESS;
    }

    int recv(context_t* context, char* msg, int timeout)
    {
        if (context == NULL) {
            // hvg::g_error = ERR_COM_CONTEXT_NULL;
            set_error<error_t>(error_t::ERR_COM_CONTEXT_NULL);
            return FAILURE;
        }

        const char* stops = COMMMAND_STOPS;
        static char send_buf[4096];
        const int port = context->port;
        context_t::status_t& status = context->status;
        context_t::status_t old_status = status;

        // check status: send only when status is PENDING
        if (status != context_t::PENDING) { 
            // hvg::g_error = ERR_COM_STATUS;
            set_error<error_t>(error_t::ERR_COM_STATUS);
            return FAILURE;
        }

        if (msg == NULL) {
            // in case of no return data
            status = context_t::PENDING;
            return 0;
        }
        unsigned char* buf = (unsigned char*)context->buf;
        int& n = context->len;
        int& pos = context->pos;
        time_t start = time(NULL);
        time_t now = time(NULL);

        status = context_t::RECIEVING;
        n = RS232_PollComport(port, buf, BUF_SIZE);
        pos = 0;

        char* d = msg;
        while (difftime(now, start) < timeout / 1000.0) {
            if (n > 0) {
                buf[n] = 0;   /* always put a "null" at the end of a string! */
                char* s = (char*)buf + pos;
                while (n > 0) {
                    SPDLOG_DEBUG("pos = {:d}, len = {:d}, s = {}", pos, n, *s);
                    if (*s == '\n' && *(d - 1) == '\r') {
                        *(d - 1) = 0;
                        status = context_t::PENDING;
                        return d - msg - 1;
                    }
                    *d++ = *s++;
                    pos++, n--;
                }
            }
            n = RS232_PollComport(port, buf, BUF_SIZE);
            pos = 0;
            now = time(NULL);
        }
        // hvg::g_error = ERR_CMD_TIMEOUT;
        set_error<error_t>(error_t::ERR_CMD_TIMEOUT);
        return FAILURE;
    }

    int send(context_t* context, const char* command, char* msg, int* condition, int timeout)
    {
        if (context == NULL) {
            //hvg::g_error = ERR_COM_CONTEXT_NULL;
            set_error<error_t>(error_t::ERR_COM_CONTEXT_NULL);
            return FAILURE;
        }

        // TODO: check command is valid

        const char* stops = COMMMAND_STOPS;
        static char send_buf[4096];
        const int port = context->port;
        context_t::status_t& status = context->status;
        context_t::status_t old_status = status;

        // check status: send only when status is PENDING
        if (status != context_t::PENDING) { // && status != context_t::TIMEOUT) {
            // hvg::g_error = ERR_COM_STATUS;
            set_error<error_t>(error_t::ERR_COM_STATUS);
            return FAILURE;
        }

        // always flush RX/TX buffer before sending command
        RS232_flushRXTX(port);

        // Send command to HVG
        sprintf(send_buf, "%s%s", command, stops);
        status = context_t::SENDING;
        RS232_cputs(port, send_buf);
        status = context_t::SENT;

        if (msg == NULL) {
            // in case of no return data
            status = context_t::PENDING;
            return 0;
        }

        unsigned char* buf = (unsigned char*)context->buf;
        int& n = context->len;
        int& pos = context->pos;
        time_t start = time(NULL);
        time_t now = time(NULL);

        status = context_t::RECIEVING;
        n = RS232_PollComport(port, buf, BUF_SIZE);
        pos = 0;

        char* d = msg;
        while (difftime(now, start) < timeout / 1000.0) {
            if (n > 0) {
                buf[n] = 0;   /* always put a "null" at the end of a string! */
                char* s = (char*)buf + pos;
                while (n > 0) {
                    //SPDLOG_DEBUG("pos = {:d}, len = {:d}, s = {2.2X}, d = {0.2X} \n", pos, n, *s, *d);
                    SPDLOG_DEBUG("pos = {:d}, len = {:d}, s = {}", pos, n, *s);
                    if (*s == '\n' && *(d - 1) == '\r') {
                        *(d - 1) = 0;
                        const int len = d - msg - 1;
                        if (strncmp((char*)msg, ">RDY", 4) != 0) {
                            // hvg::g_error = ERR_CMD_STATUS_PARSE;
                            set_error<error_t>(error_t::ERR_CMD_STATUS_PARSE);
                            status = context_t::PENDING;
                            return len;
                        }

                        // in case the HVG status is changed 
                        int cond0 = 0;
                        int cond1 = 0;
                        int rtv = sscanf((char*)msg, ">RDY %x %x", &cond0, &cond1);
                        if (rtv != 2) {
                            status = context_t::PENDING;
                            return FAILURE;
                        }
                        *condition = (cond0 << 16) | cond1;
                        s++, pos++, n--;    // skip \n
                        d = msg;
                    }
                    *d++ = *s++;
                    pos++, n--;
                }
            }
            n = RS232_PollComport(port, buf, BUF_SIZE);
            pos = 0;
            now = time(NULL);
            // spdlog::debug("difftime = {:f}", difftime(now, start));
        }
        status = context_t::PENDING;
        // hvg::g_error = ERR_CMD_TIMEOUT;
        set_error<error_t>(error_t::ERR_CMD_TIMEOUT);
        return FAILURE;
    }
   
    int send(context_t* context, const char* command, HVG_CALLBACK_SEND callback)
    {
        if (context == NULL) {
            // hvg::g_error = ERR_COM_CONTEXT_NULL;
            set_error<error_t>(error_t::ERR_COM_CONTEXT_NULL);
            return FAILURE;
        }

        // TODO: check command is valid

        const char* stops = COMMMAND_STOPS;
        static char send_buf[4096];
        const int port = context->port;
        context_t::status_t& status = context->status;
        context_t::status_t old_status = status;

        // check status: send only when status is PENDING
        if (status != context_t::PENDING) { // && status != context_t::TIMEOUT) {
            // hvg::g_error = ERR_COM_STATUS;
            set_error<error_t>(error_t::ERR_COM_STATUS);
            return FAILURE;
        }

        // always flush RX/TX buffer before sending command
        RS232_flushRXTX(port);

        // Send command to HVG
        sprintf(send_buf, "%s%s", command, stops);
        status = context_t::SENDING;
        RS232_cputs(port, send_buf);
        status = context_t::SENT;

        unsigned char* buf = (unsigned char*)context->buf;
        int& n = context->len;
        int& pos = context->pos;
        time_t start = time(NULL);
        time_t now = time(NULL);

        status = context_t::RECIEVING;
        n = RS232_PollComport(port, buf, BUF_SIZE);
        pos = 0;

        char msg[1024];
        char* d = msg;
        while (1) {
            if (n > 0) {
                buf[n] = 0;   /* always put a "null" at the end of a string! */
                char* s = (char*)buf + pos;
                while (n > 0) {
                    //SPDLOG_DEBUG("pos = {:d}, len = {:d}, s = {2.2X}, d = {0.2X} \n", pos, n, *s, *d);
                    SPDLOG_DEBUG("pos = {:d}, len = {:d}, s = {}", pos, n, *s);
                    if (*s == '\n' && *(d - 1) == '\r') {
                        *(d - 1) = 0;
                        const int len = d - msg - 1;
                        int e = callback(msg, len);
                        if (e == BREAK) {
                            status = context_t::PENDING;
                            return 0;
                        }
                        s++, pos++, n--;    // skip \n
                        d = msg;
                    }
                    *d++ = *s++;
                    pos++, n--;
                }
            }
            n = RS232_PollComport(port, buf, BUF_SIZE);
            pos = 0;
        }
    }

    //int send(int port, const char* command, char* str, int timeout = 100 /* ms */)
    //{
    //    const char* stops = hvg::COMMMAND_STOPS;
    //    static char send_buf[4096];

    //    sprintf(send_buf, "%s%s", command, stops);
    //    RS232_cputs(port, send_buf);

    //    unsigned char r_buf[4096];
    //    time_t start = time(NULL);
    //    time_t now = time(NULL);
    //    printf("difftime = %f", difftime(now, start));
    //    while (difftime(now, start) < timeout / 1000.0) {
    //        int n = RS232_PollComport(port, r_buf, 4095);
    //        if (n > 0) {
    //            r_buf[n] = 0;   /* always put a "null" at the end of a string! */
    //            char* s = (char*)r_buf;
    //            char* d = str;
    //            while (n-- > 0) {
    //                if (*s == '\n' && *(d - 1) == '\r') {
    //                    *(d - 1) = 0;
    //                    return d - str - 1;
    //                }
    //                else {
    //                    *d++ = *s++;
    //                }
    //            }
    //        }
    //        now = time(NULL);
    //    }
    //    return 0;
    //}

    int init_rs232(int port, int baud)
    {
        //unsigned char buf[4096];
        char mode[] = { '8','N','2',0 };

        if (RS232_OpenComport(port, baud, mode, 0))
        {
            // hvg::g_error = ERR_COM_OPEN;
            set_error<error_t>(error_t::ERR_COM_OPEN);
            return FAILURE;
        }
        return port;
    }
}
#endif // HVG_SIEMENS_IMPLEMENTED
#endif // HVG_SIEMENS_IMPLEMENTATION

