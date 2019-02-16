#pragma once

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <fstream>

namespace ecl{
    class Error{
    protected:
        static int error;
        static const char* getErrorString();
    public:
        static void checkError();
    };

    class Platform : public Error{
    private:
        static cl_uint platforms_count; // количество opencl платформ в системе
        static cl_uint* devices_counts; // количество вычислительных устройств в каждой платформе

        static cl_platform_id* platforms; // указатель на массив opencl платформ
        static cl_device_id** devices; // указатель на вычислительные устройства каждой платформы

        static bool initialized; // платформы инициализированны

        static void checkPlatformIndex(size_t platform_index);
    public:
        static void init();

        static cl_uint getPlatformsCount();
        static cl_uint getDevicesCount(size_t platform_index);
        static bool isInitialized();

        static cl_platform_id* getPlatform(size_t platform_index);
        static cl_device_id* getDevice(size_t platform_index, size_t device_index);

        static std::string getPlatformInfo(size_t platform_index, cl_platform_info info);
        static std::string getDeviceInfo(size_t platform_index, size_t device_index, cl_device_info info);

        static void free();
    };

    class GPUProgram : public Error{
    private:
        std::map<cl_context*, cl_program> program; // карта проргамм по контексту
        const char* program_source; // указатель на исходный текст программы
        size_t program_source_length; // длина исходного текста

        const char* getBuildError(cl_context* context, cl_device_id* device);
    public:
        GPUProgram(const char* src, size_t len);
        GPUProgram(const char* src);

        static const char* loadProgram(const char* filename);

        const cl_program* getProgram(cl_context* context) const; // получить указатель на программу
        const char* getProgramSource() const;
        size_t getProgramSourceLength() const;

        void setProgramSource(const char* src, size_t len);

        void checkProgram(cl_context* context, cl_device_id* device); // проверить программу на контекст

        ~GPUProgram();
    };

    class GPUKernel: public Error{
    private:
        std::map<cl_program*, cl_kernel> kernel; // карта ядер по программам
        const char* name; // имя ядра
    public:
        GPUKernel();
        GPUKernel(const char* name);

        void setKernelName(const char* name);

        const cl_kernel* getKernel(cl_program* program) const; // получить указатель на ядро
        void checkKernel(cl_program* program); // проверить ядро на прграмму
        ~GPUKernel();
    };

    class GPUArgument : public Error{
    private:
        std::map<cl_context*, cl_mem> buffer; // карта буферов по контексту
        void* data_ptr; // указатель на массив данных
        size_t data_size; // размер массива данных
        cl_mem_flags memory_type; // тип используемой памяти
    public:
        GPUArgument();
        GPUArgument(const void* data_ptr, size_t data_size);
        GPUArgument(void* data_ptr, size_t data_size, cl_mem_flags memory_type);

        void* getDataPtr();
        size_t getDataSize() const;
        cl_mem_flags getMemoryType() const;
        const cl_mem* getBuffer(cl_context* context) const; // получить указатель на буфер по контексту

        void checkBuffer(cl_context* context); // проверить buffer на контекст

        void setDataPtr(void* data_ptr);
        void setDataSize(size_t data_size);
        void setMemoryType(cl_mem_flags memory_type);

        ~GPUArgument();
    };

    class GPU : public Error{
        private:
            cl_device_id* device; // указатель на привязанное устройство

            cl_context context; // opencl контекст
            cl_command_queue queue; // очередь запросов на привязанное устройство
        public:
            GPU(size_t platform_index, size_t device_index);

            void sendData(std::vector<GPUArgument*> args); // отправить данные на устройство
            // выполнить программу на устройстве
            void compute(GPUProgram* prog, GPUKernel* kern, std::vector<GPUArgument*> args, std::vector<size_t> global_work_size, std::vector<size_t> local_work_size);
            void compute(GPUProgram* prog, GPUKernel* kern, std::vector<GPUArgument*> args, std::vector<size_t> global_work_size);
            void thread(GPUProgram* prog, GPUKernel* kern, std::vector<GPUArgument*> args);
            void threads_join();
            
            void receiveData(std::vector<GPUArgument*> args); // получить данные с устройства
            ~GPU();
    };


