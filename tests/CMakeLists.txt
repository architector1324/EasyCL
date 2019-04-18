cmake_minimum_required(VERSION 3.7...3.13)

###############################################################################
# Add Tests
###############################################################################

easycl_add_test(Buffer Buffer.cpp)
easycl_add_test(Array Array.cpp)
easycl_add_test(Computer Computer.cpp)
easycl_add_test(Error Error.cpp)
easycl_add_test(Kernel Kernel.cpp)
easycl_add_test(Platform Platform.cpp)
easycl_add_test(Program Program.cpp)
easycl_add_test(System System.cpp)
easycl_add_test(var var.cpp)