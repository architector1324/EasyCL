#include <stdexcept>

namespace ecl{
    class Error{
        protected:
            static int error;
            static const char* getErrorString();
        public:
            static void checkError();
    };
}