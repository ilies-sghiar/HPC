cmake_minimum_required(VERSION 2.6)
# set( CMAKE_VERBOSE_MAKEFILE on )

project(imageFilter)

set(CMAKE_CXX_STANDARD 14)

# declare executable file and source files
add_executable(
        imageCopyFilter
        imageCopyFilter.cpp
	utils.cpp
)

add_compile_options(Wno-deprecated-declarations)
add_definitions(-DCL_TARGET_OPENCL_VERSION=120)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L/usr/local/cuda-10.1/targets/x86_64-linux/lib/")

target_link_libraries(
  imageCopyFilter
  OpenCL
  m
  freeimage
)
