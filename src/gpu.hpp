#include "argument.hpp"

#ifndef EASYCL_GPU
#define EASYCL_GPU

namespace ecl{
    class GPU : public Error{
        private:
            cl_device_id* device; // указатель на привязанное устройство

            cl_context context; // opencl контекст
            cl_command_queue queue; // очередь запросов на привязанное устройство
        public:
            GPU(size_t platform_index, size_t device_index);

            void sendData(std::vector<GPUArgument*> args); // отправить данные на устройство
            // выполнить программу на устройстве
            void compute(GPUProgram* prog, GPUKernel* func, std::vector<GPUArgument*> args, std::vector<size_t> global_work_size);
            void receiveData(std::vector<GPUArgument*> args); // получить данные с устройства
            ~GPU();
    };
}

#endif // EASYCL_GPU
