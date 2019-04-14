#include <iostream>
#include <EasyCL/EasyCL.hpp>

int main() {
	ecl::Program prog = ecl::Program::load("kernels/vectors.cl");
	ecl::Kernel kern = "add";
	
	float V1[] = {5.0f, 2.0f, -3.0f, 4.0f};
	float V2[] = {-2.0f, 4.0f, 1.0f, 0.5f};

	ecl::Array<float> v1(V1, 4);
	ecl::Array<float> v2(V2, 4);
	ecl::Array<float> result(4);

	auto p = ecl::System::getPlatform(0);
	ecl::Computer video(0, p, ecl::DEVICE::GPU);

	video << v1 << v2 << result;
	video.compute(prog, kern, {&v1, &v2, &result}, {4});
	video >> result;

	std::cout << result.getConstArray()[0] << std::endl;
	std::cout << result.getConstArray()[1] << std::endl;
	std::cout << result.getConstArray()[2] << std::endl;
	std::cout << result.getConstArray()[3] << std::endl;
	
	ecl::System::free();
	return 0;
}