# Copyright (C) The MooGL Project
#
# The OpenGLESDemo samples on macOS, on MooGL's OpenGL ES contexts.
#
#   cmake -B build -DCMAKE_PREFIX_PATH=/path/to/MooGL-SDK && cmake --build build
#
# or against a MooGL build tree:
#
#   cmake -B build -DMOOGL_BUILD_DIR=/path/to/MGL/build -DMOOGL_GLFW_INCLUDE=/path/to/glfw/include
cmake_minimum_required(VERSION 3.16)
project(OpenGLESDemo CXX C)

set(CMAKE_CXX_STANDARD 17)
set(MAIN ${CMAKE_CURRENT_SOURCE_DIR}/../app/src/main)
set(CPP ${MAIN}/cpp)

file(GLOB_RECURSE SOURCES ${CPP}/sample/*.cpp ${CPP}/utils/*.cpp)

# Android only, or needing assimp, FreeType or OpenCV
list(FILTER SOURCES EXCLUDE REGEX "SharedEGLContextSample|TextRenderSample|Model3DSample|PortraitStayColorExample|lesson7|lesson8")

file(GLOB_RECURSE HEADERS ${CPP}/sample/*.h ${CPP}/utils/*.h)
set(INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CPP}/include ${CPP}/inc)
foreach(h ${HEADERS})
    get_filename_component(d ${h} DIRECTORY)
    list(APPEND INCLUDES ${d})
endforeach()
list(REMOVE_DUPLICATES INCLUDES)

add_executable(OpenGLESDemo main.cpp ${SOURCES})
target_include_directories(OpenGLESDemo PRIVATE ${INCLUDES})
target_compile_definitions(OpenGLESDemo PRIVATE OPENGLESDEMO_MAIN_DIR="${MAIN}" GL_GLEXT_PROTOTYPES)
# the samples are someone else's code; their warnings are theirs
target_compile_options(OpenGLESDemo PRIVATE -w)

if(MOOGL_BUILD_DIR)
    target_include_directories(OpenGLESDemo PRIVATE ${MOOGL_GLFW_INCLUDE})
    target_link_libraries(OpenGLESDemo PRIVATE ${MOOGL_BUILD_DIR}/libglfw.dylib ${MOOGL_BUILD_DIR}/libmoogl.dylib)
    set_target_properties(OpenGLESDemo PROPERTIES BUILD_RPATH ${MOOGL_BUILD_DIR})
else()
    find_package(MooGL REQUIRED)
    target_link_libraries(OpenGLESDemo PRIVATE MooGL::glfw MooGL::GL)
endif()
