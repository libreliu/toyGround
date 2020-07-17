cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# +----------------------------------------------------+
# |  vulkan : the toyGround for vulkan                 |
# +----------------------------------------------------+

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (vulkan_SRCS
    main.cpp
)

add_executable(vulkan ${vulkan_SRCS})

target_link_libraries(vulkan CONAN_PKG::glfw)

# Vulkan
# Define the VULKAN_SDK environment variable in order to build
# - It's default for Vulkan LunarG build, TODO: Linux
find_package(Vulkan REQUIRED)
target_link_libraries(vulkan Vulkan::Vulkan)

# Todo: also add header files into dependency
target_include_directories(vulkan PRIVATE ${PROJECT_HEADER_DIR}/vulkan)