    // Error
    int Error::error = 0;

    void Error::checkError(){
        if (error != 0) throw std::runtime_error(getErrorString());
    }

    const char* Error::getErrorString()
    {
        switch (error)
        {
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

    // Platform
    cl_uint Platform::platforms_count = 0;
    cl_uint* Platform::devices_counts = nullptr;
    cl_platform_id* Platform::platforms = nullptr;
    cl_device_id** Platform::devices = nullptr;
    bool Platform::initialized = false;

    void Platform::checkPlatformIndex(size_t platform_index){
        if(platform_index >= platforms_count) throw std::runtime_error("invalid platform");
    }

    void Platform::init(){
        error = clGetPlatformIDs(0, nullptr, &platforms_count);
        checkError();

        platforms = new cl_platform_id[platforms_count];
        devices_counts = new cl_uint[platforms_count];
        devices = new cl_device_id*[platforms_count];

        error = clGetPlatformIDs(platforms_count, platforms, nullptr);
        checkError();

        for(size_t i(0); i < platforms_count; i++){
            error = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, 0, nullptr, &devices_counts[i]);
            checkError();

            devices[i] = new cl_device_id[devices_counts[i]];
            error = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, devices_counts[i], devices[i], nullptr);
            checkError();
        }

        initialized = true;
    }

    cl_uint Platform::getPlatformsCount(){
        return platforms_count;
    }
    cl_uint Platform::getDevicesCount(size_t platform_index){
        checkPlatformIndex(platform_index);
        return devices_counts[platform_index];
    }

    cl_platform_id* Platform::getPlatform(size_t platform_index){
        checkPlatformIndex(platform_index);
        return platforms + platform_index;
    }
    cl_device_id* Platform::getDevice(size_t platform_index, size_t device_index){
        checkPlatformIndex(platform_index);
        if(device_index >= devices_counts[platform_index]) throw std::runtime_error("invalid device");
        return devices[platform_index] + device_index;
    }

    std::string Platform::getPlatformInfo(size_t platform_index, cl_platform_info info){
        size_t info_size;
        cl_platform_id platform = *getPlatform(platform_index);

        error = clGetPlatformInfo(platform, info, 0, nullptr, &info_size);
        checkError();

        char* info_src = new char[info_size];
        error = clGetPlatformInfo(platform, info, info_size, info_src, nullptr);
        checkError();

        return info_src;
    }

    std::string Platform::getDeviceInfo(size_t platform_index, size_t device_index, cl_device_info info){
        size_t info_size;
        cl_device_id device = *getDevice(platform_index, device_index);

        error = clGetDeviceInfo(device, info, 0, nullptr, &info_size);
        checkError();

        if(info == (CL_DEVICE_EXTENSIONS | CL_DEVICE_NAME | CL_DEVICE_PROFILE | CL_DEVICE_VENDOR | CL_DEVICE_VERSION | CL_DRIVER_VERSION)){
            char* info_src = new char[info_size];
            error = clGetDeviceInfo(device, info, info_size, info_src, nullptr);
            checkError();

            return info_src;
        }

        cl_uint* info_src = new cl_uint[info_size];
        error = clGetDeviceInfo(device, info, info_size, info_src, nullptr);
        checkError();

        return std::to_string(info_src[0]);
    }

    bool Platform::isInitialized(){
        return initialized;
    }

    void Platform::free(){
        if(initialized){
            if(devices != nullptr){
                for(size_t i(0); i < platforms_count; i++){
                    for(size_t j(0); j < devices_counts[i]; j++){
                        error = clReleaseDevice(devices[i][j]);
                        checkError();
                    }
                    delete[] devices[i];
                } 
                delete[] devices;
            }
            if(devices_counts != nullptr) delete[] devices_counts;
            if(platforms != nullptr) delete[] platforms;
        }
    }

    // GPUProgram
    const char* GPUProgram::getBuildError(cl_context* context, cl_device_id* device){
        size_t info_size;
        clGetProgramBuildInfo(program.at(context), *device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &info_size);

        char* info = new char[info_size];
        clGetProgramBuildInfo(program.at(context), *device, CL_PROGRAM_BUILD_LOG, info_size, info, nullptr);

        return info;
    }

