#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>

#include "cl.h"
#include "websocket.h"

namespace websocket {
    bool init(websocket_t* s, const char* url) {
        s->socket = cl::alloc<ix::WebSocket>();
        new(s->socket)ix::WebSocket;

        strncpy(s->url, url, 1024);
        s->socket->setUrl(url);

        ix::initNetSystem();
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

    bool connect() {
        return true;
    }

    bool send() {
        return true;
    }
}