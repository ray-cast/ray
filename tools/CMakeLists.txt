cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${INCLUDE_PATH})

FOREACH(tools
    FontGen
)
ADD_SUBDIRECTORY(${tools})

SET_TARGET_ATTRIBUTE(${tools} "tools")

ENDFOREACH()