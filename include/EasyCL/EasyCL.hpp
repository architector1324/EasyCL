#pragma once

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace ecl{
    const std::size_t MAX_PLATFORMS_COUNT = 64;
    const std::size_t MAX_DEVICES_COUNT = 64;
    const std::size_t MAX_INFO_SIZE = 1024;

    enum ACCESS{READ = CL_MEM_READ_ONLY, WRITE = CL_MEM_WRITE_ONLY, READ_WRITE = CL_MEM_READ_WRITE};
    enum DEVICE{CPU = CL_DEVICE_TYPE_CPU, GPU = CL_DEVICE_TYPE_GPU, ACCEL = CL_DEVICE_TYPE_ACCELERATOR};
    enum CONTROL{BIND, FREE};

///////////////////////////////////////////////////////////////////////////////
// Error Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Error{
    protected:
        static int error;
        static std::string getErrorString();

    public:
        static void checkError(const std::string&);
    };

///////////////////////////////////////////////////////////////////////////////
// Platform Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Platform : public Error{
    private:
        cl_platform_id platform;

        std::vector<cl_device_id> cpus;
        std::vector<cl_device_id> gpus;
        std::vector<cl_device_id> accs;

        void initDevices(std::vector<cl_device_id>&, cl_device_type);
        void freeDevices(std::vector<cl_device_id>&);
    public:
        Platform() = default;
        Platform(cl_platform_id);

        cl_device_id getDevice(std::size_t, DEVICE) const;

        std::string getPlatformInfo(cl_platform_info) const;
        std::string getDeviceInfo(std::size_t, DEVICE, cl_device_info) const;

        ~Platform();
    };

///////////////////////////////////////////////////////////////////////////////
// System Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class System : public Error{
    private:
        static std::vector<const Platform*> platforms;
        static bool initialized;
    public:
		System() = delete;
        static void init();
        static const Platform* getPlatform(std::size_t);
        static void free();
    };

///////////////////////////////////////////////////////////////////////////////
// Program Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Program : public Error{
    private:
        std::map<cl_context, cl_program> program;
        std::string source;

        std::string getBuildError(cl_context, cl_device_id);

    public:
        Program(const char*);
        Program(const std::string&);

        Program(const Program&);
        Program& operator=(const Program&);

        Program(Program&&);
        Program& operator=(Program&&);

        static std::string load(const std::string&);

        cl_program getProgram(cl_context) const;
        const std::string& getSource() const;

        Program& operator=(const std::string&);
        Program& operator=(const char*);

        Program& operator+=(const std::string&);
        Program& operator+=(const char*);
        Program& operator+=(const Program&);

        Program operator+(const std::string&);
        Program operator+(const char*);
        Program operator+(const Program&);
        friend std::ostream& operator<<(std::ostream&, const Program&);

        void setSource(const std::string&);

        bool checkProgram(cl_context, cl_device_id);

        void clearFields();
        ~Program();
    };

///////////////////////////////////////////////////////////////////////////////
// Kernel Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Kernel: public Error{
    private:
        std::map<cl_program, cl_kernel> kernel; // карта ядер по программам
        std::string name;

    public:
        Kernel(const char*);
        Kernel(const std::string&);

        Kernel(const Kernel&);
        Kernel& operator=(const Kernel&);

        Kernel(Kernel&&);
        Kernel& operator=(Kernel&&);

        Kernel& operator=(const std::string&);
        Kernel& operator=(const char*);

        Kernel& operator+=(const std::string&);
        Kernel& operator+=(const char*);
        Kernel& operator+=(const Kernel&);

        Kernel operator+(const std::string&);
        Kernel operator+(const char*);
        Kernel operator+(const Kernel&);
        friend std::ostream& operator<<(std::ostream&, const Kernel&);

        void setName(const std::string&);
        const std::string& getName() const;

        cl_kernel getKernel(cl_program) const; // получить указатель на ядро
        bool checkKernel(cl_program); // проверить ядро на прграмму

        void clearFields();
        ~Kernel();
    };

///////////////////////////////////////////////////////////////////////////////
// ArgumentBase Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class ArgumentBase : public Error{
    protected:
        std::map<cl_context, cl_mem> buffer; // карта буферов по контексту
        void* data_ptr = nullptr; // указатель на массив данных
        std::size_t data_size = 0; // размер массива данных
        cl_mem_flags memory_type = 0; // тип используемой памяти

    public:
        ArgumentBase();
        ArgumentBase(const void*, std::size_t);
        ArgumentBase(void*, std::size_t, cl_mem_flags);

        ArgumentBase(ArgumentBase&&);
        ArgumentBase& operator=(ArgumentBase&&);

        void* getDataPtr();
        std::size_t getDataSize() const;
        cl_mem_flags getMemoryType() const;
        cl_mem getBuffer(cl_context) const;

        bool checkBuffer(cl_context) const;
        void createBuffer(cl_context);

        void setDataPtr(void*);
        void setDataSize(std::size_t);
        void setMemoryType(cl_mem_flags);

        void clearBuffer(cl_context);
        virtual void clearFields();

        ~ArgumentBase();
    };

