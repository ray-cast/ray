CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FOREACH(myplugins
  unzip
  zlib
  libjson
  libjpeg
  libpng
  tinyxml
  angelscript
  freetype
  mygui
  asJIT
)
# 添加参与编译的子目录
ADD_SUBDIRECTORY(${myplugins})
ENDFOREACH()

FOREACH(myplugins
  libjson
  libjpeg
  libpng
  unzip
  zlib
  zlibstatic
  tinyxml
  angelscript
  freetype
  mygui
  dist
  asJIT
)
SET_TARGET_ATTRIBUTE(${myplugins} "contrib")

ENDFOREACH()

IF(BUILD_BULLET)
    ADD_SUBDIRECTORY(bullet)
    SET_TARGET_ATTRIBUTE(BulletCollision "contrib")
    SET_TARGET_ATTRIBUTE(BulletDynamics "contrib")
    SET_TARGET_ATTRIBUTE(BulletSoftBody "contrib")
    SET_TARGET_ATTRIBUTE(LinearMath "contrib")
    SET_TARGET_ATTRIBUTE(MiniCL "contrib")
    SET_TARGET_ATTRIBUTE(BulletSoftBodySolvers_OpenCL_Mini "contrib")
    IF(BUILD_MULTITHREADING)
        SET_TARGET_ATTRIBUTE(BulletMultiThreaded "contrib")
    ENDIF()
    IF(BUILD_NVIDIA_OPENCL_DEMOS)
        SET_TARGET_ATTRIBUTE(BulletSoftBodySolvers_OpenCL_NVidia "contrib")
    ENDIF()
ENDIF()

IF(BUILD_PLATFORM_GLES)
    ADD_SUBDIRECTORY(gles)
ENDIF()

IF(BUILD_PLATFORM_WINDOWS)
    ADD_SUBDIRECTORY(glew)
    SET_TARGET_ATTRIBUTE(glew "contrib")
ENDIF()

IF(BUILD_PLATFORM_SDL2)
  ADD_SUBDIRECTORY(SDL2)
  SET_TARGET_ATTRIBUTE(SDL2 "contrib")
  SET_TARGET_ATTRIBUTE(SDL2main "contrib")
  SET_TARGET_ATTRIBUTE(SDL2-static "contrib")
ENDIF()