    GPUProgram::GPUProgram(const char* src, size_t len){
        program_source = src;
        program_source_length = len;
    }
    GPUProgram::GPUProgram(const char* src){
        program_source = src;
        program_source_length = strnlen(src, 65535);
    }

    const char* GPUProgram::loadProgram(const char* filename){
        std::ifstream f(filename, std::ios::binary);
        if(!f.is_open()) throw std::runtime_error("wrong program filename");

        std::string temp;
        std::getline (f, temp, (char)f.eof());
        f.close();

        char* result = new char[temp.length() + 1];
        strncpy(result, temp.c_str(), temp.length() + 1);

        return result;
    }

    const cl_program* GPUProgram::getProgram(cl_context* context) const{
        return &program.at(context);
    }
    const char* GPUProgram::getProgramSource() const{
        return program_source;
    }
    size_t GPUProgram::getProgramSourceLength() const{
        return program_source_length;
    }

    void GPUProgram::setProgramSource(const char* src, size_t len){
        if(program.size() == 0){
            this->program_source = src;
            this->program_source_length = len;
        } 
        else throw std::runtime_error("unable to change program until it's using");
    }

    void GPUProgram::checkProgram(cl_context* context, cl_device_id* device){
        if(program.find(context) == program.end()){
            program.emplace(context, clCreateProgramWithSource(*context, 1, (const char**)&program_source, (const size_t*)&program_source_length, &error));
            checkError();

            error = clBuildProgram(program.at(context), 0, nullptr, nullptr, nullptr, nullptr);
            if(error != 0) throw std::runtime_error(getBuildError(context, device));
        }
    }

    GPUProgram::~GPUProgram(){
        for(const std::pair<cl_context*, cl_program>& p : program) clReleaseProgram(p.second);
    }

    // GPUKernel
    GPUKernel::GPUKernel(){
    name = nullptr;
    }
    GPUKernel::GPUKernel(const char* name){
        this->name = name;
    }

    void GPUKernel::setKernelName(const char* name){
        if(kernel.size() == 0) this->name = name;
        else throw std::runtime_error("unable to change kernel name until it's using");
    }

    const cl_kernel* GPUKernel::getKernel(cl_program* program) const{
        return &kernel.at(program);
    }
    void GPUKernel::checkKernel(cl_program* program){
        if(kernel.find(program) == kernel.end()){
            kernel.emplace(program, clCreateKernel(*program, name, &error));
            checkError();
        }
    }

    GPUKernel::~GPUKernel(){
        for(const std::pair<cl_program*, cl_kernel>& p : kernel) clReleaseKernel(p.second);
    }

    // GPUArgument
    GPUArgument::GPUArgument(){
        this->data_ptr = nullptr;
        this->data_size = 0;
        memory_type = 0;
    }

    GPUArgument::GPUArgument(const void* data_ptr, size_t data_size){
        this->data_ptr = (void*)data_ptr;
        this->data_size = data_size;
        memory_type = CL_MEM_READ_ONLY;
    }

    GPUArgument::GPUArgument(void* data_ptr, size_t data_size, cl_mem_flags memory_type){
        this->data_ptr = data_ptr;
        this->data_size = data_size;
        this->memory_type = memory_type;
    }

    void* GPUArgument::getDataPtr(){
        return data_ptr;
    }
    size_t GPUArgument::getDataSize() const{
        return data_size;
    }
    cl_mem_flags GPUArgument::getMemoryType() const{
        return memory_type;
    }
    const cl_mem* GPUArgument::getBuffer(cl_context* context) const{
        return &buffer.at(context);
    }

    void GPUArgument::checkBuffer(cl_context* context){
        if(buffer.find(context) == buffer.end()){
            buffer.emplace(context, clCreateBuffer(*context, memory_type, data_size, nullptr, &error));
            checkError();
        }
    }

    void GPUArgument::setDataPtr(void* data_ptr){
        this->data_ptr = data_ptr;
    }
    void GPUArgument::setDataSize(size_t data_size){
        this->data_size = data_size;
    }
    void GPUArgument::setMemoryType(cl_mem_flags memory_type){
        this->memory_type = memory_type;
    }