///////////////////////////////////////////////////////////////////////////////
// Variable Class Declaration
///////////////////////////////////////////////////////////////////////////////
    template<typename T> class Variable : public ArgumentBase{
        private:
            T local_value;
            CONTROL control = CONTROL::FREE;

        public:
            Variable();
            Variable(const T&);
            Variable(ACCESS);
            Variable(const T&, ACCESS);

            Variable(const Variable<T>&);
            Variable<T>& operator=(const Variable<T>&);

            Variable(Variable<T>&&);
            Variable<T>& operator=(Variable<T>&&);

            const T& getValue() const;
            void setValue(const T&);

            Variable<T>& operator++(int);
            Variable<T>& operator--(int);
            Variable<T>& operator=(const T&);
            Variable<T>& operator+=(const T&);
            Variable<T>& operator-=(const T&);
            Variable<T>& operator*=(const T&);
            Variable<T>& operator/=(const T&);
            bool operator==(const T&);
            bool operator!=(const T&);
            Variable<T> operator+(const T&);
            Variable<T> operator-(const T&);
            Variable<T> operator*(const T&);
            Variable<T> operator/(const T&);

            operator T&();
            operator const T&() const;

            void clearFields() override;
            ~Variable();
    };

///////////////////////////////////////////////////////////////////////////////
// Array Class Declaration
///////////////////////////////////////////////////////////////////////////////
    template<typename T> class Array : public ArgumentBase{
        private:
            CONTROL control = CONTROL::FREE;

        public:
            Array();
            Array(std::size_t);
            Array(std::size_t, ACCESS);
            Array(const T*, std::size_t, CONTROL control = FREE);
            Array(T*, std::size_t, ACCESS, CONTROL control = FREE);

            Array(const Array<T>&);
            Array<T>& operator=(const Array<T>&);

            Array(Array<T>&&);
            Array<T>& operator=(Array<T>&&);

            const T* getConstArray() const;
            T* getArray();

            T& operator[](std::size_t i);
            operator T*();
            operator const T*() const;

            void setArray(const T*, std::size_t);
            void setArray(T*, std::size_t, ACCESS);

            void clearFields() override;
            ~Array();
    };

///////////////////////////////////////////////////////////////////////////////
// Computer Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Computer : public Error{
        private:
            cl_device_id device = nullptr; // указатель на привязанное устройство

            cl_context context = nullptr; // opencl контекст
            cl_command_queue queue = nullptr; // очередь запросов на привязанное устройство

        public:
			Computer() = delete;
            Computer(std::size_t, const Platform*, DEVICE);

			cl_device_id getDevice() const;
			cl_context getContext() const;
			cl_command_queue getQueue() const;

			void send(ArgumentBase&, bool sync = true);
			void receive(ArgumentBase&, bool sync = true);
			void release(ArgumentBase&, bool sync = true);
			void grab(ArgumentBase&, bool sync = true);

            void send(const std::vector<ArgumentBase*>&);
			void receive(const std::vector<ArgumentBase*>&);
			void release(const std::vector<ArgumentBase*>&);
			void grab(const std::vector<ArgumentBase*>&);

            void compute(Program&, Kernel&, const std::vector<const ArgumentBase*>&, const std::vector<std::size_t>&, const std::vector<std::size_t>&);
            void compute(Program&, Kernel&, const std::vector<const ArgumentBase*>&, const std::vector<std::size_t>&);

			friend Computer& operator<<(Computer&, ArgumentBase&);
			friend Computer& operator>>(Computer&, ArgumentBase&);

            ~Computer();
    };

///////////////////////////////////////////////////////////////////////////////
// Thread Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Thread : public Error{
        private:
            cl_event sync;
            Computer* video;
            std::vector<ArgumentBase*> args;

            bool readed = false;
        public:
			Thread() = delete;
            Thread(Program&, Kernel&, const std::vector<ArgumentBase*>&, Computer*);
            void join();
            ~Thread();
    };
}

///////////////////////////////////////////////////////////////////////////////
// Error Class Definition
///////////////////////////////////////////////////////////////////////////////
int ecl::Error::error = 0;

void ecl::Error::checkError(const std::string& where){
    if (error != 0)
        throw std::runtime_error(where + ": " + getErrorString());
}

