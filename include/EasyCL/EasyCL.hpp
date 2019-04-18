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

		void copy(const Program&);
		void move(Program&);
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

        void clear();
        ~Program();
    };

///////////////////////////////////////////////////////////////////////////////
// Kernel Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Kernel: public Error{
    private:
        std::map<cl_program, cl_kernel> kernel; // карта ядер по программам
        std::string name;

		void copy(const Kernel&);
		void move(Kernel&);
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

        void clear();
        ~Kernel();
    };

///////////////////////////////////////////////////////////////////////////////
// Buffer Class Declaration
///////////////////////////////////////////////////////////////////////////////
    class Buffer : public Error{
    protected:
        std::map<cl_context, cl_mem> buffer; // buffers map by context
        void* ptr = nullptr; // pointer to data
        std::size_t size = 0; // sizeof data
        ACCESS access = READ; // memory access

		void copy(const Buffer&);
		void move(Buffer&);
    public:
        Buffer(void*, std::size_t, ACCESS);

		Buffer(const Buffer&);
		Buffer& operator=(const Buffer&);

		Buffer(Buffer&&);
		Buffer& operator=(Buffer&&);

		cl_mem getBuffer(cl_context) const;
		virtual void* getPtr();
		std::size_t getSize() const;
		ACCESS getAccess() const;

		void setPtr(void*);

		bool checkBuffer(cl_context) const;
		void createBuffer(cl_context);
		void releaseBuffer(cl_context);

		void clear();

		~Buffer();
    };

///////////////////////////////////////////////////////////////////////////////
// var Container Declaration
///////////////////////////////////////////////////////////////////////////////
template<typename T>
class var : public Buffer {
private:
	T value;

	void copy(const var<T>&);
	void move(var<T>&);
public:
	var();
	var(const T&);
	var(ACCESS);
	var(const T&, ACCESS);

	var(const var<T>&);
	var<T>& operator=(const var<T>&);

	var(var<T>&&);
	var<T>& operator=(var<T>&&);

	const T& getValue() const;
	void* getPtr() override;

	void setValue(const T&);

	var<T>& operator++(int);
	var<T>& operator--(int);
	var<T>& operator=(const T&);
	var<T>& operator+=(const T&);
	var<T>& operator-=(const T&);
	var<T>& operator*=(const T&);
	var<T>& operator/=(const T&);
	var<T> operator+(const T&);
	var<T> operator-(const T&);
	var<T> operator*(const T&);
	var<T> operator/(const T&);

	operator T&();
	operator const T&() const;

	void clear();

	~var();
};

///////////////////////////////////////////////////////////////////////////////
// array Class Declaration
///////////////////////////////////////////////////////////////////////////////
template<typename T>
class array : public Buffer {
private:
	T* arr = nullptr;
	std::size_t arr_size = 0;
	FREE manage = MANUALLY;

	void copy(const array<T>&);
	void move(array<T>&);
public:
	array();
	explicit array(std::size_t, ACCESS access = READ_WRITE);
	array(const T*, std::size_t, FREE manage = MANUALLY);
	array(T*, std::size_t, ACCESS, FREE manage = MANUALLY);

	array(const array<T>&);
	array<T>& operator=(const array<T>&);

	array(array<T>&&);
	array<T>& operator=(array<T>&&);

	T* getArray();
	const T* getConstArray() const;
	void* getPtr() override;
	std::size_t getArraySize() const;

	T& operator[](std::size_t);
	operator T*();
	operator const T*() const;

	void clear();
	~array();
};

