IF(BUILD_PLATFORM_ANDROID)
    OPTION(BUILD_OPENGL_ES "on for DX11 off for EGL3" ON)
ELSE()
    OPTION(BUILD_OPENGL "on for DX11 off for EGL3" ON)
    OPTION(BUILD_OPENGL_ES "on for DX11 off for EGL3" OFF)
    OPTION(BUILD_DIRECTX_11 "on for DX11 off for EGL3" OFF)
ENDIF()

OPTION(BUILD_GLEW_SHARED  "on for use off for ignore" OFF)

SET(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source/librenderer)

IF(NOT BUILD_GLEW_SHARED)
    ADD_DEFINITIONS(-DGLEW_STATIC)
ENDIF()

IF(BUILD_DIRECTX_11)
    ADD_DEFINITIONS(-D_BUILD_DIRECTX_11)
ELSEIF(BUILD_OPENGL)
    ADD_DEFINITIONS(-D_BUILD_OPENGL)
ELSEIF(BUILD_OPENGL_ES)
    ADD_DEFINITIONS(-D_BUILD_OPENGL_ES)
ENDIF()

SET(RENDERER_CORE
    ${SOURCE_PATH}/render_buffer.cpp
    ${HEADER_PATH}/render_buffer.h
    ${HEADER_PATH}/render_device.h
    ${SOURCE_PATH}/render_device.cpp
    ${HEADER_PATH}/render_window.h
    ${SOURCE_PATH}/render_window.cpp
    ${HEADER_PATH}/render_state.h
    ${SOURCE_PATH}/render_state.cpp
    ${HEADER_PATH}/render_texture.h
    ${SOURCE_PATH}/render_texture.cpp
    ${HEADER_PATH}/render_command.h
    ${SOURCE_PATH}/render_command.cpp
    ${HEADER_PATH}/render_types.h
    ${HEADER_PATH}/shader.h
    ${SOURCE_PATH}/shader.cpp
)
SOURCE_GROUP("core" FILES ${RENDERER_CORE})

SET(RENDERER_CAMERA
    ${HEADER_PATH}/camera.h
    ${SOURCE_PATH}/camera.cpp
)
SOURCE_GROUP("renderer\\camera" FILES ${RENDERER_CAMERA})

SET(RENDERER_FONT
    ${HEADER_PATH}/font.h
    ${SOURCE_PATH}/font.cpp
    ${HEADER_PATH}/font_bitmap.h
    ${SOURCE_PATH}/font_bitmap.cpp
    ${HEADER_PATH}/font_distance_field.h
    ${SOURCE_PATH}/font_distance_field.cpp
)
SOURCE_GROUP("renderer\\font" FILES ${RENDERER_FONT})

SET(RENDERER_LIGHT
    ${HEADER_PATH}/light.h
    ${SOURCE_PATH}/light.cpp
)
SOURCE_GROUP("renderer\\light" FILES ${RENDERER_LIGHT})

SET(RENDERER_MATERIAL
    ${HEADER_PATH}/material.h
    ${SOURCE_PATH}/material.cpp
    ${HEADER_PATH}/material_fwd.h
    ${HEADER_PATH}/material_param.h
    ${SOURCE_PATH}/material_param.cpp
    ${HEADER_PATH}/material_manager.h
    ${SOURCE_PATH}/material_manager.cpp
    ${HEADER_PATH}/material_pass.h
    ${SOURCE_PATH}/material_pass.cpp
    ${HEADER_PATH}/material_tech.h
    ${SOURCE_PATH}/material_tech.cpp
    ${HEADER_PATH}/material_maker.h
    ${SOURCE_PATH}/material_maker.cpp
)
SOURCE_GROUP("renderer\\material" FILES ${RENDERER_MATERIAL})

SET(RENDERER_SCENE
    ${HEADER_PATH}/render_object.h
    ${SOURCE_PATH}/render_object.cpp
    ${HEADER_PATH}/render_scene.h
    ${SOURCE_PATH}/render_scene.cpp
)
SOURCE_GROUP("renderer\\renderable" FILES ${RENDERER_SCENE})

SET(RENDERER_MESH
    ${HEADER_PATH}/render_mesh.h
    ${SOURCE_PATH}/render_mesh.cpp
)
SOURCE_GROUP("renderer\\mesh" FILES ${RENDERER_MESH})

