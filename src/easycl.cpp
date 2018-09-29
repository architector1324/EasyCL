#include "easycl.hpp"

//Copyright (c) Architector 2018, gluhoy1324@gmail.com
//This file is part of EasyCL.

//    EasyCL is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    EasyCL is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with EasyCL.  If not, see <https://www.gnu.org/licenses/>.

bool ecl::Initializable::Initialized() const
{
    return initialized;
}

bool ecl::SharedInitializable::shared_initialized = false;
bool ecl::SharedInitializable::SharedInitialized()
{
    return shared_initialized;
}

void ecl::ErrorObject::generateError(){
    size_t size = error_vector.size();
    error = "";
    for (size_t i(0); i < size; i++) if (error_vector.at(i) != "ok") error += error_vector.at(i) + (i < size - 1 ? "\n" : "");
    if (error == "") error = "ok";
}
void ecl::ErrorObject::setError(std::string error)
{
	if (error != "ok") has_error = true;
    this->error_vector.push_back(error);
    this->generateError();
}
int ecl::ErrorObject::getErrorCode() const
{
	return error_code;
}
void ecl::ErrorObject::clearErrors()
{
    error_vector.clear();
}
const std::string& ecl::ErrorObject::getError() const
{
    return (const std::string&)error;
}
const std::vector<std::string>& ecl::ErrorObject::getErrorVector() const
{
    return error_vector;
}
bool ecl::ErrorObject::HasError() const
{
	return has_error;
}

std::vector<std::string> ecl::SharedErrorObject::shared_error_vector;
std::string ecl::SharedErrorObject::shared_error = "";
int ecl::SharedErrorObject::shared_error_code = 0;
bool ecl::SharedErrorObject::has_shared_error = false;
void ecl::SharedErrorObject::setSharedError(std::string error)
{
	if (error != "ok") has_shared_error = true;
    shared_error_vector.push_back(error);
    generateSharedError();
}

void ecl::SharedErrorObject::generateSharedError(){
    size_t size = shared_error_vector.size();
    shared_error = "";
    for (size_t i(0); i < size; i++) if (shared_error_vector.at(i) != "ok") shared_error += shared_error_vector.at(i) + (i < size - 1 ? "\n" : "");
    if (shared_error == "") shared_error = "ok";
}
int ecl::SharedErrorObject::getSharedErrorCode()
{
	return shared_error_code;
}
void ecl::SharedErrorObject::clearSharedErrors()
{
    shared_error_vector.clear();
}
std::string& ecl::SharedErrorObject::getSharedError()
{
    return shared_error;
}
std::vector<std::string>& ecl::SharedErrorObject::getSharedErrorVector()
{
    return shared_error_vector;
}
bool ecl::SharedErrorObject::HasSharedError()
{
	return has_shared_error;
}

ecl::Error::Error(std::string prefix)
{
	this->prefix = prefix;
}
bool ecl::Error::checkError(ErrorObject* obj)
{
    std::string temp = getError(obj->getErrorCode());
	if (temp != "ok") {
		obj->setError(prefix + temp);
		return true;
	}
	else obj->setError("ok");
	return false;
}
bool ecl::Error::checkSharedError()
{
	std::string temp;
	temp = getError(SharedErrorObject::getSharedErrorCode());
	if (temp != "ok") {
		SharedErrorObject::setSharedError(prefix + temp);
		return true;
	}
	else SharedErrorObject::setSharedError("ok");
	return false;
}
void ecl::Error::setError(ErrorObject* obj, std::string error_str)
{
	obj->setError(error_str);
}
void ecl::Error::setSharedError(std::string error_str)
{
	SharedErrorObject::setSharedError(error_str);
}

ecl::CLError PlatformInitError("Unable to initializate platforms: ");
ecl::GPGPUError PlatformInitError2("Unable to initializate platforms: ");
ecl::CLError DeviceInitError("Unable to initializate devices: ");
ecl::CLError CreateContextError("Unable to create context: ");
ecl::CLError CreateQueueError("Unable to crate command queue:");
ecl::CLError CreateProgramError("Unable to create program: ");
ecl::CLError BuildProgramError("Unable to compile program: ");
ecl::CLError FunctionError("Unable to initializate function: ");
ecl::CLError CreateKernelError("Unable to create kernel: ");
ecl::CLError CreateBufferError("Unable to create buffer: ");
ecl::CLError SetKernelArgError("Unable to set kernel arguments: ");
ecl::CLError WriteBufferError("Unable to send data to GPU: ");
ecl::CLError ExecuteError("Unable to execute the function: ");
ecl::CLError ReadBufferError("Unable to receive data from GPU: ");
ecl::GPGPUError GPUInitError("Unable to init GPU: ");

