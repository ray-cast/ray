PROJECT(libplatform)

SET(LIB_NAME libplatform)

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/tinyxml)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/json/include)

SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/source/libplatform)

SET(PLATFORM_CORE_LIST
    ${HEADER_PATH}/def.h
    ${HEADER_PATH}/delegate.h
    ${HEADER_PATH}/delegate_impl.h
    ${HEADER_PATH}/format.h
    ${HEADER_PATH}/format_impl.h
    ${HEADER_PATH}/macro.h
    ${HEADER_PATH}/mutex.h
    ${HEADER_PATH}/platform.h
    ${HEADER_PATH}/queue.h
    ${HEADER_PATH}/singleton.h
    ${HEADER_PATH}/thread.h
    ${SOURCE_PATH}/thread.cpp
    ${HEADER_PATH}/thread_local.h
    ${HEADER_PATH}/trait.h
    ${HEADER_PATH}/interval.hpp
    ${SOURCE_PATH}/message.cpp
    ${HEADER_PATH}/message.h
    ${SOURCE_PATH}/string.cpp
    ${HEADER_PATH}/string.h
    ${SOURCE_PATH}/utf8.cpp
    ${HEADER_PATH}/utf8.h
    ${SOURCE_PATH}/version.cpp
    ${HEADER_PATH}/version.h
    ${HEADER_PATH}/variant.h
    ${SOURCE_PATH}/variant.cpp
    ${HEADER_PATH}/rtti.h
    ${SOURCE_PATH}/rtti.cpp
    ${HEADER_PATH}/rtti_factory.h
    ${SOURCE_PATH}/rtti_factory.cpp
    ${HEADER_PATH}/rtti_macros.h
    ${HEADER_PATH}/memory.h
    ${SOURCE_PATH}/timer.cpp
    ${HEADER_PATH}/timer.h
    ${HEADER_PATH}/reference.h
    ${SOURCE_PATH}/reference.cpp
    ${HEADER_PATH}/noncopyable.h
)
SOURCE_GROUP("core" FILES ${PLATFORM_CORE_LIST})

SET(PLATFORM_DEBUG_LIST
    ${HEADER_PATH}/allocate.h
    ${SOURCE_PATH}/allocate.cpp
    ${HEADER_PATH}/assert.h
    ${HEADER_PATH}/debug.h
    ${HEADER_PATH}/err.h
    ${SOURCE_PATH}/err.cpp
    ${HEADER_PATH}/error.h
    ${HEADER_PATH}/except.h
    ${SOURCE_PATH}/except.cpp
    ${HEADER_PATH}/new.h
    ${SOURCE_PATH}/new.cpp
    ${HEADER_PATH}/win_int.h
    ${SOURCE_PATH}/win_int.cpp
    ${HEADER_PATH}/win_wk.h
    ${SOURCE_PATH}/win_wk.cpp
    ${HEADER_PATH}/win_wkbs.h
    ${SOURCE_PATH}/win_wkbs.cpp
)
SOURCE_GROUP("debug" FILES ${PLATFORM_DEBUG_LIST})