std::string ecl::Error::getErrorString(){
    switch (error){
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

///////////////////////////////////////////////////////////////////////////////
// Platform Class Definition
///////////////////////////////////////////////////////////////////////////////
void ecl::Platform::initDevices(std::vector<cl_device_id>& devs, cl_device_type type){
    cl_uint count;
    error = clGetDeviceIDs(platform, type, 0, nullptr, &count);

    if(error != CL_DEVICE_NOT_FOUND){
        cl_device_id temp[MAX_DEVICES_COUNT];

        error = clGetDeviceIDs(platform, type, count, temp, nullptr);
        checkError("Platform [init]");

        for(std::size_t i = 0; i < count; i++) devs.push_back(temp[i]);
    }
}

void ecl::Platform::freeDevices(std::vector<cl_device_id>& devs){
    for(auto& d : devs){
        error = clReleaseDevice(d);
        checkError("Platform [free]");
    }
}

ecl::Platform::Platform(cl_platform_id platform){
    this->platform = platform;
    initDevices(cpus, CL_DEVICE_TYPE_CPU);
    initDevices(gpus, CL_DEVICE_TYPE_GPU);
    initDevices(accs, CL_DEVICE_TYPE_ACCELERATOR);
}

cl_device_id ecl::Platform::getDevice(std::size_t i, DEVICE type) const{
    cl_uint count = 0;
    const cl_device_id* dev;

    if(type == CPU){
        count = cpus.size();
        dev = cpus.data();
    }else if(type == GPU){
        count = gpus.size();
        dev = gpus.data();
    }else if(type == ACCEL){
        count = accs.size();
        dev = accs.data();
    } else throw std::runtime_error("invalid device type");

    if(i >= count) throw std::runtime_error("invalid device");

    return dev[i];
}

std::string ecl::Platform::getPlatformInfo(cl_platform_info info) const{
    std::size_t info_size;

    error = clGetPlatformInfo(platform, info, 0, nullptr, &info_size);
    checkError("Platform [get info]");

    char info_src[MAX_INFO_SIZE];
    error = clGetPlatformInfo(platform, info, info_size, info_src, nullptr);
    checkError("Platform [get info]");

    return info_src;
}

std::string ecl::Platform::getDeviceInfo(std::size_t i, DEVICE type, cl_device_info info) const{
    std::size_t info_size;
    cl_device_id device = getDevice(i, type);

    error = clGetDeviceInfo(device, info, 0, nullptr, &info_size);
    checkError("Platform [get device info]");

    bool is_char = (info == CL_DEVICE_EXTENSIONS) |
                    (info == CL_DEVICE_NAME) |
                    (info == CL_DEVICE_PROFILE) |
                    (info == CL_DEVICE_VENDOR) |
                    (info == CL_DEVICE_VERSION) |
                    (info == CL_DRIVER_VERSION);

    if(is_char){
        char info_src[MAX_INFO_SIZE];
        error = clGetDeviceInfo(device, info, info_size, info_src, nullptr);
        checkError("Platform [get device info]");

        return info_src;
    }

    if(info == CL_DEVICE_MAX_WORK_ITEM_SIZES){
        std::size_t info_src[3];
        error = clGetDeviceInfo(device, info, info_size, info_src, nullptr);
        checkError("Platform [get device info]");

        return std::to_string(info_src[0]) + ", " + 
                std::to_string(info_src[1]) + ", " + std::to_string(info_src[2]);
    }

    cl_uint info_src;
    error = clGetDeviceInfo(device, info, info_size, &info_src, nullptr);
    checkError("Platform [get device info]");

    return std::to_string(info_src);
}

ecl::Platform::~Platform(){
    freeDevices(cpus);
    freeDevices(gpus);
    freeDevices(accs);
}

///////////////////////////////////////////////////////////////////////////////
// System Class Definition
///////////////////////////////////////////////////////////////////////////////
std::vector<const ecl::Platform*> ecl::System::platforms;
bool ecl::System::initialized = false;

void ecl::System::init(){
    cl_uint count;

    error = clGetPlatformIDs(0, nullptr, &count);
    checkError("System [init]");

    if(count > 0){
        cl_platform_id temp[MAX_PLATFORMS_COUNT];
        error = clGetPlatformIDs(count, temp, nullptr);
        checkError("System [init]");

        for(std::size_t i = 0; i < count; i++) platforms.push_back(new Platform(temp[i]));
    }
    initialized = true;
}

const ecl::Platform* ecl::System::getPlatform(std::size_t i){
    if(!initialized) init();
    return platforms.at(i);
}

void ecl::System::free(){
    for(auto* p : platforms) delete p;
}

///////////////////////////////////////////////////////////////////////////////
// Program Class Definition
///////////////////////////////////////////////////////////////////////////////
std::string ecl::Program::getBuildError(cl_context context, cl_device_id device){
    std::size_t info_size;
    clGetProgramBuildInfo(program.at(context), device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &info_size);

    char* info = new char[info_size];
    clGetProgramBuildInfo(program.at(context), device, CL_PROGRAM_BUILD_LOG, info_size, info, nullptr);

    return info;
}

void ecl::Program::clearFields(){
    for(const auto& p : program) clReleaseProgram(p.second);
    source.clear();
    program.clear();
}

ecl::Program::Program(const char* src){
    source = src;
}
ecl::Program::Program(const std::string& src){
    source = src;
}

ecl::Program::Program(const Program& other){
    source = other.source;
}
ecl::Program& ecl::Program::operator=(const Program& other){
    clearFields();
    source = other.source;

    return *this;
}

ecl::Program::Program(Program&& other){
    source = other.source;
    program = std::move(other.program);

    other.clearFields();
}
ecl::Program& ecl::Program::operator=(Program&& other){
    clearFields();
    source = other.source;
    program = std::move(other.program);

    other.clearFields();

    return *this;
}

std::string ecl::Program::load(const std::string& filename){
    std::ifstream f(filename, std::ios::binary);
    if(!f.is_open()) throw std::runtime_error("wrong program filename");

    std::string result;
    std::getline (f, result, (char)f.eof());
    f.close();

    return result;
}

cl_program ecl::Program::getProgram(cl_context context) const{
    return program.at(context);
}
const std::string& ecl::Program::getSource() const{
    return source;
}

namespace ecl{
    std::ostream& operator<<(std::ostream& s, const Program& other){
        s << other.getSource();
        return s;
    }
}

void ecl::Program::setSource(const std::string& src){
    if(program.size() == 0){
        source = src;
    }
    else throw std::runtime_error("unable to change program until it's using");
}


ecl::Program& ecl::Program::operator=(const std::string& src){
    setSource(src);
    return *this;
}
ecl::Program& ecl::Program::operator=(const char* src){
    setSource(src);
    return *this;
}

ecl::Program& ecl::Program::operator+=(const std::string& src){
    setSource(source + src);
    return *this;
}
ecl::Program& ecl::Program::operator+=(const char* src){
    setSource(source + src);
    return *this;
}
ecl::Program& ecl::Program::operator+=(const Program& other){
    setSource(source + other.source);
    return *this;
}

ecl::Program ecl::Program::operator+(const std::string& src){
    Program result(*this);
    result += src;
    return result;
}
ecl::Program ecl::Program::operator+(const char* src){
    Program result(*this);
    result += src;
    return result;
}
ecl::Program ecl::Program::operator+(const Program& other){
    Program result(*this);
    result += other;
    return result;
}


bool ecl::Program::checkProgram(cl_context context, cl_device_id device){
    if(program.find(context) == program.end()){
        const char* src = source.c_str();
        std::size_t len  = source.size();

        program.emplace(context, clCreateProgramWithSource(context, 1, (const char**)&src, (const std::size_t*)&len, &error));
        checkError("Program [check]");

        error = clBuildProgram(program.at(context), 0, nullptr, nullptr, nullptr, nullptr);
        if(error != 0) throw std::runtime_error(getBuildError(context, device));

        return false;
    }
    return true;
}

ecl::Program::~Program(){
    clearFields();
}

///////////////////////////////////////////////////////////////////////////////
// Kernel Class Definition
///////////////////////////////////////////////////////////////////////////////
void ecl::Kernel::clearFields(){
    for(const auto& p : kernel) clReleaseKernel(p.second);
    name.clear();
}
ecl::Kernel::Kernel(const char* name){
    this->name = name;
}
ecl::Kernel::Kernel(const std::string& name){
    this->name = name;
}

ecl::Kernel::Kernel(const Kernel& other){
    name = other.name;
}
ecl::Kernel& ecl::Kernel::operator=(const Kernel& other){
    clearFields();
    name = other.name;

    return *this;
}

ecl::Kernel::Kernel(Kernel&& other){
    name = other.name;
    kernel = std::move(other.kernel);

    other.clearFields();
}
ecl::Kernel& ecl::Kernel::operator=(Kernel&& other){
    clearFields();
    name = other.name;
    kernel = std::move(other.kernel);

    other.clearFields();

    return *this;
}

namespace ecl{
    std::ostream& operator<<(std::ostream& s, const Kernel& other){
        s << other.getName();
        return s;
    }
}

void ecl::Kernel::setName(const std::string& name){
    if(kernel.size() == 0) this->name = name;
    else throw std::runtime_error("unable to change kernel name until it's using");
}
const std::string& ecl::Kernel::getName() const{
    return name;
}

ecl::Kernel& ecl::Kernel::operator=(const std::string& src){
    setName(src);
    return *this;
}
ecl::Kernel& ecl::Kernel::operator=(const char* src){
    setName(src);
    return *this;
}

ecl::Kernel& ecl::Kernel::operator+=(const std::string& src){
    setName(name + src);
    return *this;
}
ecl::Kernel& ecl::Kernel::operator+=(const char* src){
    setName(name + src);
    return *this;
}
ecl::Kernel& ecl::Kernel::operator+=(const Kernel& other){
    setName(name + other.name);
    return *this;
}

ecl::Kernel ecl::Kernel::operator+(const std::string& src){
    Kernel result(*this);
    result += src;
    return result;
}
ecl::Kernel ecl::Kernel::operator+(const char* src){
    Kernel result(*this);
    result += src;
    return result;
}
ecl::Kernel ecl::Kernel::operator+(const Kernel& other){
    Kernel result(*this);
    result += other;
    return result;
}


cl_kernel ecl::Kernel::getKernel(cl_program program) const{
    return kernel.at(program);
}
bool ecl::Kernel::checkKernel(cl_program program){
    if(kernel.find(program) == kernel.end()){
        kernel.emplace(program, clCreateKernel(program, name.c_str(), &error));
        checkError("Kernel [check]");
        return false;
    }
    return true;
}

ecl::Kernel::~Kernel(){
    clearFields();
}

///////////////////////////////////////////////////////////////////////////////
// ArgumentBase Class Definition
///////////////////////////////////////////////////////////////////////////////
void ecl::ArgumentBase::clearFields(){
    for(const std::pair<cl_context, cl_mem>& p : buffer) clearBuffer(p.first);
    data_ptr = nullptr;
    data_size = 0;
    memory_type = 0;
    buffer.clear();
}
ecl::ArgumentBase::ArgumentBase() {
    data_ptr = nullptr;
    data_size = 0;
    memory_type = READ_WRITE;
}
ecl::ArgumentBase::ArgumentBase(const void* data_ptr, std::size_t data_size){
    this->data_ptr = (void*)data_ptr;
    this->data_size = data_size;
    memory_type = READ;
}
ecl::ArgumentBase::ArgumentBase(void* data_ptr, std::size_t data_size, cl_mem_flags memory_type){
    this->data_ptr = data_ptr;
    this->data_size = data_size;
    this->memory_type = memory_type;
}

ecl::ArgumentBase::ArgumentBase(ArgumentBase&& other){
    data_size = other.data_size;
    data_ptr = other.data_ptr;
    memory_type = other.memory_type;
    buffer = std::move(other.buffer);

    other.clearFields();
}
ecl::ArgumentBase& ecl::ArgumentBase::operator=(ArgumentBase&& other){
    clearFields();

    data_size = other.data_size;
    data_ptr = other.data_ptr;
    memory_type = other.memory_type;
    buffer = std::move(other.buffer);

    other.clearFields();

    return *this;
}

void* ecl::ArgumentBase::getDataPtr(){
    return data_ptr;
}
std::size_t ecl::ArgumentBase::getDataSize() const{
    return data_size;
}
cl_mem_flags ecl::ArgumentBase::getMemoryType() const{
    return memory_type;
}
cl_mem ecl::ArgumentBase::getBuffer(cl_context context) const{
    return buffer.at(context);
}

bool ecl::ArgumentBase::checkBuffer(cl_context context) const{
    if(buffer.find(context) == buffer.end()) return false;
    return true;
}
void ecl::ArgumentBase::createBuffer(cl_context context){
    if(!checkBuffer(context)){
        buffer.emplace(context, clCreateBuffer(context, memory_type, data_size, nullptr, &error));
        checkError("ArgumentBase [check]");
    }
}

void ecl::ArgumentBase::setDataPtr(void* data_ptr){
    this->data_ptr = data_ptr;
}
void ecl::ArgumentBase::setDataSize(std::size_t data_size){
    if(buffer.size() == 0){
        this->data_size = data_size;
    }
    else throw std::runtime_error("unable to change array size until it's using");
}
void ecl::ArgumentBase::setMemoryType(cl_mem_flags memory_type){
    if(buffer.size() == 0){
        this->memory_type = memory_type;
    }
    else throw std::runtime_error("unable to change array memory type until it's using");
}

void ecl::ArgumentBase::clearBuffer(cl_context context){
    auto it = buffer.find(context);
    if(it != buffer.end()){
        error = clReleaseMemObject(buffer.at(context));
        buffer.erase(it);

        checkError("ArgumentBase [clear]");
    }
}

ecl::ArgumentBase::~ArgumentBase(){
    clearFields();
}

///////////////////////////////////////////////////////////////////////////////
// Variable Class Definition
///////////////////////////////////////////////////////////////////////////////
template<typename T>
void ecl::Variable<T>::clearFields(){
    ArgumentBase::clearFields();
    local_value.~T();
}

template<typename T>
ecl::Variable<T>::Variable() : local_value(), ArgumentBase(&local_value, sizeof(T), READ_WRITE){
}
template<typename T>
ecl::Variable<T>::Variable(const T& value) : ArgumentBase(&local_value, sizeof(T), READ_WRITE){
    local_value = value;
}

template<typename T>
ecl::Variable<T>::Variable(ACCESS memory_access) : ArgumentBase(&local_value, sizeof(T), memory_access){
}

template<typename T>
ecl::Variable<T>::Variable(const T& value, ACCESS memory_access) : ArgumentBase(&local_value, sizeof(T), memory_access){
    local_value = value;
}

template<typename T>
ecl::Variable<T>::Variable(const Variable<T>& other) : ArgumentBase(&local_value, other.data_size, other.memory_type){
    local_value = other.local_value;
}
template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator=(const Variable<T>& other){
    clearFields();

    local_value = other.local_value;
    data_ptr = &local_value;
    data_size = other.data_size;
    memory_type = other.memory_type;

    return *this;
}

template<typename T>
ecl::Variable<T>::Variable(Variable<T>&& other) : ArgumentBase(std::move(other)){
    local_value = other.local_value;
    data_ptr = &local_value;

    other.clearFields();
}
template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator=(Variable<T>&& other){
    clearFields();

    data_size = other.data_size;
    memory_type = other.memory_type;
    buffer = std::move(other.buffer);

    local_value = other.local_value;
    data_ptr = &local_value;

    other.clearFields();
    return *this;
}

template<typename T>
const T& ecl::Variable<T>::getValue() const{
    return local_value;
}

template<typename T>
void ecl::Variable<T>::setValue(const T& value){
    local_value = value;
}

template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator++(int){
    ++local_value;
    return *this;
}
template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator--(int){
    --local_value;
    return *this;
}

template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator=(const T& value){
    setValue(value);
    return *this;
}
template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator+=(const T& value){
    setValue(local_value + value);
    return *this;
}
template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator-=(const T& value){
    setValue(local_value - value);
    return *this;
}
template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator*=(const T& value){
    setValue(local_value * value);
    return *this;
}
template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator/=(const T& value){
    setValue(local_value / value);
    return *this;
}

template<typename T>
bool ecl::Variable<T>::operator==(const T& value){
    return local_value == value;
}
template<typename T>
bool ecl::Variable<T>::operator!=(const T& value){
    return local_value != value;
}

template<typename T>
ecl::Variable<T> ecl::Variable<T>::operator+(const T& value){
    Variable<T> result(*this);
    result += value;
    return result;
}
template<typename T>
ecl::Variable<T> ecl::Variable<T>::operator-(const T& value){
    Variable<T> result(*this);
    result -= value;
    return result;
}
template<typename T>
ecl::Variable<T> ecl::Variable<T>::operator*(const T& value){
    Variable<T> result(*this);
    result *= value;
    return result;
}
template<typename T>
ecl::Variable<T> ecl::Variable<T>::operator/(const T& value){
    Variable<T> result(*this);
    result /= value;
    return result;
}

template<typename T>
ecl::Variable<T>::operator T&(){
    return local_value;
}

template<typename T>
ecl::Variable<T>::operator const T&() const{
    return local_value;
}

template<typename T>
ecl::Variable<T>::~Variable(){
    clearFields();
}

///////////////////////////////////////////////////////////////////////////////
// Array Class Definition
///////////////////////////////////////////////////////////////////////////////
template<typename T>
void ecl::Array<T>::clearFields(){
    ArgumentBase::clearFields();
    if(control == BIND){
        delete[] static_cast<T*>(data_ptr);
        data_ptr = nullptr;
    }
}

template<typename T>
ecl::Array<T>::Array() : ArgumentBase(){
}

template<typename T>
ecl::Array<T>::Array(std::size_t array_size) : ArgumentBase(nullptr, array_size * sizeof(T), READ_WRITE){
    this->control = BIND;
    T* temp = new T[array_size];
    data_ptr = temp;
}
template<typename T>
ecl::Array<T>::Array(std::size_t array_size, ACCESS memory_access) : ArgumentBase(nullptr, array_size * sizeof(T), memory_access){
    this->control = BIND;
    T* temp = new T[array_size];
    data_ptr = temp;
}

template<typename T>
ecl::Array<T>::Array(const T* array, std::size_t array_size, CONTROL control) : ArgumentBase(static_cast<const void*>(array), array_size * sizeof(T)){
    this->control = control;
}

template<typename T>
ecl::Array<T>::Array(T* array, std::size_t array_size, ACCESS memory_access, CONTROL control) : ArgumentBase(static_cast<void*>(array), array_size * sizeof(T), memory_access) {
    this->control = control;
}

template<typename T>
ecl::Array<T>::Array(const Array<T>& other) : ArgumentBase(nullptr, other.data_size, other.memory_type){
    control = BIND;
    std::size_t count = data_size / sizeof(T);
    data_ptr = new T[count];

    std::copy(static_cast<T*>(other.data_ptr), static_cast<T*>(other.data_ptr) + count, static_cast<T*>(data_ptr));
}
template<typename T>
ecl::Array<T>& ecl::Array<T>::operator=(const Array<T>& other){
    clearFields();

    control = BIND;
    std::size_t count = data_size / sizeof(T);
    data_ptr = new T[count];

    std::copy(static_cast<T*>(other.data_ptr), static_cast<T*>(other.data_ptr) + count, static_cast<T*>(data_ptr));

    data_size = other.data_size;
    memory_type = other.memory_type;        

    return *this;
}

template<typename T>
ecl::Array<T>::Array(Array<T>&& other) : ArgumentBase(std::move(other)){
    control = other.control;
    other.control = FREE;

    other.clearFields();
}
template<typename T>
ecl::Array<T>& ecl::Array<T>::operator=(Array<T>&& other){
    clearFields();

    data_ptr = other.data_ptr;
    data_size = other.data_size;
    memory_type = other.memory_type;
    buffer = std::move(other.buffer);
    control = other.control;

    other.control = FREE;

    other.clearFields();
    return *this;
}

template<typename T>
const T* ecl::Array<T>::getConstArray() const{
    return static_cast<const T*>(data_ptr);
}

template<typename T>
T* ecl::Array<T>::getArray(){
    return static_cast<T*>(data_ptr);
}

template<typename T>
void ecl::Array<T>::setArray(const T* array, std::size_t array_size){
    clearFields();

    this->setDataPtr(array);
    this->setDataSize(array_size * sizeof(T));
    this->setMemoryType(READ);
    control = FREE;
}
template<typename T>
void ecl::Array<T>::setArray(T* array, std::size_t array_size, ACCESS memory_access){
    clearFields();

    this->setDataPtr(array);
    this->setDataSize(array_size * sizeof(T));
    this->setMemoryType(memory_access);
    control = FREE;
}

template<typename T>
T& ecl::Array<T>::operator[](std::size_t i){
    return getArray()[i];
}

template<typename T>
ecl::Array<T>::operator T*(){
    return getArray();
}

template<typename T>
ecl::Array<T>::operator const T*() const{
    return getConstArray();
}

template<typename T>
ecl::Array<T>::~Array(){
    clearFields();
}

///////////////////////////////////////////////////////////////////////////////
// Computer Class Definition
///////////////////////////////////////////////////////////////////////////////
ecl::Computer::Computer(std::size_t i, const Platform* platform, DEVICE dev){
    device = platform->getDevice(i, dev);

    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &error);
    checkError("Computer [init]");

    queue = clCreateCommandQueue(context, device, 0, &error);
    checkError("Computer [init]");
}


void ecl::Computer::send(ecl::ArgumentBase& arg, bool sync) {
	arg.createBuffer(context);

	error = clEnqueueWriteBuffer(queue, arg.getBuffer(context), CL_FALSE, 0, arg.getDataSize(), arg.getDataPtr(), 0, nullptr, nullptr);
	checkError("Computer [send data]");

    if(sync){
        error = clFinish(queue);
        checkError("Computer [send data]");
    }
}
void ecl::Computer::send(const std::vector<ArgumentBase*>& args){
    std::size_t count = args.size();
    for(std::size_t i(0); i < count; i++) send(*args[i], false);
    
	error = clFinish(queue);
    checkError("Computer [send data]");
}

void ecl::Computer::compute(Program& prog, Kernel& kern, const std::vector<const ArgumentBase*>& args, const std::vector<std::size_t>& global_work_size, const std::vector<std::size_t>& local_work_size){
    prog.checkProgram(context, device);
    cl_program prog_program = prog.getProgram(context);
    
    kern.checkKernel(prog_program);
    cl_kernel kern_kernel = kern.getKernel(prog_program);

    std::size_t count = args.size();
    for (std::size_t i(0); i < count; i++) {
        const ArgumentBase* curr = args.at(i);
        bool sended = curr->checkBuffer(context);
        if(!sended) throw std::runtime_error("argument wasn't sent to computer");

        cl_mem buf = curr->getBuffer(context);
        error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), &buf);
        checkError("Computer [compute]");
    }

    error = clEnqueueNDRangeKernel(queue, kern_kernel, global_work_size.size(), nullptr, global_work_size.data(), local_work_size.data(), 0, nullptr, nullptr);
    checkError("Computer [compute]");
    
    error = clFinish(queue);
    checkError("Computer [compute]");
}
void ecl::Computer::compute(Program& prog, Kernel& kern, const std::vector<const ArgumentBase*>& args, const std::vector<std::size_t>& global_work_size){
    prog.checkProgram(context, device);
    cl_program prog_program = prog.getProgram(context);
    
    kern.checkKernel(prog_program);
    cl_kernel kern_kernel = kern.getKernel(prog_program);

    std::size_t count = args.size();
    for (std::size_t i(0); i < count; i++) {
        const ArgumentBase* curr = args.at(i);
        bool sended = curr->checkBuffer(context);
        if(!sended) throw std::runtime_error("argument wasn't sent to computer");

        cl_mem buf = curr->getBuffer(context);
        error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), &buf);
        checkError("Computer [compute]");
    }

    error = clEnqueueNDRangeKernel(queue, kern_kernel, global_work_size.size(), nullptr, global_work_size.data(), nullptr, 0, nullptr, nullptr);
    checkError("Computer [compute]");
    
    error = clFinish(queue);
    checkError("Computer [compute]");
}

