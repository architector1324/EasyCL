EasyCL (Easy Computing Language) - this is a С++ library based on the OpenCL specification, but it is not its object-oriented wrapper.

In pure OpenCL, there is one inconvenience, which can be conditionally called the dependence of the levels. 
So, any argument is strictly tied to the function in which it appears. 
The function is strictly tied to the program in which it is declared. 
And the program is strictly tied to the device in which it will be executed.

Because of this tight binding, pure OpenCL loses its flexibility and convenience in writing code.
The EasyCL library was created in order to solve the above problem. 
At this stage, the library will allow executing programs on video cards. 
In the future, the version will be expanded to execution on any devices.

-----------------Library Elements-------------
1) ecl::Platform
It is a static object that contains all information about all platforms and devices in the system.
The platforms are initialized automatically when creating the GPU object, but you can do this manually through the method ecl::Platform::init(cl_device_type type)

2) ecl::GPU
This is the object of the video card, to which you can send data, compute program functions and receive data back.

ecl::GPU::sendData 
ecl::GPU::compute
ecl::GPU::receiveData

3) ecl::GPUArgument
This is an abstraction representing the argument of any function running on any device. 
On the physical layer contains a pointer to the data, their size and the type of buffer memory.
This argument can be used in any function and in any program. 
Also, at any time you can change the data pointer, data size and memory type.

ecl::GPUArgument::setPtr();
ecl::GPUArgument::setArrSize();
ecl::GPUArgument::setMemType();

4) ecl::GPUFunction
This is an abstraction of a function that can be executed in any program on any device.

5) ecl::GPUProgram
This is an abstraction of the program that can be executed on any device.

-----------------Hello World------------------

#include <iostream>
#include "easycl.hpp"

using namespace std;
using namespace ecl;

int main(){
	string src = "__kernel void add(__global float* a, __global float* b, __global float* result) {"
		"    size_t i = get_global_id(0);"
		"    result[i] = a[i] + b[i];"
		"}";

	GPU video(0, 0);
	cout << video.getError() << endl;

	float A[3] = { 1.0f, 2.0f, 3.0f };
	float B[3] = {-0.5f, -0.5f, -0.5f};
	float C[3];

	GPUArgument a(A, 3 * sizeof(float), CL_MEM_READ_ONLY);
	GPUArgument b(B, 3 * sizeof(float), CL_MEM_READ_ONLY);
	GPUArgument c(C, 3 * sizeof(float), CL_MEM_READ_WRITE);

	GPUFunction add("add");
	GPUProgram program(src);
	
	// In fact, your program starts here

	cout << video.sendData({&a, &b}) << endl;
	cout << video.compute(&program, &add, {&a, &b, &c}, {3}, 1) << endl;
	cout << video.receiveData({&c}) << endl;
	
	// ends here

	for (size_t i(0); i < 3; i++) cout << C[i] << " ";
	cout << endl;

	return 0;
}


-----------------An example of level independence------------------

#include <iostream>
#include "easycl.hpp"

using namespace std;
using namespace ecl;

float A[5];

void printA() {
	for (size_t i(0); i < 5; i++) cout << A[i] << " ";
	cout << endl;
}

int main()
{
	string src0 = "__kernel void test0(__global float* arr) {"
		"    arr[get_global_id(0)] = 1.0f;"
		"}"
		"__kernel void test1(__global float* arr) {"
		"    arr[get_global_id(0)] = 2.0f;"
		"}";

	string src1 = "__kernel void test0(__global float* arr) {"
		"    arr[get_global_id(0)] = 3.0f;"
		"}"
		"__kernel void test1(__global float* arr) {"
		"    arr[get_global_id(0)] = 4.0f;"
		"}";

	GPU video(0, 1);
	cout << video.getError() << endl;

	GPUArgument arr(A, 5 * sizeof(float), CL_MEM_READ_WRITE);
	GPUFunction test0("test0"), test1("test1");
	GPUProgram program0(src0), program1(src1);

	cout << video.compute(&program0, &test0, {&arr}, {5}, 1) << endl;
	cout << video.receiveData({&arr}) << endl;
	printA();

	cout << video.compute(&program0, &test1, {&arr}, {5}, 1) << endl;
	cout << video.receiveData({&arr}) << endl;
	printA();

	cout << video.compute(&program1, &test0, {&arr}, {5}, 1) << endl;
	cout << video.receiveData({&arr}) << endl;
	printA();

	cout << video.compute(&program1, &test1, {&arr}, {5}, 1) << endl;
	cout << video.receiveData({&arr}) << endl;
	printA();
}