#include <iostream>
#include <EasyCL/EasyCL.hpp>

struct Vertex{
    float x, y, z;
};

int main(){
    ecl::Program prog = ecl::Program::loadProgram("kernels/Struct.cl");
    ecl::Kernel kern = "test";
    ecl::Variable<Vertex> v({0, 0, 0});

    auto p = ecl::System::getPlatform(0);
    ecl::Computer video(0, p, ecl::DEVICE::GPU);

	video << v;

    ecl::Thread th(prog, kern, {&v}, &video);
    th.join();

    std::cout << v.getValue().x << std::endl;
    std::cout << v.getValue().y << std::endl;
    std::cout << v.getValue().z << std::endl;

    ecl::System::free();
    return 0;
}