SET(RENDERER_POST_PROCESS
    ${HEADER_PATH}/atmospheric.h
    ${SOURCE_PATH}/atmospheric.cpp
    ${HEADER_PATH}/color_grading.h
    ${SOURCE_PATH}/color_grading.cpp
    ${HEADER_PATH}/dof.h
    ${SOURCE_PATH}/dof.cpp
    ${HEADER_PATH}/fog.h
    ${SOURCE_PATH}/fog.cpp
    ${HEADER_PATH}/fxaa.h
    ${SOURCE_PATH}/fxaa.cpp
    ${HEADER_PATH}/fimic.h
    ${SOURCE_PATH}/fimic.cpp
    ${HEADER_PATH}/light_shaft.h
    ${SOURCE_PATH}/light_shaft.cpp
    ${HEADER_PATH}/ssao.h
    ${SOURCE_PATH}/ssao.cpp
    ${HEADER_PATH}/ssgi.h
    ${SOURCE_PATH}/ssgi.cpp
    ${HEADER_PATH}/ssr.h
    ${SOURCE_PATH}/ssr.cpp
    ${HEADER_PATH}/ssss.h
    ${SOURCE_PATH}/ssss.cpp
    ${HEADER_PATH}/irradiance.h
    ${SOURCE_PATH}/irradiance.cpp
)
SOURCE_GROUP("renderer\\postprocess" FILES ${RENDERER_POST_PROCESS})

SET(RENDERER_TERRAIN
    ${HEADER_PATH}/terrain.h
    ${SOURCE_PATH}/terrain.cpp
    ${HEADER_PATH}/terrain_chunk.h
    ${SOURCE_PATH}/terrain_chunk.cpp
    ${HEADER_PATH}/terrain_height_map.h
    ${SOURCE_PATH}/terrain_height_map.cpp
    ${HEADER_PATH}/terrain_lod.h
    ${SOURCE_PATH}/terrain_lod.cpp
    ${HEADER_PATH}/terrain_map.h
    ${SOURCE_PATH}/terrain_map.cpp
    ${HEADER_PATH}/terrain_mipmapping.h
    ${SOURCE_PATH}/terrain_mipmapping.cpp
    ${HEADER_PATH}/terrain_observer.h
    ${SOURCE_PATH}/terrain_observer.cpp
    ${HEADER_PATH}/terrain_types.h
)
SOURCE_GROUP("renderer\\terrain" FILES ${RENDERER_TERRAIN})

SET(RENDERER_SYSTEM
    ${HEADER_PATH}/render_pipeline.h
    ${SOURCE_PATH}/render_pipeline.cpp
    ${HEADER_PATH}/render_pipeline_base.h
    ${SOURCE_PATH}/render_pipeline_base.cpp
    ${HEADER_PATH}/render_pipeline_manager.h
    ${SOURCE_PATH}/render_pipeline_manager.cpp
    ${HEADER_PATH}/render_pipeline_manager_base.h
    ${SOURCE_PATH}/render_pipeline_manager_base.cpp
    ${HEADER_PATH}/render_post_process.h
    ${SOURCE_PATH}/render_post_process.cpp
    ${HEADER_PATH}/render_setting.h
    ${SOURCE_PATH}/render_setting.cpp
    ${HEADER_PATH}/render_system.h
    ${SOURCE_PATH}/render_system.cpp
)
SOURCE_GROUP("renderer" FILES ${RENDERER_SYSTEM})

IF(BUILD_OPENGL_ES)
    FILE(GLOB RENDERER_EGL3 "OpenGL ES3/*.*")
    SOURCE_GROUP("core\\egl3" FILES ${RENDERER_EGL3})
ENDIF()

ADD_LIBRARY(librenderer SHARED
    ${RENDERER_CORE}
    ${RENDERER_EGL3}
    ${RENDERER_MATERIAL}
    ${RENDERER_SCENE}

    ${RENDERER_LIGHT}
    ${RENDERER_CAMERA}
    ${RENDERER_MESH}
    ${RENDERER_TERRAIN}
    ${RENDERER_FONT}
    ${RENDERER_POST_PROCESS}

    ${RENDERER_SYSTEM}
)

TARGET_LINK_LIBRARIES(librenderer libplatform libmodel libimage freetype)

IF(BUILD_PLATFORM_WINDOWS)
    IF(BUILD_OPENGL_ES)
        TARGET_LINK_LIBRARIES(librenderer libEGL)
        TARGET_LINK_LIBRARIES(librenderer libGLESv2)
    ELSE()
        TARGET_LINK_LIBRARIES(librenderer glew)
    ENDIF()
ELSEIF(BUILD_PLATFORM_ANDROID)
    TARGET_LINK_LIBRARIES(librenderer EGL)
    TARGET_LINK_LIBRARIES(librenderer GLESv2)
    TARGET_LINK_LIBRARIES(librenderer GLESv3)
ELSEIF(BUILD_PLATFORM_LINUX)
    TARGET_LINK_LIBRARIES(librenderer GL)
    TARGET_LINK_LIBRARIES(librenderer X11)
ENDIF()

IF(MINGW OR UNIX)
    SET_TARGET_PROPERTIES(librenderer PROPERTIES OUTPUT_NAME renderer)
ENDIF()
SET_TARGET_ATTRIBUTE(librenderer "core")