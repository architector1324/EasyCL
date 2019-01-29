#include "gpu.hpp"

ecl::GPU::GPU(size_t platform_index, size_t device_index){
    if(!Platform::isInitialized()) Platform::init();

    device = Platform::getDevice(platform_index, device_index);

    context = clCreateContext(nullptr, 1, device, nullptr, nullptr, &error);
    checkError();

    queue = clCreateCommandQueue(context, *device, 0, &error);
    checkError();
}

void ecl::GPU::sendData(std::vector<ecl::GPUArgument*> args){
    size_t count = args.size();
    for(size_t i(0); i < count; i++){
        ecl::GPUArgument* curr = args.at(i);
        curr->checkBuffer(&context);

        error = clEnqueueWriteBuffer(queue, *curr->getBuffer(&context), CL_TRUE, 0, curr->getDataSize(), curr->getDataPtr(), 0, nullptr, nullptr);
        checkError();
    }
    error = clFinish(queue);
    checkError();
}

void ecl::GPU::compute(GPUProgram* prog, GPUKernel* kern, std::vector<GPUArgument*> args, std::vector<size_t> global_work_size, std::vector<size_t> local_work_size){
    prog->checkProgram(&context, device);
    cl_program* prog_program = (cl_program*)prog->getProgram(&context);
    
    kern->checkKernel(prog_program);
    cl_kernel kern_kernel = *kern->getKernel(prog_program);

    size_t count = args.size();
    for (size_t i(0); i < count; i++) {
        ecl::GPUArgument* curr = args.at(i);
        curr->checkBuffer(&context);

        error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), curr->getBuffer(&context));
        checkError();
	}

    error = clFinish(queue);
    checkError();

    error = clEnqueueNDRangeKernel(queue, kern_kernel, global_work_size.size(), nullptr, global_work_size.data(), local_work_size.data(), 0, nullptr, nullptr);
	checkError();
    
    error = clFinish(queue);
    checkError();
}
void ecl::GPU::compute(GPUProgram* prog, GPUKernel* kern, std::vector<GPUArgument*> args, std::vector<size_t> global_work_size){
    prog->checkProgram(&context, device);
    cl_program* prog_program = (cl_program*)prog->getProgram(&context);
    
    kern->checkKernel(prog_program);
    cl_kernel kern_kernel = *kern->getKernel(prog_program);

    size_t count = args.size();
    for (size_t i(0); i < count; i++) {
        ecl::GPUArgument* curr = args.at(i);
        curr->checkBuffer(&context);

        error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), curr->getBuffer(&context));
        checkError();
	}

    error = clFinish(queue);
    checkError();

    error = clEnqueueNDRangeKernel(queue, kern_kernel, global_work_size.size(), nullptr, global_work_size.data(), nullptr, 0, nullptr, nullptr);
	checkError();
    
    error = clFinish(queue);
    checkError();
}

void ecl::GPU::receiveData(std::vector<ecl::GPUArgument*> args){
    size_t count = args.size();
    for(size_t i(0); i < count; i++){
        ecl::GPUArgument* curr = args.at(i);
        curr->checkBuffer(&context);

        error = clEnqueueReadBuffer(queue, *curr->getBuffer(&context), CL_TRUE, 0, curr->getDataSize(), curr->getDataPtr(), 0, nullptr, nullptr);
        checkError();
    }
    error = clFinish(queue);
    checkError();
}

ecl::GPU::~GPU(){
    error = clReleaseCommandQueue(queue);
    checkError();
    error = clReleaseContext(context);
    checkError();
}

