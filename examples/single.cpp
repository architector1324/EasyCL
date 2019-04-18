#include <iostream>
#include <EasyCL/EasyCL.hpp>

int main(){
	// load program and setup kernel
    auto prog = ecl::Program::load("program.cl");
    ecl::Kernel kern = "single";

	// setup the computer
    auto plat = ecl::System::getPlatform(0);
    auto video = ecl::Computer(0, plat, ecl::DEVICE::GPU);

	// setup data container
	ecl::var<int> a = 3;

	// compute
    ecl::Frame frame = {prog, kern, {&a}};

    video << a;
    video.task(frame);
    video >> a;

	// output
    std::cout << a << std::endl;

    ecl::System::release();
    return 0;
}