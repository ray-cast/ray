project(tinyxml)

SET(LIB_NAME tinyxml)

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/tinyxml/include)

SET(HEADER_LIST
    include/tinystr.h
    include/tinyxml.h
)

SET(SOURCE_LIST
    cpp/tinystr.cpp
    cpp/tinyxml.cpp
    cpp/tinyxmlerror.cpp
    cpp/tinyxmlparser.cpp
)

SOURCE_GROUP("tinyxml" FILES ${HEADER_LIST})
SOURCE_GROUP("tinyxml" FILES ${SOURCE_LIST})

ADD_LIBRARY(${LIB_NAME} ${HEADER_LIST} ${SOURCE_LIST})