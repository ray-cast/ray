PROJECT(libmodel)

SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/source/libmodel)

SET(COMMON_LSIT
    ${HEADER_PATH}/modcfg.h
    ${HEADER_PATH}/moddef.h
    ${HEADER_PATH}/model.h
    ${HEADER_PATH}/modhelp.h
    ${HEADER_PATH}/modtypes.h
    ${HEADER_PATH}/modutil.h
    ${HEADER_PATH}/anim.h
    ${HEADER_PATH}/bone.h
)
SOURCE_GROUP("model" FILES ${COMMON_LSIT})

FILE(GLOB SOURCE_LIST "*.cpp")
SOURCE_GROUP("model" FILES ${SOURCE_LIST})

ADD_LIBRARY(libmodel SHARED ${COMMON_LSIT} ${SOURCE_LIST})

TARGET_LINK_LIBRARIES(libmodel PUBLIC libplatform)

IF(MINGW)
    FIND_LIBRARY(ICONV_FRAMEWORK iconv)
    IF(ICONV_FRAMEWORK MATCHES "iconv")
        TARGET_LINK_LIBRARIES(libmodel PRIVATE iconv)
    ELSE()
        TARGET_LINK_LIBRARIES(libmodel PRIVATE libiconv)
    ENDIF()
ELSE()
    TARGET_LINK_LIBRARIES(libmodel PRIVATE libiconv)
ENDIF()

SET_TARGET_ATTRIBUTE(libmodel "core")