#include <iostream>
#include "EasyCL.hpp"

int main(){
    ecl::Program prog = ecl::Program::loadProgram("kernel.cl");
    ecl::Kernel kern = "test";
    ecl::Array<int> a(12, ecl::ACCESS::READ_WRITE);

    auto p = ecl::System::getPlatform(0);
    ecl::Computer video(0, p, ecl::DEVICE::GPU);

    video.sendData({&a});
    video.compute(prog, kern, {&a}, {12}, {3});
    video.receiveData({&a});

    for(size_t i = 0; i < 12; i++)
        std::cout << a.getConstArray()[i] << " ";
    std::cout << std::endl;

    ecl::System::free();
    return 0;
}