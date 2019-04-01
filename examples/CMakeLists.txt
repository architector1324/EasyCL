cmake_minimum_required(VERSION 3.7...3.13)

###############################################################################
# Copy Kernels from Source Directory to Build Directory
###############################################################################

configure_file(kernels/SIMD.cl
               kernels/SIMD.cl COPYONLY)

configure_file(kernels/SingleThread.cl
               kernels/SingleThread.cl COPYONLY)

configure_file(kernels/Struct.cl
               kernels/Struct.cl COPYONLY)

###############################################################################
# Add Examples
###############################################################################

easycl_add_example(ExampleSIMD src/ExampleSIMD.cpp)
easycl_add_example(ExampleSingleThread src/ExampleSingleThread.cpp)
easycl_add_example(ExampleStruct src/ExampleStruct.cpp)