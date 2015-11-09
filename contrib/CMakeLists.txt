CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FOREACH(myplugins
  unzip
  zlib
  libjpeg
  libpng
  libiconv
  tinyxml
  freetype
)
# 添加参与编译的子目录
ADD_SUBDIRECTORY(${myplugins})
ENDFOREACH()

FOREACH(myplugins
  libjpeg
  libpng
  libiconv
  unzip
  zlib
  zlibstatic
  tinyxml
  freetype
)
SET_TARGET_ATTRIBUTE(${myplugins} "contrib")

ENDFOREACH()

IF(BUILD_PHYSIC)
    ADD_DEFINITIONS(-D_BUILD_PHYSIC)

    ADD_SUBDIRECTORY(bullet)
    SET_TARGET_ATTRIBUTE(BulletCollision "contrib")
    SET_TARGET_ATTRIBUTE(BulletDynamics "contrib")
    SET_TARGET_ATTRIBUTE(BulletSoftBody "contrib")
    SET_TARGET_ATTRIBUTE(LinearMath "contrib")
    IF(BUILD_MULTITHREADING)
        SET_TARGET_ATTRIBUTE(MiniCL "contrib")
        SET_TARGET_ATTRIBUTE(BulletSoftBodySolvers_OpenCL_Mini "contrib")
        SET_TARGET_ATTRIBUTE(BulletMultiThreaded "contrib")
    ENDIF()
    IF(BUILD_NVIDIA_OPENCL_DEMOS)
        SET_TARGET_ATTRIBUTE(BulletSoftBodySolvers_OpenCL_NVidia "contrib")
    ENDIF()
ENDIF()

IF(BUILD_GLES)
    ADD_SUBDIRECTORY(gles)
ELSEIF()
    ADD_SUBDIRECTORY(glfw)
    SET_TARGET_ATTRIBUTE(glfw "contrib")
ENDIF()

IF(_BUILD_OPENGL)
    ADD_SUBDIRECTORY(glew)
    SET_TARGET_ATTRIBUTE(glew "contrib")
ENDIF()

IF(_BUILD_SCRIPT)
    ADD_SUBDIRECTORY(angelscript)
    SET_TARGET_ATTRIBUTE(angelscript "contrib")
ENDIF()