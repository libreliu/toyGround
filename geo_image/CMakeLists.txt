cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# +----------------------------------------------------+
# |  geo_image : the toyGround for Geometry Image      |
# +----------------------------------------------------+

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (geo_image_SRCS
    main.cpp
    impl.cpp
)

add_executable(geo_image ${geo_image_SRCS})

target_link_libraries(geo_image CONAN_PKG::UHEMesh CONAN_PKG::tinygltf)