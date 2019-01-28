#include "program.hpp"

const char* ecl::GPUProgram::getBuildError(cl_context* context, cl_device_id* device){
    size_t info_size;
    clGetProgramBuildInfo(program.at(context), *device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &info_size);

    char* info = new char[info_size];
    clGetProgramBuildInfo(program.at(context), *device, CL_PROGRAM_BUILD_LOG, info_size, info, nullptr);

    return info;
}

ecl::GPUProgram::GPUProgram(const char* src, size_t len){
    program_source = src;
    program_source_length = len;
}
ecl::GPUProgram::GPUProgram(const char* src){
    program_source = src;
    program_source_length = strnlen(src, 65535);
}
ecl::GPUProgram::GPUProgram(std::string& src){
    program_source = src.data();
    program_source_length = src.length();
}

const cl_program* ecl::GPUProgram::getProgram(cl_context* context) const{
    return &program.at(context);
}

void ecl::GPUProgram::checkProgram(cl_context* context, cl_device_id* device){
    if(program.find(context) == program.end()){
        program.emplace(context, clCreateProgramWithSource(*context, 1, (const char**)&program_source, (const size_t*)&program_source_length, &error));
        checkError();

        error = clBuildProgram(program.at(context), 0, nullptr, nullptr, nullptr, nullptr);
        if(error != 0) throw std::runtime_error(getBuildError(context, device));
    }
}

ecl::GPUProgram::~GPUProgram(){
    for(const std::pair<cl_context*, cl_program>& p : program) clReleaseProgram(p.second);
}