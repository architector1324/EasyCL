#include <iostream>
#include <EasyCL/EasyCL.hpp>

int main(){
	// load program and setup kernel
    auto prog = ecl::Program::load("program.cl");
    ecl::Kernel kern = "simd";

	// get platform, setup the computer
    auto plat = ecl::System::getPlatform(0);
    auto video = ecl::Computer(0, plat, ecl::DEVICE::GPU);

	// setup data container
	ecl::array<int> a(12);

	// compute
    ecl::Frame frame = {prog, kern, {&a}};

    video << a;
    video.grid(frame, {12}, {3});
    video >> a;

	// output
    for(size_t i = 0; i < 12; i++)
        std::cout << a[i] << std::endl;

    ecl::System::release();
    return 0;
}