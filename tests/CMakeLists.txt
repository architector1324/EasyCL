cmake_minimum_required(VERSION 3.7...3.13)

###############################################################################
# Add Tests
###############################################################################

easycl_add_test(Buffer src/Buffer.cpp)
easycl_add_test(Array src/Array.cpp)
easycl_add_test(Computer src/Computer.cpp)
easycl_add_test(Error src/Error.cpp)
easycl_add_test(Kernel src/Kernel.cpp)
easycl_add_test(Platform src/Platform.cpp)
easycl_add_test(Program src/Program.cpp)
easycl_add_test(System src/System.cpp)
easycl_add_test(Thread src/Thread.cpp)
easycl_add_test(Variable src/Variable.cpp)