#include <iostream>
#include "EasyCL.hpp"

struct Vertex{
    float x, y, z;
};

int main(){
    ecl::Program prog = ecl::Program::loadProgram("kernel.cl");
    ecl::Kernel kern = "test";
    ecl::Variable<Vertex> v({0, 0, 0}, ecl::ACCESS::READ_WRITE);

    auto p = ecl::System::getPlatform(0);
    ecl::Computer video(0, p, ecl::DEVICE::GPU);

    video.sendData({&v});

    ecl::Thread th(prog, kern, {&v}, &video);
    th.join();

    std::cout << v.getValue().x << std::endl;
    std::cout << v.getValue().y << std::endl;
    std::cout << v.getValue().z << std::endl;

    ecl::System::free();
    return 0;
}