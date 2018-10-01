# Easy Computing Library

## Overview
***EasyCL*** is an open heterogeneous computing library (*GPL v 3*) based on the [***OpenCL***](https://www.khronos.org/opencl/) specification.

It is designed for easy and convenient use, at the same time optimized. It is not built on the OpenCL object wrapper from [**Khronos**](https://www.khronos.org/), but uses the original [***The OpenCL 1.2 C Specification***](https://www.khronos.org/registry/OpenCL/specs/opencl-1.2.pdf).

The library allows you to bypass some of the inconveniences of the original *OpenCL*, providing work with **abstractions** of different levels, allows you to bypass the **restriction of hard binding** of arguments to the kernel, kernels to the program, programs to the final device.

* At the moment, only OpenCL 1.2 is supported
* At the moment, only calculations on GPU's are supported
## Abstractions
### Arguments
It is an abstraction of the arguments of the kernel of a program. Once created, the argument can be used in different kernels and in different OpenCL programs.
>ecl::GPUArgument argument(void* pointer, size_t array_size, cl_mem_flags mem_type)

To change the pointer to the memory area of the host at any time during program execution:
>argument.setPtr(void* pointer)

To change the buffer size at any time during program execution:
>argument.setArrSize(size_t size)

To change the buffer memory access type:
>argument.setMemType(cl_mem_flags mem_type)

### Kernels
This is an abstraction of functions in the OpenCL program. Once created, the function can be used in various programs.
>ecl::GPUFunction myFunc = "name_of_my_kernel"
### Programs
This is an abstraction of the program from the devices of the final execution. Once created, the program can be used on various devices.
>ecl::GPUProgram myProg = "__kernel void name_of_my_kernel(){...}"

### GPU's
This is an abstraction of the execution device, which is a logical execution device. One physical execution device may have several logical ones.
> ecl::GPU video(size_t platform_index, size_t device_index)


## Hello, World

```
#include <iostream>
#include "easycl.hpp"

using namespace std;
using namespace ecl;

int main()
{
    GPUProgram program = "__kernel void test(__global float* a) {"
                "a[get_global_id(0)] += (float)get_global_id(0);"
                "}";
    GPUFunction test = "test";

    float A[5] = {0.0f};
    GPUArgument a(A, 5 * sizeof(float));

    try {
        GPU video;
        video.sendData({&a});
        video.compute(&program, &test, {&a}, {5});
        video.receiveData({&a});

    } catch (const exception& e) {
        cout << e.what() << endl;
    }

    for(size_t i(0); i < 5; i++) cout << A[i] << " ";
    cout << endl;

    return 0;
}
```