cl_uint ecl::Platform::platforms_count;
cl_platform_id* ecl::Platform::platforms;
cl_uint* ecl::Platform::devices_count;
cl_device_id** ecl::Platform::devices;
void ecl::Platform::checkPlatform(size_t platform_index)
{
    if(platform_index >= platforms_count){
        shared_error_code = CL_INVALID_PLATFORM;
        PlatformInitError.checkSharedError();
    }
}
void ecl::Platform::checkDevice(size_t platform_index, size_t device_index)
{
    checkPlatform(platform_index);
    if(device_index >= devices_count[platform_index]){
        shared_error_code = CL_INVALID_DEVICE;
        DeviceInitError.checkSharedError();
    }
}

std::string& ecl::Platform::init(cl_device_type type)
{

    if (shared_initialized) {
		shared_error_code = -1;
		PlatformInitError2.checkSharedError();
		return getSharedError();
	}
    clGetPlatformIDs(0, nullptr,& platforms_count);

	platforms = new cl_platform_id[platforms_count];
	shared_error_code = clGetPlatformIDs(platforms_count, platforms, nullptr);
	if (PlatformInitError.checkSharedError()) return getSharedError();

	devices_count = new cl_uint[platforms_count];
	devices = new cl_device_id*[platforms_count];

	for (size_t i(0); i < platforms_count; i++) {
        clGetDeviceIDs(platforms[i], type, 0, nullptr,& devices_count[i]);
		devices[i] = new cl_device_id[devices_count[i]];
		shared_error_code = clGetDeviceIDs(platforms[i], type, devices_count[i], devices[i], nullptr);
		if (DeviceInitError.checkSharedError()) return getSharedError();
	}

    shared_initialized = true;
	return getSharedError();
}

cl_platform_id* ecl::Platform::getPlatform(size_t i)
{
	return platforms + i;
}
cl_device_id* ecl::Platform::getDevice(size_t platform_i, size_t i)
{
    return devices[platform_i] + i;
}

cl_uint ecl::Platform::getPlatformsCount()
{
    return platforms_count;
}


ecl::CLError::CLError(std::string prefix) : Error(prefix) {}
const char* ecl::CLError::getError(int error) const
{
	switch (error)
	{
	case 0:
		return "ok";
		break;
	case -1:
		return "device not found";
		break;
	case -2:
		return "device not available";
		break;
	case -3:
		return "compiler not available";
		break;
	case -4:
		return "mem object allocation failure";
		break;
	case -5:
		return "out of resources";
		break;
	case -6:
		return "out of host memory";
		break;
	case -7:
		return "profiling info not available";
		break;
	case -8:
		return "mem copy overlap";
		break;
	case -9:
		return "image format mismatch";
		break;
	case -10:
		return "image format not supported";
		break;
	case -11:
		return "build program failure";
		break;
	case -12:
		return "map failure";
		break;
	case -13:
		return "misaligned sub buffer offset";
		break;
	case -14:
		return "exec status error for events in wait list";
		break;
	case -15:
		return "compile program failure";
		break;
	case -16:
		return "linker not available";
		break;
	case -17:
		return "link program failure";
		break;
	case -18:
		return "device partition failed";
		break;
	case -19:
		return "kernel argument info not available";
		break;
	case -30:
		return "invalid value";
		break;
	case -31:
		return "invalid device type";
		break;
	case -32:
		return "invalid platform";
		break;
	case -33:
		return "invalid device";
		break;
	case -34:
		return "invalid context";
		break;
	case -35:
		return "invalid queue properties";
		break;
	case -36:
		return "invalid command queue";
		break;
	case -37:
		return "invalid host ptr";
		break;
	case -38:
		return "invalid mem object";
		break;
	case -39:
		return "invalid image format descriptor";
		break;
	case -40:
		return "invalid image size";
		break;
	case -41:
		return "invalid sampler";
		break;
	case -42:
		return "invalid binary";
		break;
	case -43:
		return "invalid build options";
		break;
	case -44:
		return "invalid program";
		break;
	case -45:
		return "invalid program executable";
		break;
	case -46:
		return "invalid kernel name";
		break;
	case -47:
		return "invalid kernel definition";
		break;
	case -48:
		return "invalid kernel";
		break;
	case -49:
		return "invalid argument index";
		break;
	case -50:
		return "invalid argument value";
		break;
	case -51:
		return "invalid argument size";
		break;
	case -52:
		return "invalid kernel arguments";
		break;
	case -53:
		return "invalid work dimension";
		break;
	case -54:
		return "invalid work group size";
		break;
	case -55:
		return "invalid work item size";
		break;
	case -56:
		return "invalid global offset";
		break;
	case -57:
		return "invalid event wait list";
		break;
	case -58:
		return "invalid event";
		break;
	case -59:
		return "invalid operation";
		break;
	case -60:
		return "invalid gl object";
		break;
	case -61:
		return "invalid buffer size";
		break;
	case -62:
		return "invalid mip level";
		break;
	case -63:
		return "invalid global work size";
		break;
	case -64:
		return "invalid property";
		break;
	case -65:
		return "invalid image descriptor";
		break;
	case -66:
		return "invalid compiler options";
		break;
	case -67:
		return "invalid linker options";
		break;
	case -68:
		return "invalid device partition count";
		break;
	case -1000:
		return "invalid gl sharegroup reference khr";
		break;
	case -1001:
		return "platform not found khr";
		break;
	case -1002:
		return "invalid d3d10 device khr";
		break;
	case -1003:
		return "invalid d3d10 resource khr";
		break;
	case -1004:
		return "d3d10 resource already acquired khr";
		break;
	case -1005:
		return "d3d10 resource not acquired khr";
		break;
	default:
		return "unknown opencl error";
		break;
	}
}

