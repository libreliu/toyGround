cmake_minimum_required(VERSION 3.0.0)
project(toyGround VERSION 0.1.0)


# External headers are not meant to be changed, so they don't 
# appear as targets in build system as well
set(EXTERNAL_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# But project headers do.
set(PROJECT_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/api)

add_subdirectory(noise)
add_subdirectory(editor)