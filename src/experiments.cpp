// some silly experiments

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "toml++/toml.h"

#include "cl.h"
#include "mod/hnd/hnd.hxx"


#include <future>
#include <chrono>
#include <iostream>


extern "C" {

    uint32_t addition(uint32_t a, uint32_t b);


    cl::usize image_u16_get_width(image_u16_t* image)
    {
        return sil::get_width(image);
    }
    cl::usize image_u16_get_height(image_u16_t* image)
    {
        return sil::get_height(image);
    }
}

void confile_file(std::string_view doc);

int32_t add(int32_t a, int32_t b)
{
    return a + b;
}

// a non-optimized way of checking for prime numbers:
bool is_prime(int x) {
    for (int i = 2; i < x; ++i) if (x % i == 0) return false;
    return true;
}

void test_futre()
{
    // call function asynchronously:
    std::future<bool> fut = std::async(is_prime, 700020007);

    // do something while waiting for function to set future:
    std::cout << "checking, please wait";
    std::chrono::milliseconds span(100);
    while (fut.wait_for(span) == std::future_status::timeout)
        std::cout << '.';

    bool x = fut.get();

    std::cout << "\n700020007 " << (x ? "is" : "is not") << " prime.\n";
}

void experiments() 
{
    //wchar_t hello[256] = L"我们";
    //wprintf(L"%s\n", hello);
    //printf(u8"这个用了u8 张三\n");
    //printf("这个没用u8 张三\n");
    //printf("%ls\n", L"他们");

    std::cout << "addition: " << addition(100, 100) << std::endl;

    const char* raw_file_name = R"(C:\Projects\CBCT\data\headneck_1024x1024\raw\headneck_360_1024.raw.001)";
    FILE* fp = fopen(raw_file_name, "r");
    if (fp == nullptr) {
        return;
    }

    constexpr cl::u64 len = 1024 * 1024 * 2;
    char* buf = (char*)malloc(len);
    memset(buf, 0, len);
    fread(buf, len, 1, fp);

    image_u16_t* image = cl::build_raw<image_u16_t>(1024, 1024, 1, (cl::u16*)buf);
    //std::cout << "image width: " << func(image) << std::endl;

    hnd_header_t* header = hnd_header_build();
    hnd_header_set_angle(header, 180.0);
    hnd_header_drop(header);

    uintptr_t hnd_data_size = 0;
    uint8_t* hnd_data = encode((uint8_t*)buf, 1024, 1024, 2, &hnd_data_size);

    auto hnd = cl::build_raw<mod::hnd::modal::hnd_t>(image, 0.417, 0.417, 1.0);

    char* header_buf = (char*)hnd_header_to_raw(hnd->header);

    const char* hnd_file_name = R"(C:\Projects\CBCT\data\headneck_1024x1024\hnd\Scan1\headneck_360_1024.001.hnd)";
    int n = mod::hnd::control::write_to_file(hnd, hnd_file_name);
    if (n > 0) {
        SPDLOG_INFO("hnd file: {:s} with {:d} bytes", hnd_file_name, n);
    }
    else {
        SPDLOG_INFO("hnd file write error.");
    }

    SPDLOG_INFO("Config file ...");
    fp = fopen(R"(config.toml)", "r");
    if (fp == nullptr) {
        SPDLOG_WARN("cannot open config.toml file");
    } 

    printf("-------------------> %d", set_callback(add));
    
    //n = fread(buf, sizeof(char), 1024 * 1024 * 2, fp);
    //assert(n < 1024 * 1024 * 2);
    //buf[n] = '\0';
    //confile_file(buf);

    test_futre();
}

#define TOML_EXCEPTIONS 0
#include "toml++/toml.h"

void confile_file(std::string_view doc)
{
    auto config = toml::parse(doc);
    //auto node = config["raw_data_folder"];
    //auto v = node.value<std::string_view>();
    //SPDLOG_INFO("config: raw_data_folder = {:s}", config["raw_data_folder"].value_or("error"));
    //SPDLOG_INFO("config: raw_data_basename = {:s}", config["raw_data_basename"].value_or("error"));

    //std::chrono::time_point;
    //struct Data {
    //    int a;
    //    double b;
    //} data = {10, 100.0};
    //std::hash<Data>(data);
}