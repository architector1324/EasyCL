#include "platform.hpp"

cl_uint ecl::Platform::platforms_count = 0;
cl_uint* ecl::Platform::devices_counts = nullptr;
cl_platform_id* ecl::Platform::platforms = nullptr;
cl_device_id** ecl::Platform::devices = nullptr;
bool ecl::Platform::initialized = false;

void ecl::Platform::checkPlatformIndex(size_t platform_index){
    if(platform_index >= platforms_count) throw std::runtime_error("invalid platform");
}

void ecl::Platform::init(){
    error = clGetPlatformIDs(0, nullptr, &platforms_count);
    checkError();

    platforms = new cl_platform_id[platforms_count];
    devices_counts = new cl_uint[platforms_count];
    devices = new cl_device_id*[platforms_count];

    error = clGetPlatformIDs(platforms_count, platforms, nullptr);
    checkError();

    for(size_t i(0); i < platforms_count; i++){
        error = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, nullptr, &devices_counts[i]);
        checkError();

        devices[i] = new cl_device_id[devices_counts[i]];
        error = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, devices_counts[i], devices[i], nullptr);
        checkError();
    }

    initialized = true;
}

cl_uint ecl::Platform::getPlatformsCount(){
    return platforms_count;
}
cl_uint ecl::Platform::getDevicesCount(size_t platform_index){
    checkPlatformIndex(platform_index);
    return devices_counts[platform_index];
}

cl_platform_id* ecl::Platform::getPlatform(size_t platform_index){
    checkPlatformIndex(platform_index);
    return platforms + platform_index;
}
cl_device_id* ecl::Platform::getDevice(size_t platform_index, size_t device_index){
    checkPlatformIndex(platform_index);
    if(device_index >= devices_counts[platform_index]) throw std::runtime_error("invalid device");
    return devices[platform_index] + device_index;
}

std::string ecl::Platform::getPlatformInfo(size_t platform_index, cl_platform_info info){
    size_t info_size;
    cl_platform_id platform = *getPlatform(platform_index);

    error = clGetPlatformInfo(platform, info, 0, nullptr, &info_size);
    checkError();

    char* info_src = new char[info_size];
    error = clGetPlatformInfo(platform, info, info_size, info_src, nullptr);
    checkError();

    return info_src;
}

std::string ecl::Platform::getDeviceInfo(size_t platform_index, size_t device_index, cl_device_info info){
    size_t info_size;
    cl_device_id device = *getDevice(platform_index, device_index);

    error = clGetDeviceInfo(device, info, 0, nullptr, &info_size);
    checkError();

    if(info == (CL_DEVICE_EXTENSIONS | CL_DEVICE_NAME | CL_DEVICE_PROFILE | CL_DEVICE_VENDOR | CL_DEVICE_VERSION | CL_DRIVER_VERSION)){
        char* info_src = new char[info_size];
        error = clGetDeviceInfo(device, info, info_size, info_src, nullptr);
        checkError();

        return info_src;
    }

    cl_uint* info_src = new cl_uint[info_size];
    error = clGetDeviceInfo(device, info, info_size, info_src, nullptr);
    checkError();

    return std::to_string(info_src[0]);
}

bool ecl::Platform::isInitialized(){
    return initialized;
}

void ecl::Platform::free(){
    if(initialized){
        if(devices != nullptr){
            for(size_t i(0); i < platforms_count; i++){
                for(size_t j(0); j < devices_counts[i]; j++){
                    error = clReleaseDevice(devices[i][j]);
                    checkError();
                }
                delete[] devices[i];
            } 
            delete[] devices;
        }
        if(devices_counts != nullptr) delete[] devices_counts;
        if(platforms != nullptr) delete[] platforms;
        
    }
}