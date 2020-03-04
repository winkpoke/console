#include "console.h"
#include "cl.h"

void experiments();

int main(int, char**)
{
    //setlocale(LC_ALL, "chs");
    setlocale(LC_ALL, "zh-CN.UTF8");

    experiments();

    //// CBCT init
    control::init();
    std::thread connect_to_fpd(control::connect_to_fpd);
    //std::thread connect_to_hvg(modal::connect_to_hvg);
    std::thread connect_to_upstream_server(control::connect_to_upstream_server);

    std::thread ui_run([]() {
        if (ui::init()) {
            ui::run();
            ui::drop();
        }});

    ui_run.join();
    connect_to_fpd.join();
    // connect_to_hvg.join();
    connect_to_upstream_server.join();

    control::drop();
    // modal::drop();

#if !defined(NDEBUG)
    SPDLOG_ERROR("memory leaks: ");
    for (auto iter : cl::g_allocation) {
        SPDLOG_ERROR(" {:p} wit type of {:s} ", (void*)iter.first, iter.second);
    }
    
#endif 
    return 0;
}
