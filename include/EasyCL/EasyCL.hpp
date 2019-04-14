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
    enum FREE{AUTO, MANUALLY};
    enum EXEC {SYNC, ASYNC};

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
        cl_platform_id platform = nullptr;
        std::string name = "";

        std::vector<cl_device_id> cpus;
        std::vector<cl_device_id> gpus;
        std::vector<cl_device_id> accs;

        void initDevices(std::vector<cl_device_id>&, cl_device_type);
        void releaseDevices(std::vector<cl_device_id>&);
    public:
        Platform() = default;
        Platform(cl_platform_id);

        cl_device_id getDevice(std::size_t, DEVICE) const;
        const std::string& getName() const;

        std::string getPlatformInfo(cl_platform_info) const;
        std::string getDeviceInfo(std::size_t, DEVICE, cl_device_info) const;

        friend std::ostream& operator<<(std::ostream&, const Platform&);
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
        static const Platform& getPlatform(std::size_t);
        static void release();
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

        static Program load(const std::string&);

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

        cl_kernel getKernel(cl_program) const;
        bool checkKernel(cl_program);

        void clearFields();
        ~Kernel();
    };

///////////////////////////////////////////////////////////////////////////////
// Buffer Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Buffer : public Error{
    protected:
        std::map<cl_context, cl_mem> buffer; // buffers map by context
        void* data_ptr = nullptr; // pointer to data
        std::size_t data_size = 0; // sizeof data
        ACCESS access = READ; // memory access

        bool ref = true;
    public:
        Buffer(void*, std::size_t, ACCESS);

        Buffer(const Buffer&);
        Buffer& operator=(const Buffer&);

        Buffer(Buffer&&);
        Buffer& operator=(Buffer&&);

        void* getDataPtr();
        std::size_t getDataSize() const;
        ACCESS getAccess() const;
        cl_mem getBuffer(cl_context) const;

        bool checkBuffer(cl_context) const;
        void createBuffer(cl_context);
        void releaseBuffer(cl_context);

        void setDataPtr(void*);
        void setDataSize(std::size_t);
        void setAccess(ACCESS);

        virtual void clearFields();

        ~Buffer();
    };

