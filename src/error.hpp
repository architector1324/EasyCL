#include <stdexcept>

#ifndef EASYCL_ERROR
#define EASYCL_ERROR

namespace ecl{
    class Error{
        protected:
            static int error;
            static const char* getErrorString();
        public:
            static void checkError();
    };
}

#endif // EASYCL_ERROR
