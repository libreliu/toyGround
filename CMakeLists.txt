cmake_minimum_required(VERSION 3.0.0)
project(toyGround VERSION 0.1.0)

# Test if user have done "conan install"
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
    message("conanbuildinfo.cmake did not found!")
    exec_program("conan" "${CMAKE_BINARY_DIR}"
        ARGS install "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE conan_output
        RETURN_VALUE conan_ret)
    message("Conan output: ${conan_output}")
    if (conan_ret EQUAL 0)
        message("Conan install successful.")
    else()
        message(FATAL_ERROR "Conan install failed! Make sure you have conan installed. "
        "Sometimes you may need to use conan install . with --build=missing option to manually build the stuff.")
    endif()
else()
    message("conanbuildinfo.cmake found!")
endif()

# Time to inject conan!
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# External headers are not meant to be changed, so they don't 
# appear as targets in build system as well
set(EXTERNAL_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# But project headers do.
set(PROJECT_HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/api)

add_subdirectory(noise)
add_subdirectory(editor)
add_subdirectory(vulkan)