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

const char* ecl::GPUProgram::loadProgram(const char* filename){
    std::ifstream f(filename, std::ios::binary);
    if(!f.is_open()) throw std::runtime_error("wrong program filename");

    std::string temp;
    std::getline (f, temp, (char)f.eof());
    f.close();

    char* result = new char[temp.length() + 1];
    strncpy(result, temp.c_str(), temp.length() + 1);

    return result;
}

const cl_program* ecl::GPUProgram::getProgram(cl_context* context) const{
    return &program.at(context);
}
const char* ecl::GPUProgram::getProgramSource() const{
    return program_source;
}
size_t ecl::GPUProgram::getProgramSourceLength() const{
    return program_source_length;
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