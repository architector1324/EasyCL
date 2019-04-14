#include <iostream>
#include <EasyCL/EasyCL.hpp>

int main(){
    auto prog = ecl::Program::load("program.cl");
    ecl::Kernel kern = "simd";
    ecl::Array<int> a(12);

    auto plat = ecl::System::getPlatform(0);
    auto video = ecl::Computer(0, plat, ecl::DEVICE::GPU);

    ecl::Frame frame = {prog, kern, {&a}};

    video << a;
    video.grid(frame, {12}, {3});
    video >> a;

    for(size_t i = 0; i < 12; i++)
        std::cout << a[i] << std::endl;

    ecl::System::release();
    return 0;
}