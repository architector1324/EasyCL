#include <iostream>
#include "EasyCL.hpp"

int main(){
    ecl::GPU video(0, 0);

    ecl::GPUProgram prog = ecl::GPUProgram::loadProgram("kernel.cl");
    ecl::GPUKernel kern = "test";

    size_t A[12] = {0};
    ecl::GPUArgument a(A, 12 * sizeof(size_t), CL_MEM_READ_WRITE);

    size_t i = 0;
    ecl::GPUArgument gpu_i(&i, sizeof(size_t), CL_MEM_READ_ONLY);

    video.sendData({&a, &gpu_i});
    
    while(i < 12){
        video.thread(&prog, &kern, {&a, &gpu_i});
        i++;
        video.sendData({&gpu_i});
    }
    video.threads_join();

    video.receiveData({&a});

    for(size_t i = 0 ; i < 12; i++) std::cout << A[i] << " ";
    std::cout << std::endl;

    ecl::Platform::free();

    return 0;
}
