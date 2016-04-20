cmake_minimum_required(VERSION 2.8)

include_directories(include)
include_directories(src/mesa)
include_directories(src/mapi)
include_directories(src/glsl)
INCLUDE_DIRECTORIES(SRC)

FILE(GLOB glcpp-library_sources src/glsl/glcpp/*.c src/util/*.c)
ADD_LIBRARY(glcpp-library ${glcpp-library_sources})

FILE(GLOB mesa_sources src/mesa/program/*.c src/mesa/main/*.c)
ADD_LIBRARY(mesa ${mesa_sources})

FILE(GLOB glsl_sources src/glsl/*.cpp src/glsl/*.c)
FILE(GLOB glsl_sources_remove src/glsl/main.cpp src/glsl/builtin_stubs.cpp)
LIST(REMOVE_ITEM glsl_sources ${glsl_sources_remove})
ADD_LIBRARY(glsl_optimizer ${glsl_sources})
TARGET_LINK_LIBRARIES(glsl_optimizer glcpp-library mesa)