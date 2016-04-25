SET(LIB_NAME librenderer)

IF(BUILD_PLATFORM_ANDROID)
    SET(RENDERSYSTEM 2)
ELSE()
    SET(RENDERSYSTEM 3)
ENDIF()

OPTION(BUILD_OPENGL_ES2 "ON for debug or OFF for release" ON)
OPTION(BUILD_OPENGL_ES3 "ON for debug or OFF for release" ON)
OPTION(BUILD_OPENGL_CORE "ON for debug or OFF for release" ON)
OPTION(BUILD_VULKAN "ON for debug or OFF for release" ON)

IF(LIBRARY_OUT_NAME MATCHES "64")
    SET(DEFAULT_LINK_LIBRARY 2)
ELSE()
    SET(DEFAULT_LINK_LIBRARY 1)
ENDIF()

SET(BUILD_OPENGL_ES_LIB ${DEFAULT_LINK_LIBRARY} CACHE STRING "Specify the platform. Possible values:
  1 - AMD
  2 - Mali"
  3 - PowerVR)

IF(BUILD_OPENGL_ES_LIB EQUAL 1)
    ADD_DEFINITIONS(-D__AMD__)
ELSEIF(BUILD_OPENGL_ES_LIB EQUAL 2)
    ADD_DEFINITIONS(-D__Mali__)
ELSEIF(BUILD_OPENGL_ES_LIB EQUAL 3)
    ADD_DEFINITIONS(-D__PowerVR__)
ENDIF()

IF(BUILD_OPENGL_ES2 OR BUILD_OPENGL_ES3)
    SET(BUILD_OPENGL_ES TRUE)
    ADD_DEFINITIONS(-D_BUILD_OPENGL_ES)
    IF(BUILD_OPENGL_ES2)
        ADD_DEFINITIONS(-D_BUILD_OPENGL_ES2)
    ENDIF()
    IF(BUILD_OPENGL_ES3)
        ADD_DEFINITIONS(-D_BUILD_OPENGL_ES3)
    ENDIF()
ENDIF()

IF(BUILD_OPENGL_CORE)
    ADD_DEFINITIONS(-D_BUILD_OPENGL)
    IF(BUILD_OPENGL_CORE)
        ADD_DEFINITIONS(-D_BUILD_OPENGL_CORE)
    ENDIF()
ENDIF()

IF(BUILD_VULKAN)
    ADD_DEFINITIONS(-D_BUILD_VULKAN)
ENDIF()

SET(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source/librenderer)

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/HLSLCrossCompiler/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/glsl-optimizer/src)

IF(BUILD_OPENGL_CORE OR BUILD_OPENGL_COMPATIBLE)
    INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/glew/include)
ENDIF()

IF(BUILD_OPENGL_ES)
    INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/gles/include)
ENDIF()

IF(BUILD_VULKAN)
    INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/glslang)
ENDIF()

IF(BUILD_VULKAN)
    INCLUDE_DIRECTORIES($ENV{VK_SDK_PATH}/include)
    IF(LIBRARY_OUT_NAME MATCHES "64")
        SET(VK_LINK_DIRECTORIES $ENV{VK_SDK_PATH}/Bin)
    ELSE()
        SET(VK_LINK_DIRECTORIES $ENV{VK_SDK_PATH}/Bin32)
    ENDIF()
ENDIF()

