project(tinyxml)

SET(LIB_NAME tinyxml)

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/tinyxml)

SET(HEADER_LIST
   tinystr.h
   tinyxml.h
)

SET(SOURCE_LIST
    tinystr.cpp
    tinyxml.cpp
    tinyxmlerror.cpp
    tinyxmlparser.cpp
)

SOURCE_GROUP("tinyxml" FILES ${HEADER_LIST})
SOURCE_GROUP("tinyxml" FILES ${SOURCE_LIST})

ADD_LIBRARY(${LIB_NAME} STATIC ${HEADER_LIST} ${SOURCE_LIST})