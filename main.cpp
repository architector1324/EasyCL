#include <iostream>
#include "EasyCL.hpp"

int main(){
    ecl::GPU video(0, 1);

    ecl::GPUProgram prog = ecl::GPUProgram::loadProgram("kernel.cl");
    ecl::GPUKernel kern = "test";

    size_t A[12] = {0};
    ecl::GPUArgument a(A, 12 * sizeof(size_t), CL_MEM_READ_WRITE);

    video.sendData({&a});
    video.compute(&prog, &kern, {&a}, {12}, {3});
    video.receiveData({&a});

    for(size_t i = 0 ; i < 12; i++) std::cout << A[i] << " ";
    std::cout << std::endl;

    ecl::Platform::free();

    return 0;
}