///////////////////////////////////////////////////////////////////////////////
// Variable Class Declaration
///////////////////////////////////////////////////////////////////////////////
    template<typename T> class Variable : public Buffer{
        private:
            T local_value;

        public:
            Variable();
            Variable(const T&, ACCESS access = READ_WRITE);

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
    template<typename T> class Array : public Buffer{
        private:
            FREE manage = FREE::MANUALLY;
            std::size_t size;
        public:
            Array();
            Array(std::size_t, ACCESS access = READ_WRITE);
            Array(const T*, std::size_t, FREE manage = MANUALLY);
            Array(T*, std::size_t, ACCESS, FREE manage = MANUALLY);

            Array(const Array<T>&);
            Array<T>& operator=(const Array<T>&);

            Array(Array<T>&&);
            Array<T>& operator=(Array<T>&&);

            T* getArray();
            std::size_t getSize() const;
            const T* getConstArray() const;

            T& operator[](std::size_t i);
            operator T*();
            operator const T*() const;

            void clearFields() override;
            ~Array();
    };

///////////////////////////////////////////////////////////////////////////////
// Frame Struct Declaration
///////////////////////////////////////////////////////////////////////////////
    struct Frame{
        Program& prog;
        Kernel& kern;
        const std::vector<const Buffer*>& args;
    };
///////////////////////////////////////////////////////////////////////////////
// Computer Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Computer : public Error{
        private:
            cl_device_id device = nullptr;
            std::string name = "";

            cl_context context = nullptr;
            cl_command_queue queue = nullptr;

        public:
			Computer() = delete;
            Computer(std::size_t, const Platform&, DEVICE);

			cl_device_id getDevice() const;
			cl_context getContext() const;
			cl_command_queue getQueue() const;
            const std::string& getName() const;

			void send(Buffer&, bool sync = true);
			void receive(Buffer&, bool sync = true);
			void release(Buffer&, bool sync = true);
			void grab(Buffer&, bool sync = true);

            void send(const std::vector<Buffer*>&);
			void receive(const std::vector<Buffer*>&);
			void release(const std::vector<Buffer*>&);
			void grab(const std::vector<Buffer*>&);

            void grid(const Frame&, const std::vector<std::size_t>&, const std::vector<std::size_t>&, EXEC sync = SYNC);
            void grid(const Frame&, const std::vector<std::size_t>&, EXEC sync = SYNC);
            void task(const Frame&, EXEC sync = SYNC);

            void await();

            friend std::ostream& operator<<(std::ostream&, const Computer&);
			friend Computer& operator<<(Computer&, Buffer&);
			friend Computer& operator>>(Computer&, Buffer&);

            ~Computer();
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

void ecl::Platform::releaseDevices(std::vector<cl_device_id>& devs){
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
    name = getPlatformInfo(CL_PLATFORM_NAME);
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
const std::string& ecl::Platform::getName() const{
    return name;
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

namespace ecl{
    std::ostream& operator<<(std::ostream& s, const Platform& p){
        s << p.getName();
        return s;
    }
}

ecl::Platform::~Platform(){
    releaseDevices(cpus);
    releaseDevices(gpus);
    releaseDevices(accs);
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

const ecl::Platform& ecl::System::getPlatform(std::size_t i){
    if(!initialized) init();
    return *platforms.at(i);
}

void ecl::System::release(){
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

ecl::Program ecl::Program::load(const std::string& filename){
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
// Buffer Class Definition
///////////////////////////////////////////////////////////////////////////////
void ecl::Buffer::clearFields(){
    for(const std::pair<cl_context, cl_mem>& p : buffer) releaseBuffer(p.first);
    if(!ref) delete[] static_cast<uint8_t*>(data_ptr);

    data_ptr = nullptr;
    data_size = 0;
    access = READ;
    buffer.clear();
}
ecl::Buffer::Buffer(void* data_ptr, std::size_t data_size, ACCESS access){
    this->data_ptr = data_ptr;
    this->data_size = data_size;
    this->access = access;
}

ecl::Buffer::Buffer(const Buffer& other){
    clearFields();

    data_size = other.data_size;
    access = other.access;
    data_ptr = new uint8_t[data_size];

    memcpy(data_ptr, other.data_ptr, data_size);
    ref = false;
}
ecl::Buffer& ecl::Buffer::operator=(const Buffer& other){
    clearFields();

    data_size = other.data_size;
    access = other.access;
    data_ptr = new uint8_t[data_size];

    memcpy(data_ptr, other.data_ptr, data_size);
    ref = false;

    return *this;
}

ecl::Buffer::Buffer(Buffer&& other){
    data_size = other.data_size;
    data_ptr = other.data_ptr;
    access = other.access;
    buffer = std::move(other.buffer);

    other.clearFields();
}
ecl::Buffer& ecl::Buffer::operator=(Buffer&& other){
    clearFields();

    data_size = other.data_size;
    data_ptr = other.data_ptr;
    access = other.access;
    buffer = std::move(other.buffer);

    other.clearFields();

    return *this;
}

void* ecl::Buffer::getDataPtr(){
    return data_ptr;
}
std::size_t ecl::Buffer::getDataSize() const{
    return data_size;
}
ecl::ACCESS ecl::Buffer::getAccess() const{
    return access;
}
cl_mem ecl::Buffer::getBuffer(cl_context context) const{
    return buffer.at(context);
}

bool ecl::Buffer::checkBuffer(cl_context context) const{
    if(buffer.find(context) == buffer.end()) return false;
    return true;
}
void ecl::Buffer::createBuffer(cl_context context){
    if(!checkBuffer(context)){
        buffer.emplace(context, clCreateBuffer(context, access, data_size, nullptr, &error));
        checkError("Buffer [check]");
    }
}

void ecl::Buffer::setDataPtr(void* data_ptr){
    this->data_ptr = data_ptr;
}
void ecl::Buffer::setDataSize(std::size_t data_size){
    if(buffer.size() == 0){
        this->data_size = data_size;
    }
    else throw std::runtime_error("unable to change array size until it's using");
}
void ecl::Buffer::setAccess(ACCESS access){
    if(buffer.size() == 0){
        this->access = access;
    }
    else throw std::runtime_error("unable to change array memory type until it's using");
}

void ecl::Buffer::releaseBuffer(cl_context context){
    auto it = buffer.find(context);
    if(it != buffer.end()){
        error = clReleaseMemObject(buffer.at(context));
        buffer.erase(it);

        checkError("Buffer [clear]");
    }
}

ecl::Buffer::~Buffer(){
    clearFields();
}

///////////////////////////////////////////////////////////////////////////////
// Variable Class Definition
///////////////////////////////////////////////////////////////////////////////
template<typename T>
void ecl::Variable<T>::clearFields(){
    Buffer::clearFields();
    local_value.~T();
}

template<typename T>
ecl::Variable<T>::Variable() : local_value(), Buffer(&local_value, sizeof(T), READ_WRITE){
}
template<typename T>
ecl::Variable<T>::Variable(const T& value, ACCESS access) : Buffer(&local_value, sizeof(T), access){
    local_value = value;
}

template<typename T>
ecl::Variable<T>::Variable(const Variable<T>& other) : Buffer(&local_value, other.data_size, other.access){
    local_value = other.local_value;
}
template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator=(const Variable<T>& other){
    clearFields();

    local_value = other.local_value;
    data_ptr = &local_value;
    data_size = other.data_size;
    access = other.access;

    return *this;
}

template<typename T>
ecl::Variable<T>::Variable(Variable<T>&& other) : Buffer(std::move(other)){
    local_value = other.local_value;
    data_ptr = &local_value;

    other.clearFields();
}
template<typename T>
ecl::Variable<T>& ecl::Variable<T>::operator=(Variable<T>&& other){
    clearFields();

    data_size = other.data_size;
    access = other.access;
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
    if(manage == AUTO) ref = false;
    Buffer::clearFields();
}

template<typename T>
ecl::Array<T>::Array() : Buffer(nullptr, 0, READ){
}

template<typename T>
ecl::Array<T>::Array(std::size_t array_size, ACCESS access) : Buffer(nullptr, array_size * sizeof(T), access){
    this->manage = AUTO;
    this->size = array_size;
    data_ptr = new T[array_size];
}

template<typename T>
ecl::Array<T>::Array(const T* array, std::size_t array_size, FREE manage) : Buffer((void*)array, array_size * sizeof(T), READ){
    this->manage = manage;
    this->size = array_size;
}

template<typename T>
ecl::Array<T>::Array(T* array, std::size_t array_size, ACCESS access, FREE manage) : Buffer(static_cast<void*>(array), array_size * sizeof(T), access) {
    this->manage = manage;
    this->size = array_size;
}

template<typename T>
ecl::Array<T>::Array(const Array<T>& other) : Buffer(other){
    clearFields();

    manage = AUTO;
    size = other.size;
    access = other.access;
}
template<typename T>
ecl::Array<T>& ecl::Array<T>::operator=(const Array<T>& other){
    clearFields();

    data_size = other.data_size;
    access = other.access;
    data_ptr = new uint8_t[data_size];

    memcpy(data_ptr, other.data_ptr, data_size);
    ref = false;

    manage = AUTO;
    size = other.size;
    access = other.access;    

    return *this;
}

template<typename T>
ecl::Array<T>::Array(Array<T>&& other) : Buffer(std::move(other)){
    manage = other.manage;
    size = other.size;
    other.manage = MANUALLY;

    other.clearFields();
}
template<typename T>
ecl::Array<T>& ecl::Array<T>::operator=(Array<T>&& other){
    clearFields();

    data_ptr = other.data_ptr;
    data_size = other.data_size;
    access = other.access;
    buffer = std::move(other.buffer);
    manage = other.manage;
    size = other.size;

    other.manage = MANUALLY;

    other.clearFields();
    return *this;
}

template<typename T>
const T* ecl::Array<T>::getConstArray() const{
    return static_cast<const T*>(data_ptr);
}
template<typename T>
std::size_t ecl::Array<T>::getSize() const{
    return size;
}

template<typename T>
T* ecl::Array<T>::getArray(){
    return static_cast<T*>(data_ptr);
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
ecl::Computer::Computer(std::size_t i, const Platform& platform, DEVICE dev){
    device = platform.getDevice(i, dev);
    name = platform.getDeviceInfo(i, dev, CL_DEVICE_NAME);

    context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &error);
    checkError("Computer [init]");

    queue = clCreateCommandQueue(context, device, 0, &error);
    checkError("Computer [init]");
}


void ecl::Computer::send(ecl::Buffer& arg, bool sync) {
	arg.createBuffer(context);

	error = clEnqueueWriteBuffer(queue, arg.getBuffer(context), CL_FALSE, 0, arg.getDataSize(), arg.getDataPtr(), 0, nullptr, nullptr);
	checkError("Computer [send data]");

    if(sync){
        error = clFinish(queue);
        checkError("Computer [send data]");
    }
}
void ecl::Computer::send(const std::vector<Buffer*>& args){
    std::size_t count = args.size();
    for(std::size_t i(0); i < count; i++) send(*args[i], false);
    
	error = clFinish(queue);
    checkError("Computer [send data]");
}

void ecl::Computer::grid(const Frame& frame, const std::vector<std::size_t>& global_work_size, const std::vector<std::size_t>& local_work_size, EXEC sync){
    auto& prog = frame.prog;
    auto& kern = frame.kern;
    const auto& args = frame.args;

    prog.checkProgram(context, device);
    cl_program prog_program = prog.getProgram(context);
    
    kern.checkKernel(prog_program);
    cl_kernel kern_kernel = kern.getKernel(prog_program);

    std::size_t count = args.size();
    for (std::size_t i(0); i < count; i++) {
        const Buffer* curr = args.at(i);
        bool sended = curr->checkBuffer(context);
        if(!sended) throw std::runtime_error("Computer [grid]: buffer wasn't sent to computer");

        cl_mem buf = curr->getBuffer(context);
        error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), &buf);
        checkError("Computer [grid]");
    }

    error = clEnqueueNDRangeKernel(queue, kern_kernel, global_work_size.size(), nullptr, global_work_size.data(), local_work_size.data(), 0, nullptr, nullptr);
    checkError("Computer [grid]");
    
    if(sync == SYNC) await();
}
void ecl::Computer::grid(const Frame& frame, const std::vector<std::size_t>& global_work_size, EXEC sync){
    auto& prog = frame.prog;
    auto& kern = frame.kern;
    const auto& args = frame.args;
    
    prog.checkProgram(context, device);
    cl_program prog_program = prog.getProgram(context);
    
    kern.checkKernel(prog_program);
    cl_kernel kern_kernel = kern.getKernel(prog_program);

    std::size_t count = args.size();
    for (std::size_t i(0); i < count; i++) {
        const Buffer* curr = args.at(i);
        bool sended = curr->checkBuffer(context);
        if(!sended) throw std::runtime_error("Computer [grid]: buffer wasn't sent to computer");

        cl_mem buf = curr->getBuffer(context);
        error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), &buf);
        checkError("Computer [grid]");
    }

    error = clEnqueueNDRangeKernel(queue, kern_kernel, global_work_size.size(), nullptr, global_work_size.data(), nullptr, 0, nullptr, nullptr);
    checkError("Computer [grid]");
    
    if(sync == SYNC) await();
}
void ecl::Computer::task(const Frame& frame, EXEC sync){
    auto& prog = frame.prog;
    auto& kern = frame.kern;
    const auto& args = frame.args;
    
    prog.checkProgram(context, device);
    cl_program prog_program = prog.getProgram(context);
    
    kern.checkKernel(prog_program);
    cl_kernel kern_kernel = kern.getKernel(prog_program);

    std::size_t count = args.size();
    for (std::size_t i(0); i < count; i++) {
        const Buffer* curr = args.at(i);
        bool sended = curr->checkBuffer(context);
        if(!sended) throw std::runtime_error("Computer [task]: buffer wasn't sent to computer");

        cl_mem buf = curr->getBuffer(context);
        error = clSetKernelArg(kern_kernel, i, sizeof(cl_mem), &buf);
        checkError("Computer [task]");
    }

    error = clEnqueueTask(queue, kern_kernel, 0, nullptr, nullptr);
    checkError("Computer [task]");

    if(sync == SYNC) await();
}

void ecl::Computer::await(){
    error = clFinish(queue);
    checkError("Computer [await]");
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
const std::string& ecl::Computer::getName() const{
    return name;
}

void ecl::Computer::receive(Buffer& arg, bool sync) {
	bool sended = arg.checkBuffer(context);
	if (!sended) throw std::runtime_error("Computer [receive]: buffer wasn't sent to computer");
	if (arg.getAccess() == READ) throw std::runtime_error("Computer [receive]: trying to receive read-only data");

	error = clEnqueueReadBuffer(queue, arg.getBuffer(context), CL_FALSE, 0, arg.getDataSize(), arg.getDataPtr(), 0, nullptr, nullptr);
	checkError("Computer [receive data]");

    if(sync){
        error = clFinish(queue);
        checkError("Computer [receive data]");
    }
}
void ecl::Computer::receive(const std::vector<Buffer*>& args){
    std::size_t count = args.size();
	for (std::size_t i(0); i < count; i++) receive(*args[i], false);

    error = clFinish(queue);
    checkError("Computer [receive data]");
}

void ecl::Computer::release(Buffer& arg, bool sync) {
	arg.releaseBuffer(context);

    if(sync){
        error = clFinish(queue);
        checkError("Computer [clear data]");
    }
}

void ecl::Computer::release(const std::vector<Buffer*>& args){
	for (auto* arg : args) release(*arg, false);
    
    error = clFinish(queue);
    checkError("Computer [clear data]");
}

void ecl::Computer::grab(Buffer& arg, bool sync) {
	receive(arg, sync);
	release(arg, sync);
}

void ecl::Computer::grab(const std::vector<Buffer*>& args){
    receive(args);
    release(args);
}

namespace ecl {
    std::ostream& operator<<(std::ostream& s, const Computer& video) {
		s << video.getName();
		return s;
	}

	Computer& operator<<(Computer& video, Buffer& arg) {
		video.send(arg);
		return video;
	}

	Computer& operator>>(Computer& video, Buffer& arg) {
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