cl_device_id ecl::Computer::getDevice() const{
    return device;
}
cl_context ecl::Computer::getContext() const{
    return context;
}
cl_command_queue ecl::Computer::getQueue() const{
    return queue;
}

void ecl::Computer::receive(ArgumentBase& arg, bool sync) {
	bool sended = arg.checkBuffer(context);
	if (!sended) throw std::runtime_error("Computer [receive]: argument wasn't sent to computer");
	if (arg.getMemoryType() == READ) throw std::runtime_error("Computer [receive]: trying to receive read-only data");

	error = clEnqueueReadBuffer(queue, arg.getBuffer(context), CL_FALSE, 0, arg.getDataSize(), arg.getDataPtr(), 0, nullptr, nullptr);
	checkError("Computer [receive data]");

    if(sync){
        error = clFinish(queue);
        checkError("Computer [receive data]");
    }
}
void ecl::Computer::receive(const std::vector<ArgumentBase*>& args){
    std::size_t count = args.size();
	for (std::size_t i(0); i < count; i++) receive(*args[i], false);

    error = clFinish(queue);
    checkError("Computer [receive data]");
}

void ecl::Computer::release(ArgumentBase& arg, bool sync) {
	arg.clearBuffer(context);

    if(sync){
        error = clFinish(queue);
        checkError("Computer [clear data]");
    }
}

