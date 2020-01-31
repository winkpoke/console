#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXWebSocket.h>

#include "websocket.h"

namespace websocket {
    bool websocket_t::init(websocket_t* s, const char* url) {
        strncpy(s->url, url, 1024);
        ix::initNetSystem();
        new(&s->socket)ix::WebSocket;
        s->socket.setUrl(url);
        return true;
    }

    void websocket_t::drop(websocket_t* s) {
        if (s) {
            free(s->url);
            s->socket.~WebSocket();
            ix::uninitNetSystem();
        }
        free(s);
    }

    bool connect() {
        return true;
    }

    bool send() {
        return true;
    }
}