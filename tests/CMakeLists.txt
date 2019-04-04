cmake_minimum_required(VERSION 3.7...3.13)

###############################################################################
# Add Tests
###############################################################################

easycl_add_test(TestArgumentBase src/TestArgumentBase.cpp)
easycl_add_test(TestArray src/TestArray.cpp)
easycl_add_test(TestComputer src/TestComputer.cpp)
easycl_add_test(TestError src/TestError.cpp)
easycl_add_test(TestKernel src/TestKernel.cpp)
easycl_add_test(TestPlatform src/TestPlatform.cpp)
easycl_add_test(TestProgram src/TestProgram.cpp)
easycl_add_test(TestSystem src/TestSystem.cpp)
easycl_add_test(TestThread src/TestThread.cpp)
easycl_add_test(TestVariable src/TestVariable.cpp)