SET(PLATFORM_IO_LIST
    ${SOURCE_PATH}/archive.cpp
    ${HEADER_PATH}/archive.h
    ${SOURCE_PATH}/archive_buf.cpp
    ${HEADER_PATH}/archive_buf.h
    ${SOURCE_PATH}/consolo.cpp
    ${HEADER_PATH}/consolo.h
    ${HEADER_PATH}/fcntl.h
    ${HEADER_PATH}/file.h
    ${SOURCE_PATH}/file.cpp
    ${SOURCE_PATH}/filebuf.cpp
    ${HEADER_PATH}/filebuf.h
    ${SOURCE_PATH}/fstream.cpp
    ${HEADER_PATH}/fstream.h
    ${SOURCE_PATH}/ioassign.cpp
    ${HEADER_PATH}/ioassign.h
    ${HEADER_PATH}/iosbase.h
    ${SOURCE_PATH}/iosbase.cpp
    ${HEADER_PATH}/iosfwd.h
    ${SOURCE_PATH}/iostream.cpp
    ${HEADER_PATH}/iostream.h
    ${SOURCE_PATH}/ioserver.cpp
    ${HEADER_PATH}/ioserver.h
    ${SOURCE_PATH}/iointerface.cpp
    ${HEADER_PATH}/iointerface.h
    ${SOURCE_PATH}/iolistener.cpp
    ${HEADER_PATH}/iolistener.h
    ${SOURCE_PATH}/istream.cpp
    ${HEADER_PATH}/istream.h
    ${SOURCE_PATH}/iarchive.cpp
    ${HEADER_PATH}/iarchive.h
    ${SOURCE_PATH}/ioarchive.cpp
    ${HEADER_PATH}/ioarchive.h
    ${SOURCE_PATH}/mstream.cpp
    ${HEADER_PATH}/mstream.h
    ${SOURCE_PATH}/oarchive.cpp
    ${HEADER_PATH}/oarchive.h
    ${SOURCE_PATH}/ostream.cpp
    ${HEADER_PATH}/ostream.h
    ${HEADER_PATH}/package.h
    ${SOURCE_PATH}/package.cpp
    ${HEADER_PATH}/res_loader.h
    ${SOURCE_PATH}/res_loader.cpp
    ${SOURCE_PATH}/stream.cpp
    ${HEADER_PATH}/stream.h
    ${SOURCE_PATH}/streambuf.cpp
    ${HEADER_PATH}/streambuf.h
    ${HEADER_PATH}/xmlreader.h
    ${SOURCE_PATH}/xmlreader.cpp
    ${HEADER_PATH}/jsonreader.h
    ${SOURCE_PATH}/jsonreader.cpp
)
SOURCE_GROUP("io" FILES ${PLATFORM_IO_LIST})

SET(PLATFORM_MATH_LIST
    ${HEADER_PATH}/aabb.h
    ${HEADER_PATH}/binary.h
    ${HEADER_PATH}/boundingbox.h
    ${HEADER_PATH}/dccmn.h
    ${HEADER_PATH}/frustum.h
    ${HEADER_PATH}/gdicmn.h
    ${HEADER_PATH}/line3.h
    ${SOURCE_PATH}/mathutil.cpp
    ${HEADER_PATH}/mathutil.h
    ${HEADER_PATH}/mathfwd.h
    ${HEADER_PATH}/math.h
    ${HEADER_PATH}/mat2.h
    ${HEADER_PATH}/mat3.h
    ${HEADER_PATH}/mat4.h
    ${HEADER_PATH}/noise.h
    ${SOURCE_PATH}/noise.cpp
    ${HEADER_PATH}/obb.h
    ${HEADER_PATH}/octree.h
    ${HEADER_PATH}/octree.inl
    ${HEADER_PATH}/parse.h
    ${HEADER_PATH}/plane.h
    ${HEADER_PATH}/quat.h
    ${HEADER_PATH}/raycast.h
    ${HEADER_PATH}/rect3.h
    ${HEADER_PATH}/sphere.h
    ${HEADER_PATH}/triangle.h
    ${HEADER_PATH}/vector2.h
    ${HEADER_PATH}/vector3.h
    ${HEADER_PATH}/vector4.h
    ${HEADER_PATH}/kdtree.h
    ${HEADER_PATH}/zintrin.h
    ${HEADER_PATH}/sh_impl.h
    ${HEADER_PATH}/sh.h
)
SOURCE_GROUP("math" FILES ${PLATFORM_MATH_LIST})

ADD_LIBRARY(${LIB_NAME} SHARED ${PLATFORM_CORE_LIST} ${PLATFORM_DEBUG_LIST} ${PLATFORM_IO_LIST} ${PLATFORM_MATH_LIST})
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE tinyxml)

IF(MINGW)
    FIND_LIBRARY(ICONV_FRAMEWORK iconv)
    IF(ICONV_FRAMEWORK MATCHES "iconv")
        TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE iconv)
    ELSE()
        TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE libiconv)
    ENDIF()
ELSE()
    TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE libiconv)
ENDIF()

IF(BUILD_PLATFORM_ANDROID)
    TARGET_LINK_LIBRARIES (${LIB_NAME} PRIVATE m)
ENDIF()

IF(BUILD_PLATFORM_WINDOWS)
    TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE dbghelp)
    TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE psapi)
    TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE version)
ENDIF()

SET_TARGET_ATTRIBUTE(${LIB_NAME} "core")
IF(MINGW OR UNIX)
    SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES OUTPUT_NAME platform)
ENDIF()