SET(RENDERER_CORE
    ${HEADER_PATH}/graphics_child.h
    ${SOURCE_PATH}/graphics_child.cpp
    ${HEADER_PATH}/graphics_context.h
    ${SOURCE_PATH}/graphics_context.cpp
    ${HEADER_PATH}/graphics_command.h
    ${SOURCE_PATH}/graphics_command.cpp
    ${HEADER_PATH}/graphics_data.h
    ${SOURCE_PATH}/graphics_data.cpp
    ${HEADER_PATH}/graphics_descriptor.h
    ${SOURCE_PATH}/graphics_descriptor.cpp
    ${HEADER_PATH}/graphics_device.h
    ${SOURCE_PATH}/graphics_device.cpp
    ${HEADER_PATH}/graphics_input_layout.h
    ${SOURCE_PATH}/graphics_input_layout.cpp
    ${HEADER_PATH}/graphics_pipeline.h
    ${SOURCE_PATH}/graphics_pipeline.cpp
    ${HEADER_PATH}/graphics_resource.h
    ${SOURCE_PATH}/graphics_resource.cpp
    ${HEADER_PATH}/graphics_sampler.h
    ${SOURCE_PATH}/graphics_sampler.cpp
    ${HEADER_PATH}/graphics_semaphore.h
    ${SOURCE_PATH}/graphics_semaphore.cpp
    ${HEADER_PATH}/graphics_shader.h
    ${SOURCE_PATH}/graphics_shader.cpp
    ${HEADER_PATH}/graphics_state.h
    ${SOURCE_PATH}/graphics_state.cpp
    ${HEADER_PATH}/graphics_swapchain.h
    ${SOURCE_PATH}/graphics_swapchain.cpp
    ${HEADER_PATH}/graphics_system.h
    ${SOURCE_PATH}/graphics_system.cpp
    ${HEADER_PATH}/graphics_texture.h
    ${SOURCE_PATH}/graphics_texture.cpp
    ${HEADER_PATH}/graphics_types.h
    ${HEADER_PATH}/graphics_framebuffer.h
    ${SOURCE_PATH}/graphics_framebuffer.cpp
    ${HEADER_PATH}/graphics_variant.h
    ${SOURCE_PATH}/graphics_variant.cpp
)
SOURCE_GROUP("core\\Common" FILES ${RENDERER_CORE})

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
    ${HEADER_PATH}/material_desc.h
    ${SOURCE_PATH}/material_desc.cpp
    ${HEADER_PATH}/material_loader.h
    ${SOURCE_PATH}/material_loader.cpp
    ${HEADER_PATH}/material_maker.h
    ${SOURCE_PATH}/material_maker.cpp
    ${HEADER_PATH}/material_manager.h
    ${SOURCE_PATH}/material_manager.cpp
    ${HEADER_PATH}/material_param.h
    ${SOURCE_PATH}/material_param.cpp
    ${HEADER_PATH}/material_pass.h
    ${SOURCE_PATH}/material_pass.cpp
    ${HEADER_PATH}/material_variant.h
    ${SOURCE_PATH}/material_variant.cpp
    ${HEADER_PATH}/material_tech.h
    ${SOURCE_PATH}/material_tech.cpp
)
SOURCE_GROUP("renderer\\material" FILES ${RENDERER_MATERIAL})

SET(RENDERER_SCENE
    ${HEADER_PATH}/render_object.h
    ${SOURCE_PATH}/render_object.cpp
    ${HEADER_PATH}/render_object_manager.h
    ${SOURCE_PATH}/render_object_manager.cpp
    ${HEADER_PATH}/render_object_manager_base.h
    ${SOURCE_PATH}/render_object_manager_base.cpp
    ${HEADER_PATH}/render_scene.h
    ${SOURCE_PATH}/render_scene.cpp
)
SOURCE_GROUP("renderer\\renderable" FILES ${RENDERER_SCENE})

SET(RENDERER_MESH
    ${HEADER_PATH}/render_mesh.h
    ${SOURCE_PATH}/render_mesh.cpp
)
SOURCE_GROUP("renderer\\mesh" FILES ${RENDERER_MESH})

SET(RENDERER_GEOMETRY
    ${SOURCE_PATH}/geometry.cpp
    ${HEADER_PATH}/geometry.h
)
SOURCE_GROUP("renderer\\geometry" FILES ${RENDERER_GEOMETRY})

