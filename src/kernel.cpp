#include "kernel.hpp"

ecl::GPUKernel::GPUKernel(const char* name){
    this->name = name;
}

const cl_kernel* ecl::GPUKernel::getKernel(cl_program* program) const{
    return &kernel.at(program);
}
void ecl::GPUKernel::checkKernel(cl_program* program){
    if(kernel.find(program) == kernel.end()){
        kernel.emplace(program, clCreateKernel(*program, name, &error));
        checkError();
    }
}

ecl::GPUKernel::~GPUKernel(){
    for(const std::pair<cl_program*, cl_kernel>& p : kernel) clReleaseKernel(p.second);
}