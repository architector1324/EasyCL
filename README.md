[![CodeFactor](https://www.codefactor.io/repository/github/architector1324/easycl/badge)](https://www.codefactor.io/repository/github/architector1324/easycl)
[![Donate with Bitcoin](https://en.cryptobadges.io/badge/micro/1SrJvWx4MD1D7SPV236swT5jYJhug86tX)](https://en.cryptobadges.io/donate/1SrJvWx4MD1D7SPV236swT5jYJhug86tX)

# Easy Computing Library

## Overview
***EasyCL*** is an open heterogeneous computing library (*GPL v 3*) based on the [***OpenCL***](https://www.khronos.org/opencl/) specification. It's header-only library.

It is designed for easy and convenient use, at the same time optimized. It is not built on the OpenCL object wrapper from [**Khronos**](https://www.khronos.org/), but uses the original [***The OpenCL 1.2 C Specification***](https://www.khronos.org/registry/OpenCL/specs/opencl-1.2.pdf).

The library allows you to bypass some of the inconveniences of the original *OpenCL*, providing work with **abstractions** of different levels, allows you to bypass the **restriction of hard binding** of arguments to the kernel, kernels to the program, programs to the final device.

* At the moment, only OpenCL 1.2 is supported

## Installation
 1) Install OpenCL library on your system
 1) Clone the repo `$ git clone https://github.com/architector1324/EasyCL`
 2) Copy `EasyCL.hpp` to your project

## Hello, World (SIMD)
 1) Copy 'EasyCL.hpp' to project folder
 2) Create `main.cpp`:

```c++
#include <iostream>
#include "EasyCL.hpp"

int main(){
    ecl::Program prog = ecl::Program::loadProgram("kernel.cl");
    ecl::Kernel kern = "test";
    ecl::Array<int> a(12, ecl::ACCESS::READ_WRITE);

    auto p = ecl::System::getPlatform(0);
    ecl::Computer video(0, p, ecl::DEVICE::GPU);

    video.sendData({&a});
    video.compute(prog, kern, {&a}, {12}, {3});
    video.receiveData({&a});

    for(size_t i = 0; i < 12; i++)
        std::cout << a[i] << " ";
    std::cout << std::endl;

    ecl::System::free();
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
1 1 1 2 2 2 3 3 3 4 4 4
```

## Hello, World (Single Thread)
 1) Copy 'EasyCL.hpp' to project folder
 2) Create `main.cpp`:

```c++
#include <iostream>
#include "EasyCL.hpp"

int main(){
    ecl::Program prog = ecl::Program::loadProgram("kernel.cl");
    ecl::Kernel kern = "test";
    ecl::Variable<int> a(0, ecl::ACCESS::READ_WRITE);

    auto p = ecl::System::getPlatform(0);
    ecl::Computer video(0, p, ecl::DEVICE::GPU);

    video.sendData({&a});

    ecl::Thread th(prog, kern, {&a}, &video);
    th.join();

    std::cout << a.getValue() << std::endl;

    ecl::System::free();
    return 0;
}
```

 3) Create `kernel.cl`:
```c
__kernel void test(__global int* a){
    a[0] = 15;
}
```

 4) Type in terminal:
```bash
$ g++ -lOpenCL -o a.out main.cpp
$ ./a.out
```

Output:
```
15
```

## API
### Abstractions
#### Arguments
It is an abstraction of the arguments of the kernel of a program. Once created, the argument can be used in different kernels and in different OpenCL programs. There are two different types of arguments:
1) Variable
```c++
ecl::Variable<T> var(const& T value);
ecl::Variable<T> var(ecl::ACCESS);
ecl::Variable<T> var(const T& value, ecl::ACCESS);
```

To get /set value:
```c++
const T& getValue() const;
void setValue(const T& value);
```

Also you can use overloaded operators:
```c++
var = const T&;
var += const T&;
var -= const T&;
var *= const T&;
var /= const T&;
var++;
var--;
```

2) Array
```c++
ecl::Array<T> array(size_t array_size);
ecl::Array<T> array(size_t array_size, ecl::ACCESS);
ecl::Array<T> array(const T* array, size_t array_size, ecl::CONTROL);
ecl::Array<T> array(T* array, size_t array_size, ecl::ACCESS, ecl::CONTROL);
```

To get pointer to array:
```c++
const T* getConstArray() const;
T* getArray();
```

To change pointer to array:
```c++
void setArray(const T* array, size_t array_size);
void setArray(T* array, size_t array_size, ecl::ACCESS);
```

Also you can use overloaded access operator:
```c++
T& array[size_t]
```

Parameters:
```c++
ecl::ACCESS::READ; // read-only for value or array
ecl::ACCESS::WRITE; // write-only for value or array
ecl::ACCESS::READ_WRITE;
```

```c++
ecl::CONTROL::BIND; // after deleting an object, it frees memory from array pointer
ecl::CONTROL::FREE; // doesn't free array, you have to free it manually
```

#### Kernels
This is an abstraction of kernels in the OpenCL program. Once created, the kernel can be used in various programs.
```c++
ecl::Kernel myKern = "name_of_my_kernel";
```
To change kernel name:
```c++
void setKernelName(const char* name);
```

#### Programs
This is an abstraction of the program from the devices of the final execution. Once created, the program can be used on various devices.
```c++
ecl::Program myProg = "__kernel void name_of_my_kernel(){...}";
```
Also you can load source program from file:
```c++
static const char* loadProgram(const char* filename);
```
To change program:
```c++
void setProgramSource(const char* src, size_t len);
```

#### Computers
This is an abstraction of the execution device, which is a logical execution device. One physical execution device may have several computers.
```c++
ecl::Computer video(size_t device_index, const ecl::Platform* platform, ecl::DEVICE);
```

```c++
ecl::DEVICE::CPU; // CPU
ecl::DEVICE::GPU; // GPU
ecl::DEVICE::ACCEL; // Accelerator
```

Sending data to Device:
```c++
void sendData(std::vector<ArgumentBase*> args);
```
Receiving data from Device:
```c++
void receiveData(std::vector<ArgumentBase*> args);
```
Execute program on Device (SIMD):
```c++
void compute(ecl::Program& prog, ecl::Kernel& kern, std::vector<ArgumentBase*> args, std::vector<size_t> global_work_size);
void compute(ecl::Program& prog, ecl::Kernel& kern, std::vector<ArgumentBase*> args, std::vector<size_t> global_work_size, std::vector<size_t> local_work_size);
```

#### Threads
A thread is an abstraction of a thread on an executable device. After creating a thread, it will be immediately executed on the device, and the data will also be read when synchronizing the thread with the host or when it is destroyed.

Execute program on Device (Single thread):
```c++
ecl::Thread th(ecl::Program& prog, ecl::Kernel& kern, std::vector<ArgumentBase*> args, ecl::Computer* video);
```
Sync single threads:
```c++
void join();
```