cmake_minimum_required(VERSION 3.7...3.13)

###############################################################################
# Copy Kernels from Source Directory to Build Directory
###############################################################################

configure_file(kernels/SIMD.cl
               kernels/SIMD.cl COPYONLY)

configure_file(kernels/single.cl
               kernels/single.cl COPYONLY)

configure_file(kernels/struct.cl
               kernels/struct.cl COPYONLY)

configure_file(kernels/vectors.cl
               kernels/vectors.cl COPYONLY)

###############################################################################
# Add Examples
###############################################################################

easycl_add_example(SIMD src/SIMD.cpp)
easycl_add_example(single src/single.cpp)
easycl_add_example(struct src/struct.cpp)
easycl_add_example(pass src/pass.cpp)
easycl_add_example(vectors src/vectors.cpp)