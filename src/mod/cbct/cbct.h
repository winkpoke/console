#pragma once

#include <vector>
#include <utility>

namespace mod::cbct {
    using mod_deps_t = std::vector<std::pair<const char*, const char*>>;
    const static char* mod_name = "cbct";
    const static char* mod_version = "0.0.1";
    const static mod_deps_t mod_deps = { {"hvg", "0.0.1"}, 
                                         {"fpd", "0.0.1"} };
}