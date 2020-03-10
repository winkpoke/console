#pragma once

#include "control/hvg.hxx"
#include "control/siemens.hxx"

namespace mod::hvg {
    using mod_deps_t = std::vector<std::pair<const char*, const char*>>;
    const static char* mod_name = "hvg";
    const static char* mod_version = "0.0.1";
    const static mod_deps_t mod_deps = { };
}