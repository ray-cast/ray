SET(LIB_NAME libiconv)

INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(srclib)

ADD_DEFINITIONS(-DLIBDIR)

SET(HEADER_LIST
    config.h
    include/iconv.h
    libcharset/include/localcharset.h
)

SET(SOURCE_LIST
    lib/iconv.c
    lib/relocatable.c
    libcharset/lib/localcharset.c
)

ADD_LIBRARY(${LIB_NAME} ${HEADER_LIST} ${SOURCE_LIST})

IF(MINGW OR UNIX)
   SET_TARGET_PROPERTIES(libiconv PROPERTIES OUTPUT_NAME iconv)
ENDIF()