SET(RENDERER_POST_PROCESS
    ${SOURCE_PATH}/atmospheric.h
    ${SOURCE_PATH}/atmospheric.cpp
    ${SOURCE_PATH}/color_grading.h
    ${SOURCE_PATH}/color_grading.cpp
    ${SOURCE_PATH}/dof.h
    ${SOURCE_PATH}/dof.cpp
    ${SOURCE_PATH}/fog.h
    ${SOURCE_PATH}/fog.cpp
    ${SOURCE_PATH}/fxaa.h
    ${SOURCE_PATH}/fxaa.cpp
    ${SOURCE_PATH}/fimic.h
    ${SOURCE_PATH}/fimic.cpp
    ${SOURCE_PATH}/light_shaft.h
    ${SOURCE_PATH}/light_shaft.cpp
    ${SOURCE_PATH}/ssao.h
    ${SOURCE_PATH}/ssao.cpp
    ${SOURCE_PATH}/ssgi.h
    ${SOURCE_PATH}/ssgi.cpp
    ${SOURCE_PATH}/ssr.h
    ${SOURCE_PATH}/ssr.cpp
    ${SOURCE_PATH}/ssss.h
    ${SOURCE_PATH}/ssss.cpp
    ${SOURCE_PATH}/irradiance.h
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
    ${HEADER_PATH}/terrain_mipmapping.h
    ${SOURCE_PATH}/terrain_mipmapping.cpp
    ${HEADER_PATH}/terrain_observer.h
    ${SOURCE_PATH}/terrain_observer.cpp
    ${HEADER_PATH}/terrain_types.h
)
SOURCE_GROUP("renderer\\terrain" FILES ${RENDERER_TERRAIN})

SET(RENDER_PIPELINE
    ${SOURCE_PATH}/deferred_lighting_pipeline.h
    ${SOURCE_PATH}/deferred_lighting_pipeline.cpp
    ${SOURCE_PATH}/forward_render_pipeline.h
    ${SOURCE_PATH}/forward_render_pipeline.cpp
    ${SOURCE_PATH}/shadow_render_pipeline.h
    ${SOURCE_PATH}/shadow_render_pipeline.cpp
    ${SOURCE_PATH}/post_render_pipeline.h
    ${SOURCE_PATH}/post_render_pipeline.cpp
)
SOURCE_GROUP("renderer\\pipeline" FILES ${RENDER_PIPELINE})

SET(RENDERER_SYSTEM
    ${HEADER_PATH}/render_pipeline.h
    ${SOURCE_PATH}/render_pipeline.cpp
    ${HEADER_PATH}/render_pipeline_device.h
    ${SOURCE_PATH}/render_pipeline_device.cpp
    ${HEADER_PATH}/render_pipeline_controller.h
    ${SOURCE_PATH}/render_pipeline_controller.cpp
    ${HEADER_PATH}/render_pipeline_manager.h
    ${SOURCE_PATH}/render_pipeline_manager.cpp
    ${HEADER_PATH}/render_post_process.h
    ${SOURCE_PATH}/render_post_process.cpp
    ${HEADER_PATH}/render_system.h
    ${SOURCE_PATH}/render_system.cpp
    ${HEADER_PATH}/render_setting.h
    ${SOURCE_PATH}/render_setting.cpp
    ${HEADER_PATH}/render_types.h
)
SOURCE_GROUP("renderer" FILES ${RENDERER_SYSTEM})

IF(BUILD_OPENGL_ES2)
    FILE(GLOB RENDERER_EGL2 "OpenGL ES2/*.*")
    SOURCE_GROUP("core\\OpenGL ES2" FILES ${RENDERER_EGL2})
ENDIF()

IF(BUILD_OPENGL_ES3)
    FILE(GLOB RENDERER_EGL3 "OpenGL ES3/*.*")
    SOURCE_GROUP("core\\OpenGL ES3" FILES ${RENDERER_EGL3})
ENDIF()

IF(BUILD_OPENGL_CORE)
    FILE(GLOB RENDERER_OGL_CORE "OpenGL Core/*.*")
    SOURCE_GROUP("core\\OpenGL Core" FILES ${RENDERER_OGL_CORE})