void ecl::Computer::release(const std::vector<ArgumentBase*>& args){
	for (auto* arg : args) release(*arg, false);
    
    error = clFinish(queue);
    checkError("Computer [clear data]");
}

void ecl::Computer::grab(ArgumentBase& arg, bool sync) {
	receive(arg, sync);
	release(arg, sync);
}

void ecl::Computer::grab(const std::vector<ArgumentBase*>& args){
    receive(args);
    release(args);
}

namespace ecl {
	Computer& operator<<(Computer& video, ArgumentBase& arg) {
		video.send(arg);
		return video;
	}

	Computer& operator>>(Computer& video, ArgumentBase& arg) {
		video.receive(arg);
		return video;
	}
}

ecl::Computer::~Computer(){
    error = clReleaseCommandQueue(queue);
    checkError("Computer [free]");
    error = clReleaseContext(context);
    checkError("Computer [free]");
}

///////////////////////////////////////////////////////////////////////////////
// Thread Class Definition
///////////////////////////////////////////////////////////////////////////////
ecl::Thread::Thread(Program& prog, Kernel& kern, const std::vector<ArgumentBase*>& args, Computer* video){
    this->video = video;
    this->args = args;

    cl_context context = video->getContext();
    cl_device_id device = video->getDevice();
    cl_command_queue queue = video->getQueue();

    prog.checkProgram(context, device);
    cl_program prog_program = prog.getProgram(context);
    
    kern.checkKernel(prog_program);
    cl_kernel kern_kernel = kern.getKernel(prog_program);

    std::size_t count = args.size();
    for (std::size_t i(0); i < count; i++) {
        ArgumentBase* curr = args.at(i);
        bool sended = curr->checkBuffer(context);
        if(!sended) throw std::runtime_error("argument wasn't sent to computer");

        cl_mem buf = curr->getBuffer(context);
        error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), &buf);
        checkError("Thread [compute]");
    }

    error = clEnqueueTask(queue, kern_kernel, 0, nullptr, &sync);
    checkError("Thread [compute]");
}
void ecl::Thread::join(){
    error = clWaitForEvents(1, &sync);
    checkError("Thread [join]");

    video->receive(args);
    readed = true;
}
ecl::Thread::~Thread(){
    clReleaseEvent(sync);
    checkError("Thread [free]");
    if(!readed) video->receive(args);
}