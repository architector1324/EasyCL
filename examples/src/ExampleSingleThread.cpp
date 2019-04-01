#include <iostream>
#include <EasyCL/EasyCL.hpp>

int main(){
    ecl::Program prog = ecl::Program::loadProgram("kernels/SingleThread.cl");
    ecl::Kernel kern = "test";
    ecl::Variable<int> a = 0;

    a += 3;

    auto p = ecl::System::getPlatform(0);
    ecl::Computer video(0, p, ecl::DEVICE::GPU);

    video.send({&a});

    ecl::Thread th(prog, kern, {&a}, &video);
    th.join();

    std::cout << a << std::endl;

    ecl::System::free();
    return 0;
}