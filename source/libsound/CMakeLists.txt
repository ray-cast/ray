SET(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source/libsound)

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/libogg)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/libvorbis)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/openal/include)

OPTION(BUILD_OPENAL "on for use" ON)

SET(SOUND_SYSTEM_LIST
    ${SOURCE_PATH}/sound_buffer.cpp
    ${HEADER_PATH}/sound_buffer.h
    ${HEADER_PATH}/sound_buffer_all.h
    ${SOURCE_PATH}/sound_buffer_all.cpp
    ${HEADER_PATH}/sound_buffer_ogg.h
    ${SOURCE_PATH}/sound_buffer_ogg.cpp
    ${HEADER_PATH}/sound_device.h
    ${SOURCE_PATH}/sound_device.cpp
    ${SOURCE_PATH}/sound_listener.cpp
    ${HEADER_PATH}/sound_listener.h
    ${SOURCE_PATH}/sound_property.cpp
    ${HEADER_PATH}/sound_property.h
    ${HEADER_PATH}/sound_system.h
    ${SOURCE_PATH}/sound_system.cpp
    ${HEADER_PATH}/sound_source.h
    ${SOURCE_PATH}/sound_source.cpp
    ${HEADER_PATH}/sound_source_listener.h
    ${SOURCE_PATH}/sound_source_listener.cpp
    ${HEADER_PATH}/sound_types.h
)
SOURCE_GROUP("Common" FILES ${SOUND_SYSTEM_LIST})

IF(BUILD_OPENAL)
    ADD_DEFINITIONS(-D_BUILD_OPENAL)
    FILE(GLOB SOUND_DEVICE_LIST "OpenAL/*.*")
    SOURCE_GROUP("OpenAL" FILES ${SOUND_DEVICE_LIST})
ENDIF()

IF(BUILD_SOUND)
    ADD_LIBRARY(libsound SHARED ${SOUND_DEVICE_LIST} ${SOUND_SYSTEM_LIST})
    TARGET_LINK_LIBRARIES(libsound libplatform OpenAL32 libvorbis)
    IF(MINGW OR UNIX)
        SET_TARGET_PROPERTIES(libsound PROPERTIES OUTPUT_NAME sound)
    ENDIF()
    SET_TARGET_ATTRIBUTE(libsound "core")
ENDIF()