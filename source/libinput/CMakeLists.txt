SET(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source/libinput)

SET(INPUT_CORE_LIST
    ${HEADER_PATH}/input_button.h
    ${SOURCE_PATH}/input_controller.cpp
    ${HEADER_PATH}/input_controller.h
    ${HEADER_PATH}/input_device.h
    ${SOURCE_PATH}/input_device.cpp
    ${SOURCE_PATH}/input_device_base.cpp
    ${HEADER_PATH}/input_device_base.h
    ${HEADER_PATH}/input_event.h
    ${SOURCE_PATH}/input_event.cpp
    ${HEADER_PATH}/input_key.h
    ${HEADER_PATH}/input_keyboard.h
    ${SOURCE_PATH}/input_keyboard.cpp
    ${HEADER_PATH}/input_keyboard_base.h
    ${SOURCE_PATH}/input_keyboard_base.cpp
    ${SOURCE_PATH}/input_mouse.cpp
    ${HEADER_PATH}/input_mouse.h
    ${HEADER_PATH}/input_mouse_base.h
    ${SOURCE_PATH}/input_mouse_base.cpp
    ${HEADER_PATH}/input.h
    ${SOURCE_PATH}/input.cpp
    ${HEADER_PATH}/input_base.h
    ${SOURCE_PATH}/input_base.cpp
    ${HEADER_PATH}/input_types.h
    ${HEADER_PATH}/input_ps3.h
    ${HEADER_PATH}/input_xbox.h
)
SOURCE_GROUP("input\\core" FILES ${INPUT_CORE_LIST})

SET(INPUT_MSW_LIST
    ${HEADER_PATH}/msw_input_device.h
    ${SOURCE_PATH}/msw_input_device.cpp
    ${HEADER_PATH}/msw_input_keyboard.h
    ${SOURCE_PATH}/msw_input_keyboard.cpp
    ${HEADER_PATH}/msw_input_mouse.h
    ${SOURCE_PATH}/msw_input_mouse.cpp
)
SOURCE_GROUP("input\\msw" FILES ${INPUT_MSW_LIST})

SET(INPUT_NDK_LIST
    ${HEADER_PATH}/ndk_input_device.h
    ${SOURCE_PATH}/ndk_input_device.cpp
    ${HEADER_PATH}/ndk_input_keyboard.h
    ${SOURCE_PATH}/ndk_input_keyboard.cpp
    ${HEADER_PATH}/ndk_input_mouse.h
    ${SOURCE_PATH}/ndk_input_mouse.cpp
)
SOURCE_GROUP("input\\ndk" FILES ${INPUT_NDK_LIST})

ADD_LIBRARY(libinput SHARED ${INPUT_CORE_LIST} ${INPUT_MSW_LIST})

TARGET_LINK_LIBRARIES(libinput libplatform)
IF(BUILD_PLATFORM_WINDOWS)
    TARGET_LINK_LIBRARIES(libinput winmm)
    TARGET_LINK_LIBRARIES(libinput imm32)
ENDIF()

IF(MINGW OR UNIX)
    SET_TARGET_PROPERTIES(libinput PROPERTIES OUTPUT_NAME input)
ENDIF()

SET_TARGET_ATTRIBUTE(libinput "core")