ENDIF()

IF(BUILD_VULKAN)
    FILE(GLOB RENDERER_VULKAN "Vulkan/*.*")
    SOURCE_GROUP("core\\Vulkan" FILES ${RENDERER_VULKAN})
ENDIF()

SET(RENDERER_LIST
    ${RENDERER_CORE}
    ${RENDERER_MATERIAL}
    ${RENDERER_SCENE}

    ${RENDERER_LIGHT}
    ${RENDERER_CAMERA}
    ${RENDERER_MESH}
    ${RENDERER_GEOMETRY}
    ${RENDERER_TERRAIN}
    ${RENDERER_FONT}
    ${RENDERER_POST_PROCESS}
    ${RENDER_PIPELINE}

    ${RENDERER_SYSTEM}
)
IF(BUILD_OPENGL_ES2)
    SET(RENDERER_LIST ${RENDERER_LIST} ${RENDERER_EGL2})
ENDIF()
IF(BUILD_OPENGL_ES3)
    SET(RENDERER_LIST ${RENDERER_LIST} ${RENDERER_EGL3})
ENDIF()
IF(BUILD_OPENGL_CORE)
    SET(RENDERER_LIST ${RENDERER_LIST} ${RENDERER_OGL_CORE})
ENDIF()
IF(BUILD_VULKAN)
    SET(RENDERER_LIST ${RENDERER_LIST} ${RENDERER_VULKAN})
ENDIF()

ADD_LIBRARY(${LIB_NAME} SHARED ${RENDERER_LIST})

TARGET_LINK_LIBRARIES(${LIB_NAME} libplatform libmodel libimage freetype libHLSLcc glsl_optimizer)

IF(BUILD_PLATFORM_WINDOWS)
    IF(BUILD_PLATFORM_WINDOWS)
        TARGET_LINK_LIBRARIES(${LIB_NAME} glew)
    ENDIF()
    IF(BUILD_OPENGL_ES)
        TARGET_LINK_LIBRARIES(${LIB_NAME} libEGL)
        TARGET_LINK_LIBRARIES(${LIB_NAME} libGLESv2)
    ENDIF()
ELSEIF(BUILD_PLATFORM_ANDROID)
    TARGET_LINK_LIBRARIES(${LIB_NAME} m)
    TARGET_LINK_LIBRARIES(${LIB_NAME} EGL)
    TARGET_LINK_LIBRARIES(${LIB_NAME} GLESv2)
    TARGET_LINK_LIBRARIES(${LIB_NAME} GLESv3)
ELSEIF(BUILD_PLATFORM_LINUX)
    TARGET_LINK_LIBRARIES(${LIB_NAME} GL)
    TARGET_LINK_LIBRARIES(${LIB_NAME} X11)
ENDIF()

IF(BUILD_VULKAN)
    TARGET_LINK_LIBRARIES(${LIB_NAME} ${VK_LINK_DIRECTORIES}/vulkan-1.lib)
    TARGET_LINK_LIBRARIES(${LIB_NAME} SPIRV)
    TARGET_LINK_LIBRARIES(${LIB_NAME} glslang)
    TARGET_LINK_LIBRARIES(${LIB_NAME} HLSL)
    TARGET_LINK_LIBRARIES(${LIB_NAME} OGLCompiler)
    TARGET_LINK_LIBRARIES(${LIB_NAME} OSDependent)
ENDIF()

IF(MINGW OR UNIX)
    SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES OUTPUT_NAME renderer)
ENDIF()

IF(BUILD_PLATFORM_ANDROID AND BUILD_OPENGL_ES3)
    SET_PROPERTY(TARGET ${LIB_NAME} PROPERTY VC_MDD_ANDROID_API_LEVEL "android-21")
ENDIF()

SET_TARGET_ATTRIBUTE(${LIB_NAME} "core")