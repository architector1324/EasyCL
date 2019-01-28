#include "program.hpp"

#ifndef EASYCL_KERNEL
#define EASYCL_KERNEL

namespace ecl{
    class GPUKernel: public Error{
    private:
        std::map<cl_program*, cl_kernel> kernel; // карта ядер по программам
        const char* name; // имя ядра
    public:
        GPUKernel(const char* name);

        const cl_kernel* getKernel(cl_program* program) const; // получить указатель на ядро
        void checkKernel(cl_program* program); // проверить ядро на прграмму
        ~GPUKernel();
    };
}

#endif // EASYCL_KERNEL
