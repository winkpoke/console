#ifndef _INCLUDE_WEBSOCKET_H
#define _INCLUDE_WEBSOCKET_H

#include <string.h>
#include <functional>

// #include "ixwebsocket/IXWebSocketMessage.h"

#include "cl.h"

namespace ix {
    class WebSocket;
}

namespace websocket {
    struct websocket_t {
        char url[1024];
        ix::WebSocket* socket;
    };
    bool init(websocket_t* s, const char* url);
    void drop(websocket_t* s);

    bool connect(websocket_t* s, cl::u64 timeout = 5000);
    bool send(websocket_t* s, const char* text);
    //void on_recv_text(websocket_t* s, void(*callback)(const char* msg));
    void on_recv_text(websocket_t* s, std::function<void(const char*)> callback);
}

#endif // _INCLUDE_WEBSOCKET_H

#ifdef WEBSOCKET_IMPLEMENTATION
#ifndef WEBSOCKET_IMPLEMENTED
#define WEBSOCKET_IMPLEMENTED

#include <iostream>

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>

#include "cl.h"
//#include "websocket.h"

namespace websocket {
    bool init(websocket_t* s, const char* url) {
        s->socket = cl::alloc<ix::WebSocket>();
        new(s->socket)ix::WebSocket;

        strncpy(s->url, url, 1024);
        s->socket->setUrl(url);

        ix::initNetSystem();

        s->socket->setOnMessageCallback([](const ix::WebSocketMessagePtr& msg)
            {
                if (msg->type == ix::WebSocketMessageType::Message)
                {
                    std::cout << msg->str << std::endl;
                }
            }
        );

        return true;
    }

    void drop(websocket_t* s) {
        if (s) {
            ix::uninitNetSystem();
            if (s->socket) {
                s->socket->~WebSocket();
                free(s->socket);
            }
            free(s);
        }
    }

    bool connect(websocket_t* s, cl::u64 timeout)
    {
        const cl::u64 sleep_ms = 200;
        const cl::u64 n_loop = timeout / sleep_ms;

        s->socket->start();
        ix::ReadyState state = s->socket->getReadyState();
        for (cl::u32 i = 0; i < n_loop; ++i) {
            if (state == ix::ReadyState::Connecting || state == ix::ReadyState::Closed) {
                //std::cout << "Connecting to " << s->url << " retry ... " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
                state = s->socket->getReadyState();
            }
            else {
                //s->socket->send("<HELLO");
                return true;
            }
        }
        return false;
    }

    bool send(websocket_t* s, const char* text) {
        s->socket->send(text);
        return true;
    }

    void on_recv_text(websocket_t* s, std::function<void (const char*)> callback)
    {
        s->socket->setOnMessageCallback([=](const ix::WebSocketMessagePtr& msg) {
            if (msg->type == ix::WebSocketMessageType::Message)
            {
                if (msg->binary == false) {
                    callback(msg->str.c_str());
                }
            }
            });
    }

    /*void set_*/
}
#endif // !WEBSOCKET_IMPLEMENTED
#endif // WEBSOCKET_IMPLEMENTATION