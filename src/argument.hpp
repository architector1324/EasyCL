#include "kernel.hpp"

#ifndef EASYCL_ARGUMENT
#define EASYCL_ARGUMENT

namespace ecl{
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
}

#endif // EASYCL_ARGUMENT
