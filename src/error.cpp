#include "error.hpp"

int ecl::Error::error = 0;

void ecl::Error::checkError(){
    if (error != 0) throw std::runtime_error(getErrorString());
}

const char* ecl::Error::getErrorString()
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