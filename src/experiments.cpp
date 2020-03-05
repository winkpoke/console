// some silly experiments

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"

#include "mod/hnd/hnd.hxx"


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

}