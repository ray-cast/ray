CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

FOREACH(myplugins
    unzip
    zlib
    libjpeg
    libpng
    libiconv
    libvorbis
    libogg
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
    libvorbis
    libogg
    unzip
    zlib
    zlibstatic
    tinyxml
    freetype
)
SET_TARGET_ATTRIBUTE(${myplugins} "contrib")

ENDFOREACH()

IF(BUILD_PHYSIC)
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

IF(BUILD_PLATFORM_ANDROID)
  ADD_SUBDIRECTORY(android_native_app_glue)
ELSE()
    IF(BUILD_OPENGL_ES)
        ADD_SUBDIRECTORY(gles)
    ELSE()
        ADD_SUBDIRECTORY(glfw)
        ADD_SUBDIRECTORY(glew)
        SET_TARGET_ATTRIBUTE(glew "contrib")
        SET_TARGET_ATTRIBUTE(glfw "contrib")
    ENDIF()
ENDIF()

IF(BUILD_SCRIPT)
    ADD_SUBDIRECTORY(angelscript)
    SET_TARGET_ATTRIBUTE(angelscript "contrib")
ENDIF()

IF(BUILD_GUI AND BUILD_MYGUI)
    ADD_SUBDIRECTORY(MyGUIEngine)
    SET_TARGET_ATTRIBUTE(MyGUIEngine "contrib")
ENDIF()

IF(BUILD_SOUND)
    ADD_SUBDIRECTORY(openal)
ENDIF()
