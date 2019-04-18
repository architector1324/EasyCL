[![CodeFactor](https://www.codefactor.io/repository/github/architector1324/easycl/badge)](https://www.codefactor.io/repository/github/architector1324/easycl)
[![Donate with Bitcoin](https://en.cryptobadges.io/badge/micro/1SrJvWx4MD1D7SPV236swT5jYJhug86tX)](https://en.cryptobadges.io/donate/1SrJvWx4MD1D7SPV236swT5jYJhug86tX)

# Easy Computing Library
![](icon.png)

Icon made by [Freepik](https://www.flaticon.com/authors/freepik) from www.flaticon.com

## Overview
***EasyCL*** is open-source (*GPL v3*) **header-only** wrapper above [***OpenCL***](https://www.khronos.org/opencl/) library. It's designed for easy and convenient use, allowing you to quickly write the host part of the program and flexibly use OpenCL kernels.

 The library is built on the original [***OpenCL 1.2 C Specification***](https://www.khronos.org/registry/OpenCL/specs/opencl-1.2.pdf). It allows you to bypass some of the inconveniences of the original *OpenCL*, providing work with **abstractions** of different levels, provides you to bypass the **restriction of hard binding**.

## Installation
 1) Install OpenCL library on your system
 1) Clone the repo `$ git clone https://github.com/architector1324/EasyCL`
 2) Copy `EasyCL.hpp` to your project

## Hello, World
 1) Copy `EasyCL.hpp` to project folder
 2) Create `main.cpp`:

```c++
#include <iostream>
#include "EasyCL.hpp"

int main(){
    // setup program and kernel
    auto prog = ecl::Program::load("kernel.cl");
    ecl::Kernel kern = "test";

    // get platform, setup the computer
    auto plat = ecl::System::getPlatform(0);
    auto video = ecl::Computer(0, plat, ecl::DEVICE::GPU);

    // setup data container
    ecl::array<int> a(12); // new int[12]

    // setup compute frame
    ecl::Frame frame = {prog, kern, {&a}};

    // 12 compute units combined into 3 groups
    video << a;
    video.grid(frame, {12}, {3});
    video >> a;

    //output
    for(size_t i = 0; i < 12; i++)
        std::cout << a[i] << std::endl;

    ecl::System::release();
    return 0;
}
```
 3) Create `kernel.cl`:
```c
__kernel void test(__global int* a){
    size_t i = get_global_id(0);
    a[i] = (int)get_group_id(0) + 1;
}
```

 4) Type in terminal:
```bash
$ g++ -lOpenCL -o a.out main.cpp
$ ./a.out
```

Output:
```
1
1
1
2
2
2
3
3
3
4
4
4
```

## FAQ
- [Wiki](https://github.com/architector1324/EasyCL/wiki)
- If you have any questions, feel free to contact me olegsajaxov@yandex.ru
