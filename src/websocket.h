#ifndef _INCLUDE_WEBSOCKET_H
#define _INCLUDE_WEBSOCKET_H

#include <string.h>

namespace websocket {
    struct websocket_t {
        char url[1024];
        ix::WebSocket socket;

        static bool init(websocket_t* s, const char* url);
        static void drop(websocket_t* s);
    };
}

#endif // _INCLUDE_WEBSOCKET_H