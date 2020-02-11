#ifndef CONSOLE_INCLUDE_UI_H 
#define CONSOLE_INCLUDE_UI_H

#include <memory>

#include "cl.h"
#include "ui/image.h"

namespace ui
{
    bool init();
    void run();
    void drop();
}

#endif // CONSOLE_INCLUDE_UI_H

#ifdef CONSOLE_UI_IMPLEMENTATION
#ifndef CONSOLE_UI_IMPLEMENTED
#define CONSOLE_UI_IMPLEMENTED

#include "control/control.hxx"

#include "window.h"

#include "ui/app.hxx"

namespace ui
{
    bool init()
    {
        return true;
    }

    void run()
    {
        auto app = cl::build_unique<ui::app_t>(ui::drop);
        ui::run(app.get());
    }

    void drop()
    {}
}
#endif // !CONSOLE_UI_IMPLEMENTED
#endif   // CONSOLE_UI_IMPLEMENTATION
