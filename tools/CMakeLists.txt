cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

FOREACH(tools
    FontGen
)
ADD_SUBDIRECTORY(${tools})

SET_TARGET_ATTRIBUTE(${tools} "tools")

ENDFOREACH()