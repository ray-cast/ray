cmake_minimum_required(VERSION 2.8)

INCLUDE_DIRECTORIES(${INCLUDE_PATH})

FOREACH(samples
    cubes
)
ADD_SUBDIRECTORY(${samples})

SET_TARGET_ATTRIBUTE(${samples} "samples")

ENDFOREACH()