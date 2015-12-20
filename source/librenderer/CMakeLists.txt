IF(BUILD_PLATFORM_ANDROID)
    SET(RENDERSYSTEM 2)
ELSE()
    SET(RENDERSYSTEM 3)
ENDIF()

SET(BUILD_RENDERSYSTEM ${RENDERSYSTEM} CACHE STRING
"Specify the Render System. Possible values:
  1 - OpenGL ES2
  2 - OpenGL ES3
  3 - OpenGL Core"
)

SET(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source/librenderer)

IF(NOT GLEW_BUILD_SHARED_LIBS)
    ADD_DEFINITIONS(-DGLEW_STATIC)
ENDIF()

IF(BUILD_RENDERSYSTEM EQUAL 1)
    SET(BUILD_OPENGL_ES TRUE)
    SET(BUILD_OPENGL_ES2 TRUE)
    ADD_DEFINITIONS(-D_BUILD_OPENGL_ES)
    ADD_DEFINITIONS(-D_BUILD_OPENGL_ES2)
ELSEIF(BUILD_RENDERSYSTEM EQUAL 2)
    SET(BUILD_OPENGL_ES TRUE)
    SET(BUILD_OPENGL_ES3 TRUE)
    ADD_DEFINITIONS(-D_BUILD_OPENGL_ES)
    ADD_DEFINITIONS(-D_BUILD_OPENGL_ES3)
ELSEIF(BUILD_RENDERSYSTEM EQUAL 3)
    SET(BUILD_OPENGL TRUE)
    ADD_DEFINITIONS(-D_BUILD_OPENGL)
    ADD_DEFINITIONS(-D_BUILD_OPENGL_CORE)
ENDIF()

SET(RENDERER_CORE
    ${HEADER_PATH}/graphics_child.h
    ${SOURCE_PATH}/graphics_child.cpp
    ${HEADER_PATH}/graphics_data.h
    ${SOURCE_PATH}/graphics_data.cpp
    ${HEADER_PATH}/graphics_layout.h
    ${SOURCE_PATH}/graphics_layout.cpp
    ${HEADER_PATH}/graphics_resource.h
    ${SOURCE_PATH}/graphics_resource.cpp
    ${HEADER_PATH}/graphics_device.h
    ${SOURCE_PATH}/graphics_device.cpp
    ${HEADER_PATH}/graphics_context.h
    ${SOURCE_PATH}/graphics_context.cpp
    ${HEADER_PATH}/graphics_state.h
    ${SOURCE_PATH}/graphics_state.cpp
    ${HEADER_PATH}/graphics_sampler.h
    ${SOURCE_PATH}/graphics_sampler.cpp
    ${HEADER_PATH}/graphics_types.h
    ${HEADER_PATH}/graphics_texture.h
    ${SOURCE_PATH}/graphics_texture.cpp
    ${HEADER_PATH}/graphics_view.h
    ${SOURCE_PATH}/graphics_view.cpp
    ${HEADER_PATH}/graphics_command.h
    ${SOURCE_PATH}/graphics_command.cpp
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
    ${SOURCE_PATH}/render_buffer.cpp
    ${HEADER_PATH}/render_buffer.h
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

SET(RENDER_PIPELINE
    ${HEADER_PATH}/deferred_render_pipeline.h
    ${SOURCE_PATH}/deferred_render_pipeline.cpp
)
SOURCE_GROUP("renderer\\pipeline" FILES ${RENDER_PIPELINE})

SET(RENDERER_SYSTEM
    ${HEADER_PATH}/render_data_manager.h
    ${SOURCE_PATH}/render_data_manager.cpp
    ${HEADER_PATH}/render_data_manager_base.h
    ${SOURCE_PATH}/render_data_manager_base.cpp
    ${HEADER_PATH}/render_pipeline.h
    ${SOURCE_PATH}/render_pipeline.cpp
    ${HEADER_PATH}/render_pipeline_controller.h
    ${SOURCE_PATH}/render_pipeline_controller.cpp
    ${HEADER_PATH}/render_pipeline_manager.h
    ${SOURCE_PATH}/render_pipeline_manager.cpp
    ${HEADER_PATH}/render_post_process.h
    ${SOURCE_PATH}/render_post_process.cpp
    ${HEADER_PATH}/render_setting.h
    ${SOURCE_PATH}/render_setting.cpp
    ${HEADER_PATH}/render_system.h
    ${SOURCE_PATH}/render_system.cpp
    ${HEADER_PATH}/render_types.h
)
SOURCE_GROUP("renderer" FILES ${RENDERER_SYSTEM})

IF(BUILD_OPENGL_ES2)
    FILE(GLOB RENDERER_OGL "OpenGL ES2/*.*")
    SOURCE_GROUP("core\\egl2" FILES ${RENDERER_OGL})
ELSEIF(BUILD_OPENGL_ES3)
    FILE(GLOB RENDERER_OGL "OpenGL ES3/*.*")
    SOURCE_GROUP("core\\egl3" FILES ${RENDERER_OGL})
ELSEIF(BUILD_OPENGL)
    FILE(GLOB RENDERER_OGL "OpenGL Core/*.*")
    SOURCE_GROUP("core\\ogl" FILES ${RENDERER_OGL})
ENDIF()

ADD_LIBRARY(librenderer SHARED
    ${RENDERER_CORE}
    ${RENDERER_OGL}
    ${RENDERER_MATERIAL}
    ${RENDERER_SCENE}

    ${RENDERER_LIGHT}
    ${RENDERER_CAMERA}
    ${RENDERER_MESH}
    ${RENDERER_TERRAIN}
    ${RENDERER_FONT}
    ${RENDERER_POST_PROCESS}
    ${RENDER_PIPELINE}

    ${RENDERER_SYSTEM}
)

TARGET_LINK_LIBRARIES(librenderer libplatform libmodel libimage freetype libHLSLcc)

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