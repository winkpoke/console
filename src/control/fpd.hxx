#ifndef _CONSOLE_FPD_H_
#define _CONSOLE_FPD_H_

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "cl.h"
#include "def.h"

#include "common/flat_panel.hxx"

#include "modal/scan.hxx"

namespace control {
    struct fpd_t {
        fpd_status_t status;
        modal::scan_t* scan;
    };

    bool init(fpd_t* fpd, int width, int height);
    void drop(fpd_t* fpd);
}

#endif // !_CONSOLE_FPD_H_

#ifdef CONTROL_FPD_IMPLEMENTATION
#ifndef CONTROL_FPD_IMPLEMENTED
#define CONTROL_FPD_IMPLEMENTED

namespace control {
    // Implementations
    bool init(fpd_t* fpd, int width, int height)
    {
        if (!fpd) {
            return false;
        }

        fpd->status = FPD_UNCONNECTED;
        fpd->scan = cl::build_raw<modal::scan_t>(width, height);
        assert(fpd->scan);

        return true;
    }

    void drop(fpd_t* fpd)
    {
        if (fpd) {
            // drop(fpd->scan);
        }
        free(fpd);
    }
}

#endif // !CONTROL_FPD_IMPLEMENTED
#endif // CONTROL_FPD_IMPLEMENTATION
