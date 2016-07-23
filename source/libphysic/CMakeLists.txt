PROJECT(libphysic)

SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/source/libphysic)

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/bullet/src)

SET(PHYSICS_SYSTEM_LIST
    ${SOURCE_PATH}/physics_scene.cpp
    ${HEADER_PATH}/physics_scene.h
    ${SOURCE_PATH}/physics_system.cpp
    ${HEADER_PATH}/physics_system.h
    ${SOURCE_PATH}/physics_character.cpp
    ${HEADER_PATH}/physics_character.h
    ${SOURCE_PATH}/physics_joint.cpp
    ${HEADER_PATH}/physics_joint.h
    ${SOURCE_PATH}/physics_joint_configurable.cpp
    ${HEADER_PATH}/physics_joint_configurable.h
    ${SOURCE_PATH}/physics_joint_fixed.cpp
    ${HEADER_PATH}/physics_joint_fixed.h
    ${SOURCE_PATH}/physics_joint_spring.cpp
    ${HEADER_PATH}/physics_joint_spring.h
    ${SOURCE_PATH}/physics_body.cpp
    ${HEADER_PATH}/physics_body.h
    ${SOURCE_PATH}/physics_shape.cpp
    ${HEADER_PATH}/physics_shape.h
    ${SOURCE_PATH}/physics_shape_sphere.cpp
    ${HEADER_PATH}/physics_shape_sphere.h
    ${SOURCE_PATH}/physics_shape_box.cpp
    ${HEADER_PATH}/physics_shape_box.h
    ${SOURCE_PATH}/physics_shape_capsule.cpp
    ${HEADER_PATH}/physics_shape_capsule.h
    ${SOURCE_PATH}/physics_shape_mesh.cpp
    ${HEADER_PATH}/physics_shape_mesh.h
    ${HEADER_PATH}/physics_shape_terrain.h
    ${SOURCE_PATH}/physics_shape_terrain.cpp
)
SOURCE_GROUP("physics" FILES ${PHYSICS_SYSTEM_LIST})

ADD_LIBRARY(libphysic SHARED ${PHYSICS_SYSTEM_LIST})

TARGET_LINK_LIBRARIES(libphysic PUBLIC libplatform)

TARGET_LINK_LIBRARIES(libphysic PRIVATE BulletDynamics)
TARGET_LINK_LIBRARIES(libphysic PRIVATE BulletCollision)
TARGET_LINK_LIBRARIES(libphysic PRIVATE BulletSoftBody)
TARGET_LINK_LIBRARIES(libphysic PRIVATE LinearMath)

IF(BUILD_MULTITHREADING)
    TARGET_LINK_LIBRARIES(libphysic BulletMultiThreaded)
ENDIF()

SET_TARGET_ATTRIBUTE(libphysic "core")