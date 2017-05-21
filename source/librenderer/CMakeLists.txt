PROJECT(librenderer)

SET(LIB_NAME librenderer)

SET(HEADER_PATH ${CMAKE_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${CMAKE_SOURCE_DIR}/source/librenderer)

INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/HLSLCrossCompiler/include)

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

SET(RENDERER_LIGHTPROBE
    ${HEADER_PATH}/light_probe.h
    ${SOURCE_PATH}/light_probe.cpp
)
SOURCE_GROUP("renderer\\lightprobe" FILES ${RENDERER_LIGHTPROBE})

SET(RENDERER_MATERIAL
    ${HEADER_PATH}/material.h
    ${SOURCE_PATH}/material.cpp
    ${HEADER_PATH}/material_semantic.h
    ${SOURCE_PATH}/material_semantic.cpp
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
    ${SOURCE_PATH}/ssdo.h
    ${SOURCE_PATH}/ssdo.cpp
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

SET(RENDERER_LIST
    ${RENDERER_MATERIAL}
    ${RENDERER_SCENE}

    ${RENDERER_LIGHT}
    ${RENDERER_LIGHTPROBE}
    ${RENDERER_CAMERA}
    ${RENDERER_MESH}
    ${RENDERER_GEOMETRY}
    ${RENDERER_TERRAIN}
    ${RENDERER_FONT}
    ${RENDERER_POST_PROCESS}
    ${RENDER_PIPELINE}

    ${RENDERER_SYSTEM}
)

IF(BUILD_PLATFORM_APPLE)
    SET_SOURCE_FILES_PROPERTIES(${RENDERER_LIST} PROPERTIES LANGUAGE CXX)
ENDIF()

ADD_LIBRARY(${LIB_NAME} SHARED ${RENDERER_LIST})

TARGET_LINK_LIBRARIES(${LIB_NAME} PUBLIC lib3d)
TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE libplatform)

TARGET_LINK_LIBRARIES(${LIB_NAME} PRIVATE freetype)

SET_TARGET_ATTRIBUTE(${LIB_NAME} "core")