#include "cl.h"
#include "console.h"


int main(int, char**)
{
    //setlocale(LC_ALL, "chs");
    setlocale(LC_ALL, "zh-CN.UTF8");

    //wchar_t hello[256] = L"我们";
    //wprintf(L"%s\n", hello);
    //printf(u8"这个用了u8 张三\n");
    //printf("这个没用u8 张三\n");
    //printf("%ls\n", L"他们");


    spdlog::set_level(spdlog::level::trace); // Set global log level to debug
    SPDLOG_INFO("CBCT Console starting ...");

    //// CBCT init
    // modal::init();
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
    ////connect_to_hvg.join();
    connect_to_upstream_server.join();

    control::drop();
    // modal::drop();

    return 0;
}
