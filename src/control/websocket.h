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
    void on_recv_text(websocket_t* s, std::function<void(const char* msg)> callback);
}

#endif // _INCLUDE_WEBSOCKET_H