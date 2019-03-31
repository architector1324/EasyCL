#include <iostream>
#include "EasyCL.hpp"

int main(){
    ecl::Program prog = ecl::Program::loadProgram("kernel.cl");
    ecl::Kernel kern = "test";
    ecl::Array<int> a(12);

    auto p = ecl::System::getPlatform(0);
    ecl::Computer video(0, p, ecl::DEVICE::GPU);

    video.send({&a});
    video.compute(prog, kern, {&a}, {12}, {3});
    video.grab({&a});

    std::cout << a << std::endl;

    for(size_t i = 0; i < 12; i++)
        std::cout << a[i] << " ";
    std::cout << std::endl;

    ecl::System::free();
    return 0;
}