#include <iostream>
#include <EasyCL/EasyCL.hpp>

int main(){
    ecl::Program prog = ecl::Program::load("kernels/SIMD.cl");
    ecl::Kernel kern = "test";
    ecl::Array<int> a(12);

    auto p = ecl::System::getPlatform(0);
    ecl::Computer video(0, p, ecl::DEVICE::GPU);

    std::cout << p << std::endl;
    std::cout << video << std::endl;

	video << a;
    video.compute(prog, kern, {&a}, {12}, {3});
	video >> a;

    std::cout << a << std::endl;

    for(size_t i = 0; i < 12; i++)
        std::cout << a[i] << " ";
    std::cout << std::endl;

	video.release(a);
    ecl::System::free();

    return 0;
}