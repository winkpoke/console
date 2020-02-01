#ifndef _INCLUDE_WEBSOCKET_H
#define _INCLUDE_WEBSOCKET_H

#include <string.h>

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
}

#endif // _INCLUDE_WEBSOCKET_H