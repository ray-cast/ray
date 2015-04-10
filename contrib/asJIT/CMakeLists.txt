SET(LIB_NAME asJIT)

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/angelscript/include)

SET(JIT_LIST
    as_jit.h
    as_jit.cpp
    virtual_asm.h
)
SOURCE_GROUP("asJIT" FILES ${JIT_LIST})

IF(UNIX)
    SET(JIT_PLATFORM_LIST
        virtual_asm_linux.cpp
    )
ELSEIF(WIN64)
    SET(JIT_PLATFORM_LIST
        virtual_asm_windows.cpp
        virtual_asm_x64.cpp
    )
ELSEIF(WIN32)
    SET(JIT_PLATFORM_LIST
        virtual_asm_windows.cpp
        virtual_asm_x86.cpp
    )
    SOURCE_GROUP("asJIT" FILES ${JIT_PLATFORM_LIST})
ENDIF()

SOURCE_GROUP("asJIT" FILES ${JIT_PLATFORM_LIST})
ADD_LIBRARY(${LIB_NAME} ${JIT_LIST} ${JIT_PLATFORM_LIST})