    GPUArgument::~GPUArgument(){
        for(const std::pair<cl_context*, cl_mem>& p : buffer) clReleaseMemObject(p.second);
    }

    // GPU
    GPU::GPU(size_t platform_index, size_t device_index){
        if(!Platform::isInitialized()) Platform::init();

        device = Platform::getDevice(platform_index, device_index);

        context = clCreateContext(nullptr, 1, device, nullptr, nullptr, &error);
        checkError();

        queue = clCreateCommandQueue(context, *device, 0, &error);
        checkError();
    }

    void GPU::sendData(std::vector<GPUArgument*> args){
        size_t count = args.size();
        for(size_t i(0); i < count; i++){
            GPUArgument* curr = args.at(i);
            curr->checkBuffer(&context);

            error = clEnqueueWriteBuffer(queue, *curr->getBuffer(&context), CL_FALSE, 0, curr->getDataSize(), curr->getDataPtr(), 0, nullptr, nullptr);
            checkError();
        }
        error = clFinish(queue);
        checkError();
    }

    void GPU::compute(GPUProgram* prog, GPUKernel* kern, std::vector<GPUArgument*> args, std::vector<size_t> global_work_size, std::vector<size_t> local_work_size){
        prog->checkProgram(&context, device);
        cl_program* prog_program = (cl_program*)prog->getProgram(&context);
        
        kern->checkKernel(prog_program);
        cl_kernel kern_kernel = *kern->getKernel(prog_program);

        size_t count = args.size();
        for (size_t i(0); i < count; i++) {
            GPUArgument* curr = args.at(i);
            curr->checkBuffer(&context);

            error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), curr->getBuffer(&context));
            checkError();
        }

        error = clEnqueueNDRangeKernel(queue, kern_kernel, global_work_size.size(), nullptr, global_work_size.data(), local_work_size.data(), 0, nullptr, nullptr);
        checkError();
        
        error = clFinish(queue);
        checkError();
    }
    void GPU::compute(GPUProgram* prog, GPUKernel* kern, std::vector<GPUArgument*> args, std::vector<size_t> global_work_size){
        prog->checkProgram(&context, device);
        cl_program* prog_program = (cl_program*)prog->getProgram(&context);
        
        kern->checkKernel(prog_program);
        cl_kernel kern_kernel = *kern->getKernel(prog_program);

        size_t count = args.size();
        for (size_t i(0); i < count; i++) {
            GPUArgument* curr = args.at(i);
            curr->checkBuffer(&context);

            error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), curr->getBuffer(&context));
            checkError();
        }

        error = clEnqueueNDRangeKernel(queue, kern_kernel, global_work_size.size(), nullptr, global_work_size.data(), nullptr, 0, nullptr, nullptr);
        checkError();
        
        error = clFinish(queue);
        checkError();
    }

    void GPU::thread(GPUProgram* prog, GPUKernel* kern, std::vector<GPUArgument*> args){
        prog->checkProgram(&context, device);
        cl_program* prog_program = (cl_program*)prog->getProgram(&context);
        
        kern->checkKernel(prog_program);
        cl_kernel kern_kernel = *kern->getKernel(prog_program);

        size_t count = args.size();
        for (size_t i(0); i < count; i++) {
            GPUArgument* curr = args.at(i);
            curr->checkBuffer(&context);

            error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), curr->getBuffer(&context));
            checkError();
        }

        error = clEnqueueTask(queue, kern_kernel, 0, nullptr, nullptr);
        checkError();
    }
    void GPU::threads_join(){
        error = clFinish(queue);
        checkError();
    }

    void GPU::receiveData(std::vector<GPUArgument*> args){
        size_t count = args.size();
        for(size_t i(0); i < count; i++){
            GPUArgument* curr = args.at(i);
            curr->checkBuffer(&context);

            error = clEnqueueReadBuffer(queue, *curr->getBuffer(&context), CL_FALSE, 0, curr->getDataSize(), curr->getDataPtr(), 0, nullptr, nullptr);
            checkError();
        }
        error = clFinish(queue);
        checkError();
    }

    GPU::~GPU(){
        error = clReleaseCommandQueue(queue);
        checkError();
        error = clReleaseContext(context);
        checkError();
    }

}
