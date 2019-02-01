#include <map>
#include <cstring>
#include <fstream>
#include "platform.hpp"

#ifndef EASYCL_PROGRAM
#define EASYCL_PROGRAM

namespace ecl{
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

        void checkProgram(cl_context* context, cl_device_id* device); // проверить программу на контекст

        ~GPUProgram();
    };
}

#endif // EASYCL_PLATFORM
