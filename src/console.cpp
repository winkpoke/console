#include "console.h"
#include "cl.h"

void experiments();

int main(int argc, char** argv)
{
    //setlocale(LC_ALL, "chs");
    setlocale(LC_ALL, "zh-CN.UTF8");
    
    // initializing the logger
    spdlog::set_level(spdlog::level::trace); // Set global log level to debug
    SPDLOG_INFO("CBCT Console starting ...");

    //experiments();

    //// CBCT init
    control::init();

    // create the UI loop in its own thread
    std::thread ui_run([]() {
        if (ui::init()) {
            ui::run();
            ui::drop();
        }});

    ui_run.join();

    control::drop();

#if !defined(NDEBUG)
    SPDLOG_ERROR("memory leaks: ");
    for (auto iter : cl::g_allocation) {
        SPDLOG_ERROR(" {:p} with type of {:s} ", (void*)iter.first, iter.second);
    }
    
#endif 
    return 0;
}