ecl::GPGPUError::GPGPUError(std::string prefix) : Error(prefix) {}
const char* ecl::GPGPUError::getError(int error) const {
	switch (error) {
	case 0:
		return "ok";
	case -1:
		return "platforms are already initialized";
		break;
	case -2:
		return "gpu is not initialized";
		break;
	case -3:
		return "gpu is already initialized";
		break;
	default:
		return "unknow gpgpu error";
	}
}

ecl::GPU::GPU(size_t platform_index, size_t device_index)
{

    if (!Platform::SharedInitialized()) Platform::init(CL_DEVICE_TYPE_GPU);
    Platform::checkDevice(platform_index, device_index);
    if (Platform::HasSharedError()){
        setError(Platform::getSharedError());
    } else{
        this->device = Platform::getDevice(platform_index, device_index);

        context = clCreateContext(nullptr, 1, device, nullptr, nullptr,& error_code);
        if (CreateContextError.checkError(this)) throw;

        queue = clCreateCommandQueue(context,* device, 0,& error_code);
        if (CreateQueueError.checkError(this)) throw;

        initialized = true;
    }
}
const std::string& ecl::GPU::sendData(const std::vector<GPUArgument*>& args)
{
	for (size_t i(0); i < args.size(); i++) {
		setError(args.at(i)->checkBuffer(&context));
		if (has_error) return getError();

        error_code = clEnqueueWriteBuffer(queue,* args.at(i)->getArgument(&context), CL_TRUE, 0, args.at(i)->getArrSize(), args.at(i)->getPtr(), 0, nullptr, nullptr);
		if (WriteBufferError.checkError(this)) return getError();
	}
	return getError();
}
const std::string& ecl::GPU::compute(GPUProgram* prog, GPUFunction* func, const std::vector<GPUArgument*>& args, const std::vector<size_t>& global_work_size)
{
	setError(prog->checkProgram(&context, device));
	if (has_error) return getError();

	setError(func->checkKernel(prog->getProgram(&context)));
	if (has_error) return getError();

	for (size_t i(0); i < args.size(); i++) {
		setError(args.at(i)->checkBuffer(&context));
		if (has_error) return getError();

		error_code = clSetKernelArg(*func->getFunction(prog->getProgram(&context)), i, sizeof(cl_mem), args.at(i)->getArgument(&context));
		if (SetKernelArgError.checkError(this)) return getError();
	}

    error_code = clEnqueueNDRangeKernel(queue,* func->getFunction(prog->getProgram(&context)), global_work_size.size(), nullptr, global_work_size.data(), nullptr, 0, nullptr, nullptr);
	ExecuteError.checkError(this);

    clFinish(queue);

	return getError();
}
const std::string& ecl::GPU::receiveData(const std::vector<GPUArgument*>& args)
{
	for (size_t i(0); i < args.size(); i++) {
		setError(args.at(i)->checkBuffer(&context));
		if (has_error) return getError();

        error_code = clEnqueueReadBuffer(queue,* args.at(i)->getArgument(&context), CL_TRUE, 0, args.at(i)->getArrSize(), args.at(i)->getPtr(), 0, nullptr, nullptr);
		if (ReadBufferError.checkError(this)) return getError();
	}
    return getError();
}

