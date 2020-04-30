cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# +----------------------------------------------------+
# |  editor : the editor for toyGround                 |
# +----------------------------------------------------+

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (editor_SRCS 
    main.cpp
)

message("*NOTICE*: The vcpkg + wxWidgets + CMake is currently a crap!")

find_package(wxWidgets REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

add_executable(editor ${editor_SRCS})
target_link_libraries(editor noise ${wxWidgets_LIBRARIES})