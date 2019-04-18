#include <iostream>
#include <EasyCL/EasyCL.hpp>

int main(){
    auto prog = ecl::Program::load("program.cl");
    ecl::Kernel kern = "single";
	ecl::var<int> a = 3;

    auto plat = ecl::System::getPlatform(0);
    auto video = ecl::Computer(0, plat, ecl::DEVICE::GPU);

    ecl::Frame frame = {prog, kern, {&a}};

    video << a;
    video.task(frame);
    video >> a;

    std::cout << a << std::endl;

    ecl::System::release();
    return 0;
}