ecl::GPU::~GPU()
{
    if(initialized){
        clReleaseDevice(*device);
        clReleaseContext(context);
        clReleaseCommandQueue(queue);
    }
}

const char* ecl::GPUProgram::getBuildError(cl_context* context, cl_device_id* device)
{
    size_t err_size;
    clGetProgramBuildInfo(program.at(context),* device, CL_PROGRAM_BUILD_LOG, 0, nullptr,
		&err_size);
    char* result = new char[err_size];
    clGetProgramBuildInfo(program.at(context),* device, CL_PROGRAM_BUILD_LOG, err_size, result, 0);

	return result;
}
ecl::GPUProgram::GPUProgram(const char* source, size_t length)
{
	this->program_source = source;
	this->program_source_length = length;
}
ecl::GPUProgram::GPUProgram(const char* source){
    this->program_source = source;
    this->program_source_length = strlen(source);
}
const std::string& ecl::GPUProgram::checkProgram(cl_context* context, cl_device_id* device)
{

    if(program.find(context) == program.end()){
        program.emplace(context, clCreateProgramWithSource(*context, 1, (const char**)&program_source, (const size_t*)&program_source_length,& error_code));
        if(CreateProgramError.checkError(this)) return getError();

        error_code = clBuildProgram(program.at(context), 0, nullptr, nullptr, nullptr, nullptr);
        if (BuildProgramError.checkError(this)) BuildProgramError.setError(this, getBuildError(context, device));
    }

	return getError();
}
cl_program*  ecl::GPUProgram::getProgram(cl_context*  context)
{
    return& program.at(context);
}
ecl::GPUProgram::GPUProgram(std::string&  source)
{
	this->program_source = source.c_str();
	this->program_source_length = source.length();
}
ecl::GPUProgram::~GPUProgram()
{
	for (std::pair<cl_context*, cl_program> p : program) clReleaseProgram(p.second);
}

ecl::GPUFunction::GPUFunction(const char* name)
{
	this->name = name;
}
const std::string& ecl::GPUFunction::checkKernel(cl_program* program)
{

    if(function.find(program) == function.end()){
        function.emplace(program, clCreateKernel(*program, name,& error_code));
        CreateKernelError.checkError(this);
    }

	return getError();
}
cl_kernel* ecl::GPUFunction::getFunction(cl_program* program)
{
    return& function.at(program);
}
ecl::GPUFunction::~GPUFunction()
{
	for (std::pair<cl_program*, cl_kernel> p : function) clReleaseKernel(p.second);
}

ecl::GPUArgument::GPUArgument()
{
    this->ptr = nullptr;
    this->arr_size = 0;
    this->mem_type = 0;
}
ecl::GPUArgument::GPUArgument(const void* ptr, size_t arr_size)
{
    this->ptr = (void*)ptr;
    this->arr_size = arr_size;
    this->mem_type = CL_MEM_READ_ONLY;
}
ecl::GPUArgument::GPUArgument(void* ptr, size_t arr_size, cl_mem_flags mem_type)
{
	this->ptr = ptr;
	this->arr_size = arr_size;
	this->mem_type = mem_type;
}
const std::string& ecl::GPUArgument::checkBuffer(cl_context* context)
{

    if(buffer.find(context) == buffer.end()){
        buffer.emplace(context, clCreateBuffer(*context, mem_type, arr_size, nullptr,& error_code));
        CreateBufferError.checkError(this);
    }

	return getError();
}
void*  ecl::GPUArgument::getPtr() const
{
	return ptr;
}
size_t ecl::GPUArgument::getArrSize() const
{
	return arr_size;
}
const cl_mem* ecl::GPUArgument::getArgument(cl_context* context) const
{
    return (const cl_mem*)&buffer.at(context);
}
void ecl::GPUArgument::setPtr(void* ptr)
{
	this->ptr = ptr;
}
void ecl::GPUArgument::setArrSize(size_t arr_size)
{
	this->arr_size = arr_size;
}
void ecl::GPUArgument::setMemType(cl_mem_flags mem_type)
{
	this->mem_type = mem_type;
}
ecl::GPUArgument::~GPUArgument()
{
    for (std::pair<cl_context*, cl_mem> p : buffer) clReleaseMemObject(p.second);
}




