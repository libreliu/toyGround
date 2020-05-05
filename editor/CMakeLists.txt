cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

# +----------------------------------------------------+
# |  editor : the editor for toyGround                 |
# +----------------------------------------------------+

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set (editor_SRCS 
    Editor.cpp
    formbuilder_ui/EditorInst.cpp
    EditorCallbackImpl.cpp
    NoisePanel.cpp
    NoiseWorker.cpp
)

message("*NOTICE*: The vcpkg + wxWidgets + CMake is currently a crap!")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message("Enabling __WXDEBUG__ macro, making all wxLogDebug useful..")
    add_definitions(-D__WXDEBUG__)
endif()

find_package(wxWidgets REQUIRED COMPONENTS core base propgrid adv)
include(${wxWidgets_USE_FILE})

# Fix error LNK2019: unresolved external symbol main
# Since wxIMPLEMENT_APP instantiates WinMain instead of main
if(WIN32)
    add_executable(editor WIN32 ${editor_SRCS})
else()
    add_executable(editor ${editor_SRCS})
endif()

target_link_libraries(editor noise ${wxWidgets_LIBRARIES})

# Todo: also add header files into dependency
target_include_directories(editor PRIVATE ${PROJECT_HEADER_DIR}/editor)
target_include_directories(editor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/formbuilder_ui)