///////////////////////////////////////////////////////////////////////////////
// Frame Struct Declaration
///////////////////////////////////////////////////////////////////////////////
    struct Frame{
        Program& prog;
        Kernel& kern;
        const std::vector<const Buffer*> args;
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

			void send(Buffer&, EXEC sync = SYNC);
			void receive(Buffer&, EXEC sync = SYNC);
			void release(Buffer&, EXEC sync = SYNC);
			void grab(Buffer&, EXEC sync = SYNC);

            void send(const std::vector<Buffer*>&, EXEC sync = SYNC);
			void receive(const std::vector<Buffer*>&, EXEC sync = SYNC);
			void release(const std::vector<Buffer*>&, EXEC sync = SYNC);
			void grab(const std::vector<Buffer*>&, EXEC sync = SYNC);

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

void ecl::Program::copy(const Program& other) {
	clear();
	source = other.source;
}
void ecl::Program::move(Program& other) {
	clear();
	source = other.source;
	program = std::move(other.program);

	other.clear();
}

void ecl::Program::clear(){
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
	copy(other);
}
ecl::Program& ecl::Program::operator=(const Program& other){
	copy(other);

    return *this;
}

ecl::Program::Program(Program&& other){
	move(other);
}
ecl::Program& ecl::Program::operator=(Program&& other){
	move(other);

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
    clear();
}

///////////////////////////////////////////////////////////////////////////////
// Kernel Class Definition
///////////////////////////////////////////////////////////////////////////////
void ecl::Kernel::copy(const Kernel& other) {
	clear();
	name = other.name;
}
void ecl::Kernel::move(Kernel& other) {
	clear();

	name = other.name;
	kernel = std::move(other.kernel);

	other.clear();
}

void ecl::Kernel::clear(){
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
	copy(other);
}
ecl::Kernel& ecl::Kernel::operator=(const Kernel& other){
	copy(other);

    return *this;
}

ecl::Kernel::Kernel(Kernel&& other){
	move(other);
}
ecl::Kernel& ecl::Kernel::operator=(Kernel&& other){
	move(other);

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
    clear();
}

///////////////////////////////////////////////////////////////////////////////
// Buffer Class Definition
///////////////////////////////////////////////////////////////////////////////
void ecl::Buffer::copy(const Buffer& other) {
	clear();

	ptr = other.ptr;
	size = other.size;
	access = other.access;

	for (auto& p : other.buffer) createBuffer(p.first);
}
void ecl::Buffer::move(Buffer& other) {
	clear();

	ptr = other.ptr;
	size = other.size;
	access = other.access;
	buffer = std::move(other.buffer);

	other.clear();
}

ecl::Buffer::Buffer(void* ptr, std::size_t size, ACCESS access) {
	this->ptr = ptr;
	this->size = size;
	this->access = access;
}

ecl::Buffer::Buffer(const Buffer& other) {
	copy(other);
}
ecl::Buffer& ecl::Buffer::operator=(const Buffer& other) {
	copy(other);

	return *this;
}

ecl::Buffer::Buffer(Buffer&& other) {
	move(other);
}
ecl::Buffer& ecl::Buffer::operator=(Buffer&& other) {
	move(other);

	return *this;
}

cl_mem ecl::Buffer::getBuffer(cl_context context) const{
	return buffer.at(context);
}
void* ecl::Buffer::getPtr() {
	return ptr;
}
std::size_t ecl::Buffer::getSize() const {
	return size;
}
ecl::ACCESS ecl::Buffer::getAccess() const {
	return access;
}

void ecl::Buffer::setPtr(void* ptr) {
	this->ptr = ptr;
}

bool ecl::Buffer::checkBuffer(cl_context context) const {
	if (buffer.find(context) != buffer.end()) return true;
	return false;
}
void ecl::Buffer::createBuffer(cl_context context) {
	if (!checkBuffer(context)) {
		buffer.emplace(context, clCreateBuffer(context, access, size, nullptr, &error));
		checkError("Buffer [check]");
	}
}
void ecl::Buffer::releaseBuffer(cl_context context) {
	auto it = buffer.find(context);
	if (it != buffer.end()) {
		error = clReleaseMemObject(buffer.at(context));
		buffer.erase(it);

		checkError("Buffer [clear]");
	}
}

void ecl::Buffer::clear() {
	while(buffer.size() > 0) releaseBuffer(buffer.begin()->first);
	ptr = nullptr;
	size = 0;
	access = READ;
}

ecl::Buffer::~Buffer() {
	clear();
}

///////////////////////////////////////////////////////////////////////////////
// var Container Definition
///////////////////////////////////////////////////////////////////////////////
template<typename T>
void ecl::var<T>::copy(const var<T>& other) {
	clear();

	Buffer::copy(other);
	value = other.value;
	setPtr(&value);
}
template<typename T>
void ecl::var<T>::move(var<T>& other) {
	clear();

	Buffer::move(other);
	value = other.value;
	setPtr(&value);

	other.clear();
}

template<typename T>
ecl::var<T>::var() : Buffer(nullptr, sizeof(T), ACCESS::READ_WRITE) {
	value = T();
}
template<typename T>
ecl::var<T>::var(const T& value) : var(){
	this->value = value;
	setPtr(&this->value);
}
template<typename T>
ecl::var<T>::var(ACCESS access) : Buffer(nullptr, sizeof(T), access) {
	value = T();
}
template<typename T>
ecl::var<T>::var(const T& value, ACCESS access) : var(access) {
	this->value = value;
	setPtr(&this->value);
}

template<typename T>
ecl::var<T>::var(const var<T>& other) : Buffer(nullptr, 0, READ_WRITE) {
	copy(other);
}
template<typename T>
ecl::var<T>& ecl::var<T>::operator=(const var<T>& other) {
	copy(other);

	return *this;
}

template<typename T>
ecl::var<T>::var(var<T>&& other) : Buffer(nullptr, 0, READ_WRITE){
	move(other);
}
template<typename T>
ecl::var<T>& ecl::var<T>::operator=(var<T>&& other) {
	move(other);

	return *this;
}


template<typename T>
const T& ecl::var<T>::getValue() const {
	return value;
}
template<typename T>
void* ecl::var<T>::getPtr() {
	return &value;
}

template<typename T>
void ecl::var<T>::setValue(const T& value) {
	this->value = value;
}

template<typename T>
ecl::var<T>& ecl::var<T>::operator++(int) {
	setValue(value + 1);
	return *this;
}
template<typename T>
ecl::var<T>& ecl::var<T>::operator--(int) {
	setValue(value - 1);
	return *this;
}
template<typename T>
ecl::var<T>& ecl::var<T>::operator=(const T& v) {
	setValue(v);
	return *this;
}
template<typename T>
ecl::var<T>& ecl::var<T>::operator+=(const T& v) {
	setValue(value + v);
	return *this;
}
template<typename T>
ecl::var<T>& ecl::var<T>::operator-=(const T& v) {
	setValue(value - v);
	return *this;
}
template<typename T>
ecl::var<T>& ecl::var<T>::operator*=(const T& v) {
	setValue(value * v);
	return *this;
}
template<typename T>
ecl::var<T>& ecl::var<T>::operator/=(const T& v) {
	setValue(value / v);
	return *this;
}

template<typename T>
ecl::var<T> ecl::var<T>::operator+(const T& v) {
	var<T> result = value + v;
	return result;
}
template<typename T>
ecl::var<T> ecl::var<T>::operator-(const T& v) {
	var<T> result = value - v;
	return result;
}
template<typename T>
ecl::var<T> ecl::var<T>::operator*(const T& v) {
	var<T> result = value * v;
	return result;
}
template<typename T>
ecl::var<T> ecl::var<T>::operator/(const T& v) {
	var<T> result = value / v;
	return result;
}

template<typename T>
ecl::var<T>::operator T&() {
	return value;
}
template<typename T>
ecl::var<T>::operator const T&() const {
	return value;
}

template<typename T>
void ecl::var<T>::clear() {
	Buffer::clear();
	value.~T();
}

template<typename T>
ecl::var<T>::~var() {
	clear();
}

///////////////////////////////////////////////////////////////////////////////
// array Container Definition
///////////////////////////////////////////////////////////////////////////////
template<typename T>
void ecl::array<T>::copy(const array<T>& other) {
	clear();

	Buffer::copy(other);

	arr_size = other.arr_size;
	arr = new T[arr_size];
	std::copy(arr, arr + arr_size, other.arr);

	setPtr(arr);
	manage = AUTO;
}
template<typename T>
void ecl::array<T>::move(array<T>& other) {
	clear();

	Buffer::move(other);
	arr = other.arr;
	arr_size = other.arr_size;
	setPtr(arr);
	manage = other.manage;

	other.clear();
}

template<typename T>
ecl::array<T>::array() : Buffer(nullptr, 0, READ_WRITE) {
	arr = nullptr;
	arr_size = 0;
	manage = MANUALLY;
}
template<typename T>
ecl::array<T>::array(std::size_t size, ACCESS access) : Buffer(nullptr, size * sizeof(T), access) {
	arr = new T[size];
	setPtr(arr);
	arr_size = size;
	manage = AUTO;
}
template<typename T>
ecl::array<T>::array(const T* arr, std::size_t size, FREE manage = MANUALLY) : Buffer(nullptr, size * sizeof(T), READ){
	this->arr = const_cast<T*>(arr);
	setPtr(this->arr);
	arr_size = size;
	this->manage = manage;
}
template<typename T>
ecl::array<T>::array(T* arr, std::size_t size, ACCESS access, FREE manage = MANUALLY) : Buffer(nullptr, size * sizeof(T), access) {
	this->arr = arr;
	setPtr(this->arr);
	arr_size = size;
	this->manage = manage;
}

template<typename T>
ecl::array<T>::array(const array<T>& other) : Buffer(nullptr, 0, READ_WRITE) {
	copy(other);
}
template<typename T>
ecl::array<T>& ecl::array<T>::operator=(const array<T>& other) {
	copy(other);
	return *this;
}

template<typename T>
ecl::array<T>::array(array<T>&& other) : Buffer(nullptr, 0, READ_WRITE) {
	move(other);
}
template<typename T>
ecl::array<T>& ecl::array<T>::operator=(array<T>&& other) {
	move(other);
	return *this;
}

template<typename T>
T* ecl::array<T>::getArray() {
	return arr;
}
template<typename T>
const T* ecl::array<T>::getConstArray() const {
	return arr;
}
template<typename T>
void* ecl::array<T>::getPtr() {
	return arr;
}
template<typename T>
std::size_t ecl::array<T>::getArraySize() const {
	return arr_size;
}

template<typename T>
T& ecl::array<T>::operator[](std::size_t index) {
	return arr[index];
}
template<typename T>
ecl::array<T>::operator T*() {
	return arr;
}
template<typename T>
ecl::array<T>::operator const T*() const {
	return arr;
}

template<typename T>
void ecl::array<T>::clear() {
	Buffer::clear();
	if (manage == AUTO) delete[] arr;

	arr = nullptr;
	arr_size = 0;
	manage = MANUALLY;
}
template<typename T>
ecl::array<T>::~array() {
	clear();
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

void ecl::Computer::send(ecl::Buffer& arg, EXEC sync) {
	arg.createBuffer(context);

	error = clEnqueueWriteBuffer(queue, arg.getBuffer(context), CL_FALSE, 0, arg.getSize(), arg.getPtr(), 0, nullptr, nullptr);
	checkError("Computer [send data]");

    if(sync == SYNC) await();
}
void ecl::Computer::send(const std::vector<Buffer*>& args, EXEC sync){
    std::size_t count = args.size();
    for(std::size_t i(0); i < count; i++) send(*args[i], ASYNC);
    
	if(sync == SYNC) await();
}
void ecl::Computer::receive(Buffer& arg, EXEC sync) {
	bool sended = arg.checkBuffer(context);
	if (!sended) throw std::runtime_error("Computer [receive]: buffer wasn't sent to computer");
	if (arg.getAccess() == READ) throw std::runtime_error("Computer [receive]: trying to receive read-only data");

	error = clEnqueueReadBuffer(queue, arg.getBuffer(context), CL_FALSE, 0, arg.getSize(), arg.getPtr(), 0, nullptr, nullptr);
	checkError("Computer [receive data]");

    if(sync == SYNC) await();
}
void ecl::Computer::receive(const std::vector<Buffer*>& args, EXEC sync){
    std::size_t count = args.size();
	for (std::size_t i(0); i < count; i++) receive(*args[i], ASYNC);

    if(sync == SYNC) await();
}
void ecl::Computer::release(Buffer& arg, EXEC sync) {
	arg.releaseBuffer(context);

    if(sync == SYNC) await();
}
void ecl::Computer::release(const std::vector<Buffer*>& args, EXEC sync){
	for (auto* arg : args) release(*arg, ASYNC);
    
    if(sync == SYNC) await();
}
void ecl::Computer::grab(Buffer& arg, EXEC sync) {
	receive(arg, sync);
	release(arg, sync);
}
void ecl::Computer::grab(const std::vector<Buffer*>& args, EXEC sync){
    receive(args);
    release(args, sync);
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