#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <vector>
#include <string>
#include "error.hpp"

#ifndef EASYCL_PLATFORM
#define EASYCL_PLATFORM

namespace ecl{
    class Platform : public Error{
    private:
        static cl_uint platforms_count; // количество opencl платформ в системе
        static cl_uint* devices_counts; // количество вычислительных устройств в каждой платформе

        static cl_platform_id* platforms; // указатель на массив opencl платформ
        static cl_device_id** devices; // указатель на вычислительные устройства каждой платформы

        static bool initialized; // платформы инициализированны

        static void checkPlatformIndex(size_t platform_index);
    public:
        static void init();

        static cl_uint getPlatformsCount();
        static cl_uint getDevicesCount(size_t platform_index);
        static bool isInitialized();

        static cl_platform_id* getPlatform(size_t platform_index);
        static cl_device_id* getDevice(size_t platform_index, size_t device_index);

        static std::string getPlatformInfo(size_t platform_index, cl_platform_info info);
        static std::string getDeviceInfo(size_t platform_index, size_t device_index, cl_device_info info);

        static void free();
    };
}

#endif // EASYCL_PLATFORM
