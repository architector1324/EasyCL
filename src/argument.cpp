#include "argument.hpp"

ecl::GPUArgument::GPUArgument(){
    this->data_ptr = nullptr;
    this->data_size = 0;
    memory_type = 0;
}

ecl::GPUArgument::GPUArgument(const void* data_ptr, size_t data_size){
    this->data_ptr = (void*)data_ptr;
    this->data_size = data_size;
    memory_type = CL_MEM_READ_ONLY;
}

ecl::GPUArgument::GPUArgument(void* data_ptr, size_t data_size, cl_mem_flags memory_type){
    this->data_ptr = data_ptr;
    this->data_size = data_size;
    this->memory_type = memory_type;
}

void* ecl::GPUArgument::getDataPtr(){
    return data_ptr;
}
size_t ecl::GPUArgument::getDataSize() const{
    return data_size;
}
cl_mem_flags ecl::GPUArgument::getMemoryType() const{
    return memory_type;
}
const cl_mem* ecl::GPUArgument::getBuffer(cl_context* context) const{
    return &buffer.at(context);
}

void ecl::GPUArgument::checkBuffer(cl_context* context){
    if(buffer.find(context) == buffer.end()){
        buffer.emplace(context, clCreateBuffer(*context, memory_type, data_size, nullptr, &error));
        checkError();
    }
}

void ecl::GPUArgument::setDataPtr(void* data_ptr){
    this->data_ptr = data_ptr;
}
void ecl::GPUArgument::setDataSize(size_t data_size){
    this->data_size = data_size;
}
void ecl::GPUArgument::setMemoryType(cl_mem_flags memory_type){
    this->memory_type = memory_type;
}


ecl::GPUArgument::~GPUArgument(){
    for(const std::pair<cl_context*, cl_mem>& p : buffer) clReleaseMemObject(p.second);
}