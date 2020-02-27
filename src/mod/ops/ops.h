#pragma once

#include <thread>
#include <atomic>
#include "cl.h"
#include <librealsense2/rs.hpp>

namespace mod::ops {
    using mod_deps_t = std::vector<std::pair<const char*, const char*>>;
    const static char* mod_name = "ops";
    const static char* mod_version = "0.0.1";
    const static mod_deps_t mod_deps = { };

}

namespace mod::ops::control {
    struct ops_t {
        rs2::pipeline pipeline;
        std::thread* thread;
        std::atomic_flag runnable;
    };
    bool init(ops_t* p);

    void drop(ops_t* p);

    void start(ops_t* ops);

    void capture(ops_t* ops);
}

