#pragma once
#include <vector>
#include <utility>

namespace mod::patient {
    using mod_deps_t = std::vector<std::pair<const char*, const char*>>;
    const static char* mod_name = "patient";
    const static char* mod_version = "0.0.1";
    const static mod_deps_t mod_deps = { };
}
