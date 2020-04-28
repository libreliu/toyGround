cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(noise)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#add_definitions(-DWORLEY_DEBUG)
add_executable(noise perlin_noise.cpp ${HEADER_DIR}/vec.h)

target_include_directories(noise PRIVATE ${HEADER_DIR})
