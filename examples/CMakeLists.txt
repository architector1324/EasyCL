cmake_minimum_required(VERSION 3.7...3.13)

###############################################################################
# Copy Kernels from Source Directory to Build Directory
###############################################################################

configure_file(program.cl
               program.cl COPYONLY)

###############################################################################
# Add Examples
###############################################################################

easycl_add_example(buffer buffer.cpp)
#easycl_add_example(SIMD SIMD.cpp)
easycl_add_example(single single.cpp)