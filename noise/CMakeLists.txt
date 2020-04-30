cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# +----------------------------------------------------+
# |  noise : the noise library                         |
# +----------------------------------------------------+

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#add_definitions(-DWORLEY_DEBUG)

# Todo: recursively add all header files in api/noise
set (noise_SRCS 
    generators/PerlinNoiseGenerator2D.cpp
    generators/WorleyNoiseGenerator.cpp
    stb_impl/stb_image_impl.cpp
    stb_impl/stb_image_write_impl.cpp
)

add_library(noise STATIC ${noise_SRCS})

target_include_directories(noise PUBLIC ${EXTERNAL_HEADER_DIR})
target_include_directories(noise PUBLIC ${PROJECT_HEADER_DIR}/noise)

# +----------------------------------------------------+
# |  noise_test - examples for using the library (CLI) |
# +----------------------------------------------------+

set (noise_test_SRCS 
    noise_test.cpp
)

add_executable(noise_test ${noise_test_SRCS})
target_link_libraries(noise_test noise)