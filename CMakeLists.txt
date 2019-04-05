cmake_minimum_required(VERSION 3.7...3.13)

project(EasyCL LANGUAGES CXX)

###############################################################################
# CMake Options
###############################################################################

option(EASYCL_BUILD_TESTS OFF)
option(EASYCL_BUILD_EXAMPLES OFF)

###############################################################################
# Find OpenCL Library
###############################################################################

find_package(OpenCL REQUIRED)

###############################################################################
# Add EasyCL Library
###############################################################################

add_library(EasyCL INTERFACE)
add_library(EasyCL::EasyCL ALIAS EasyCL)
target_include_directories(EasyCL INTERFACE include)
target_link_libraries(EasyCL INTERFACE OpenCL::OpenCL)

###############################################################################
# Build Tests
###############################################################################

if(EASYCL_BUILD_TESTS)
	add_library(Catch2 INTERFACE)
	target_include_directories(Catch2 INTERFACE vendor/Catch2/single_include)
	enable_testing()
	macro(easycl_add_test TESTNAME)
		add_executable(${TESTNAME} ${ARGN})
		target_link_libraries(${TESTNAME} PRIVATE EasyCL::EasyCL)
		target_link_libraries(${TESTNAME} PRIVATE Catch2)
		add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
		set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
	endmacro()
	add_subdirectory(tests)
endif()

###############################################################################
# Build Examples
###############################################################################

if(EASYCL_BUILD_EXAMPLES)
	macro(easycl_add_example EXAMPLENAME)
		add_executable(${EXAMPLENAME} ${ARGN})
		target_link_libraries(${EXAMPLENAME} PRIVATE EasyCL::EasyCL)
		set_target_properties(${EXAMPLENAME} PROPERTIES FOLDER examples)
	endmacro()
	add_subdirectory(examples)
endif()

###############################################################################
# Install EasyCL Library
###############################################################################

install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.hpp")

###############################################################################
# Package EasyCL Library
###############################################################################

set(CPACK_PACKAGE_ICON "icon.png")
set(CPACK_PACKAGE_VENDOR "Oleg Sayakhov")
set(CPACK_PACKAGE_CONTACT "olegsajaxov@yandex.ru")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "An OpenCL based lightweight c++ computing library")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

set(CPACK_RPM_PACKAGE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")

set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")

include(CPack)