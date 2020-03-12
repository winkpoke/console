#pragma once

#include "control/fpd_dummy.hxx"
#include "control/fpd.hxx"
#include "control/iray.hxx"

namespace mod::fpd {
    using mod_deps_t = std::vector<std::pair<const char*, const char*>>;
    const static char* mod_name = "fpd";
    const static char* mod_version = "0.0.1";
    const static mod_deps_t mod_deps = { };
}