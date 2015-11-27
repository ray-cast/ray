INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/zlib)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/freeglut/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/glew/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/libjpeg)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/libpng)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/lua/src)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/tinyxml/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/freetype/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/bullet/src)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/angelscript/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/libiconv/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/asJIT)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/mojoshader)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/gles/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/libogg)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/libvorbis)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/MyGUIEngine/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/MyGUIPlatform/include)
INCLUDE_DIRECTORIES(${DEPENDENCIES_PATH}/openal/include)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)

SET(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source)

ADD_DEFINITIONS(-D_BUILD_DLL_EXPORT)
ADD_DEFINITIONS(-D_ALLOW_KEYWORD_MACROS)

IF(BUILD_INPUT)
    ADD_DEFINITIONS(-D_BUILD_INPUT)
ENDIF()

IF(BUILD_BASEGAME)
    ADD_DEFINITIONS(-D_BUILD_BASEGAME)
ENDIF()

IF(BUILD_PHYSIC)
    ADD_DEFINITIONS(-D_BUILD_PHYSIC)
ENDIF()

IF(BUILD_SCRIPT)
    ADD_DEFINITIONS(-D_BUILD_SCRIPT)
ENDIF()

IF(BUILD_GUI)
    ADD_DEFINITIONS(-D_BUILD_GUI)
ENDIF()

IF(BUILD_MYGUI)
    ADD_DEFINITIONS(-DMYGUI_STATIC)
    ADD_DEFINITIONS(-D_BUILD_MYGUI)
ENDIF()

IF(BUILD_RENDERER)
    ADD_DEFINITIONS(-D_BUILD_RENDERER)
ENDIF()

IF(NOT BUILD_GLEW_SHARED)
    ADD_DEFINITIONS(-DGLEW_STATIC)
ENDIF()

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
    ${HEADER_PATH}/scope_ptr.h
    ${HEADER_PATH}/shared_ptr.h
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
    ${SOURCE_PATH}/iolistener.cpp
    ${HEADER_PATH}/iolistener.h
    ${SOURCE_PATH}/iointerface.cpp
    ${HEADER_PATH}/iointerface.h
    ${SOURCE_PATH}/istream.cpp
    ${HEADER_PATH}/istream.h
    ${SOURCE_PATH}/archive.cpp
    ${HEADER_PATH}/archive.h
    ${SOURCE_PATH}/ostream.cpp
    ${HEADER_PATH}/ostream.h
    ${SOURCE_PATH}/mstream.cpp
    ${HEADER_PATH}/mstream.h
    ${SOURCE_PATH}/stream.cpp
    ${HEADER_PATH}/stream.h
    ${HEADER_PATH}/package.h
    ${SOURCE_PATH}/streambuf.cpp
    ${HEADER_PATH}/streambuf.h
    ${HEADER_PATH}/xmlreader.h
    ${SOURCE_PATH}/xmlreader.cpp
)
SOURCE_GROUP("io" FILES ${PLATFORM_IO_LIST})

SET(PLATFORM_MATH_LIST
    ${HEADER_PATH}/aabb.h
    ${HEADER_PATH}/array.h
    ${HEADER_PATH}/binary.h
    ${HEADER_PATH}/bound.h
    ${HEADER_PATH}/dccmn.h
    ${HEADER_PATH}/euler.h
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
    ${HEADER_PATH}/plane3.h
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
)
SOURCE_GROUP("math" FILES ${PLATFORM_MATH_LIST})

SET(INPUT_CORE_LIST
    ${HEADER_PATH}/input_button.h
    ${SOURCE_PATH}/input_controller.cpp
    ${HEADER_PATH}/input_controller.h
    ${HEADER_PATH}/input_device.h
    ${SOURCE_PATH}/input_device.cpp
    ${SOURCE_PATH}/input_device_base.cpp
    ${HEADER_PATH}/input_device_base.h
    ${HEADER_PATH}/input_event.h
    ${SOURCE_PATH}/input_event.cpp
    ${HEADER_PATH}/input_key.h
    ${HEADER_PATH}/input_keyboard.h
    ${SOURCE_PATH}/input_keyboard.cpp
    ${HEADER_PATH}/input_keyboard_base.h
    ${SOURCE_PATH}/input_keyboard_base.cpp
    ${SOURCE_PATH}/input_mouse.cpp
    ${HEADER_PATH}/input_mouse.h
    ${HEADER_PATH}/input_mouse_base.h
    ${SOURCE_PATH}/input_mouse_base.cpp
    ${HEADER_PATH}/input.h
    ${SOURCE_PATH}/input.cpp
    ${HEADER_PATH}/input_base.h
    ${SOURCE_PATH}/input_base.cpp
    ${HEADER_PATH}/input_types.h
    ${HEADER_PATH}/input_ps3.h
    ${HEADER_PATH}/input_xbox.h
)
SOURCE_GROUP("input\\core" FILES ${INPUT_CORE_LIST})

SET(INPUT_MSW_LIST
    ${HEADER_PATH}/msw_input_device.h
    ${SOURCE_PATH}/msw_input_device.cpp
    ${HEADER_PATH}/msw_input_keyboard.h
    ${SOURCE_PATH}/msw_input_keyboard.cpp
    ${HEADER_PATH}/msw_input_mouse.h
    ${SOURCE_PATH}/msw_input_mouse.cpp
)
SOURCE_GROUP("input\\msw" FILES ${INPUT_MSW_LIST})

SET(INPUT_SDL_LIST
    ${HEADER_PATH}/sdl_input_device.h
    ${SOURCE_PATH}/sdl_input_device.cpp
    ${HEADER_PATH}/sdl_input_keyboard.h
    ${SOURCE_PATH}/sdl_input_keyboard.cpp
    ${HEADER_PATH}/sdl_input_mouse.h
    ${SOURCE_PATH}/sdl_input_mouse.cpp
)
SOURCE_GROUP("input\\sdl" FILES ${INPUT_SDL_LIST})

SET(IMAGE_LIST
    ${HEADER_PATH}/imagall.h
    ${SOURCE_PATH}/imagall.cpp
    ${HEADER_PATH}/imagbase.h
    ${SOURCE_PATH}/imagbmp.cpp
    ${HEADER_PATH}/imagbmp.h
    ${HEADER_PATH}/imagcfg.h
    ${HEADER_PATH}/imagdef.h
    ${SOURCE_PATH}/image.cpp
    ${HEADER_PATH}/image.h
    ${HEADER_PATH}/imagfwd.h
    ${HEADER_PATH}/imaggif.h
    ${SOURCE_PATH}/imaggif.cpp
    ${HEADER_PATH}/imagjpeg.h
    ${SOURCE_PATH}/imagjpeg.cpp
    ${HEADER_PATH}/imagpng.h
    ${SOURCE_PATH}/imagpng.cpp
    ${HEADER_PATH}/imagtga.h
    ${SOURCE_PATH}/imagtga.cpp
    ${HEADER_PATH}/imagdds.h
    ${SOURCE_PATH}/imagdds.cpp
)
SOURCE_GROUP("image" FILES ${IMAGE_LIST})

SET(MODEL_LIST
    ${HEADER_PATH}/modall.h
    ${SOURCE_PATH}/modall.cpp
    ${HEADER_PATH}/modcfg.h
    ${HEADER_PATH}/moddef.h
    ${HEADER_PATH}/model.h
    ${SOURCE_PATH}/model.cpp
    ${HEADER_PATH}/modhelp.h
    ${SOURCE_PATH}/modhelp.cpp
    ${HEADER_PATH}/modobj.h
    ${SOURCE_PATH}/modobj.cpp
    ${HEADER_PATH}/modpmd.h
    ${SOURCE_PATH}/modpmd.cpp
    ${HEADER_PATH}/modpmx.h
    ${SOURCE_PATH}/modpmx.cpp
    ${HEADER_PATH}/modvmd.h
    ${SOURCE_PATH}/modvmd.cpp
    ${HEADER_PATH}/modsdkmesh.h
    ${SOURCE_PATH}/modsdkmesh.cpp
    ${HEADER_PATH}/modtypes.h
    ${HEADER_PATH}/modutil.h
    ${SOURCE_PATH}/modutil.cpp
    ${HEADER_PATH}/anim.h
    ${SOURCE_PATH}/anim.cpp
    ${HEADER_PATH}/bone.h
    ${SOURCE_PATH}/bone.cpp
)
SOURCE_GROUP("model" FILES ${MODEL_LIST})

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

SET(RENDERER_OGL
    ${HEADER_PATH}/ogl_buffer.h
    ${SOURCE_PATH}/ogl_buffer.cpp
    ${HEADER_PATH}/ogl_renderer.h
    ${SOURCE_PATH}/ogl_renderer.cpp
    ${HEADER_PATH}/ogl_commandlist.h
    ${SOURCE_PATH}/ogl_commandlist.cpp
    ${HEADER_PATH}/ogl_canvas.h
    ${SOURCE_PATH}/ogl_canvas.cpp
    ${HEADER_PATH}/ogl_shader.h
    ${SOURCE_PATH}/ogl_shader.cpp
    ${HEADER_PATH}/ogl_state.h
    ${SOURCE_PATH}/ogl_state.cpp
    ${HEADER_PATH}/ogl_texture.h
    ${SOURCE_PATH}/ogl_texture.cpp
    ${HEADER_PATH}/ogl_types.h
    ${SOURCE_PATH}/ogl_types.cpp
    ${HEADER_PATH}/wgl_canvas.h
    ${SOURCE_PATH}/wgl_canvas.cpp
    ${HEADER_PATH}/egl_canvas.h
    ${SOURCE_PATH}/egl_canvas.cpp
    ${HEADER_PATH}/x11_canvas.h
    ${SOURCE_PATH}/x11_canvas.cpp
)
SOURCE_GROUP("core\\device\\OGL" FILES ${RENDERER_OGL})

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

SET(NETWORK_LIST
    ${SOURCE_PATH}/network.cpp
    ${HEADER_PATH}/network.h
)
SOURCE_GROUP("network" FILES ${NETWORK_LIST})

SET(SOUND_SYSTEM_LIST
    ${SOURCE_PATH}/al_sound_buffer.cpp
    ${SOURCE_PATH}/al_sound_system.cpp
	${SOURCE_PATH}/al_sound_source.cpp
    ${SOURCE_PATH}/sound_buffer.cpp
    ${SOURCE_PATH}/sound_system.cpp
	${SOURCE_PATH}/sound_source.cpp
	${SOURCE_PATH}/sound_factory.cpp
    ${HEADER_PATH}/al_sound_buffer.h
    ${HEADER_PATH}/al_sound_system.h
	${HEADER_PATH}/al_sound_source.h
    ${HEADER_PATH}/sound_buffer.h
	${HEADER_PATH}/sound_factory.h
    ${HEADER_PATH}/sound_system.h
	${HEADER_PATH}/sound_source.h
    ${HEADER_PATH}/sound_types.h
)
SOURCE_GROUP("sound" FILES ${SOUND_SYSTEM_LIST})

SET(MYGUI_LIST
    ${HEADER_PATH}/mygui_types.h
    ${HEADER_PATH}/mygui_system.h
    ${SOURCE_PATH}/mygui_system.cpp
    ${HEADER_PATH}/mygui_buffer.h
    ${SOURCE_PATH}/mygui_buffer.cpp
    ${HEADER_PATH}/mygui_manager.h
    ${SOURCE_PATH}/mygui_manager.cpp
    ${HEADER_PATH}/mygui_texture.h
    ${SOURCE_PATH}/mygui_texture.cpp
    ${HEADER_PATH}/mygui_renderer.h
    ${SOURCE_PATH}/mygui_renderer.cpp
    ${HEADER_PATH}/mygui_widget.h
    ${SOURCE_PATH}/mygui_widget.cpp
    ${HEADER_PATH}/mygui_button.h
    ${SOURCE_PATH}/mygui_button.cpp
    ${HEADER_PATH}/mygui_textbox.h
    ${SOURCE_PATH}/mygui_textbox.cpp
    ${HEADER_PATH}/mygui_window.h
    ${SOURCE_PATH}/mygui_window.cpp
    ${HEADER_PATH}/mygui_imagebox.h
    ${SOURCE_PATH}/mygui_imagebox.cpp
    ${HEADER_PATH}/mygui_listbox.h
    ${SOURCE_PATH}/mygui_listbox.cpp
)
SOURCE_GROUP("gui\\mygui" FILES ${MYGUI_LIST})

SET(GUI_LIST
    ${HEADER_PATH}/gui_align.h
    ${HEADER_PATH}/gui_system.h
    ${SOURCE_PATH}/gui_system.cpp
    ${HEADER_PATH}/gui_system_base.h
    ${SOURCE_PATH}/gui_system_base.cpp
    ${HEADER_PATH}/gui_imageloader.h
    ${SOURCE_PATH}/gui_imageloader.cpp
    ${HEADER_PATH}/gui_types.h
    ${HEADER_PATH}/gui_input_key.h
    ${HEADER_PATH}/gui_input_button.h
    ${HEADER_PATH}/gui_widget.h
    ${SOURCE_PATH}/gui_widget.cpp
    ${HEADER_PATH}/gui_button.h
    ${SOURCE_PATH}/gui_button.cpp
    ${HEADER_PATH}/gui_textbox.h
    ${SOURCE_PATH}/gui_textbox.cpp
    ${HEADER_PATH}/gui_window.h
    ${SOURCE_PATH}/gui_window.cpp
    ${HEADER_PATH}/gui_imagebox.h
    ${SOURCE_PATH}/gui_imagebox.cpp
    ${HEADER_PATH}/gui_editbox.h
    ${SOURCE_PATH}/gui_editbox.cpp
)
SOURCE_GROUP("gui" FILES ${GUI_LIST})

SET(PHYSICS_SYSTEM_LIST
    ${SOURCE_PATH}/physics_scene.cpp
    ${HEADER_PATH}/physics_scene.h
    ${SOURCE_PATH}/physics_system.cpp
    ${HEADER_PATH}/physics_system.h
    ${SOURCE_PATH}/physics_character.cpp
    ${HEADER_PATH}/physics_character.h
    ${SOURCE_PATH}/physics_joint.cpp
    ${HEADER_PATH}/physics_joint.h
    ${SOURCE_PATH}/physics_joint_ball.cpp
    ${HEADER_PATH}/physics_joint_ball.h
    ${SOURCE_PATH}/physics_joint_fixed.cpp
    ${HEADER_PATH}/physics_joint_fixed.h
    ${SOURCE_PATH}/physics_joint_spring.cpp
    ${HEADER_PATH}/physics_joint_spring.h
    ${SOURCE_PATH}/physics_rigidbody.cpp
    ${HEADER_PATH}/physics_rigidbody.h
    ${SOURCE_PATH}/physics_shape.cpp
    ${HEADER_PATH}/physics_shape.h
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

SET(GAMEAPP_LIST
    ${SOURCE_PATH}/game_application.cpp
    ${HEADER_PATH}/game_application.h
    ${SOURCE_PATH}/game_object.cpp
    ${HEADER_PATH}/game_object.h
    ${SOURCE_PATH}/game_base_features.cpp
    ${HEADER_PATH}/game_base_features.h
    ${SOURCE_PATH}/game_object_manager.cpp
    ${HEADER_PATH}/game_object_manager.h
    ${SOURCE_PATH}/game_component.cpp
    ${HEADER_PATH}/game_component.h
    ${SOURCE_PATH}/game_features.cpp
    ${HEADER_PATH}/game_features.h
    ${SOURCE_PATH}/game_server.cpp
    ${HEADER_PATH}/game_server.h
    ${SOURCE_PATH}/game_controller.cpp
    ${HEADER_PATH}/game_controller.h
    ${SOURCE_PATH}/game_scene.cpp
    ${HEADER_PATH}/game_scene.h
    ${HEADER_PATH}/game_types.h
)
SOURCE_GROUP("system\\app" FILES ${GAMEAPP_LIST})

SET(GUI_FEATURES_LIST
    ${HEADER_PATH}/gui_feature.h
    ${SOURCE_PATH}/gui_feature.cpp
    ${HEADER_PATH}/gui_behaviour_component.h
    ${SOURCE_PATH}/gui_behaviour_component.cpp
    ${HEADER_PATH}/gui_button_component.h
    ${SOURCE_PATH}/gui_button_component.cpp
    ${HEADER_PATH}/gui_camera_component.h
    ${SOURCE_PATH}/gui_camera_component.cpp
    ${HEADER_PATH}/gui_widget_component.h
    ${SOURCE_PATH}/gui_widget_component.cpp
    ${HEADER_PATH}/gui_layout_component.h
    ${SOURCE_PATH}/gui_layout_component.cpp
    ${HEADER_PATH}/gui_label_component.h
    ${SOURCE_PATH}/gui_label_component.cpp
    ${HEADER_PATH}/gui_window_component.h
    ${SOURCE_PATH}/gui_window_component.cpp
    ${HEADER_PATH}/gui_input_manager.h
    ${SOURCE_PATH}/gui_input_manager.cpp
)
SOURCE_GROUP("system\\gui" FILES ${GUI_FEATURES_LIST})

SET(RENDER_FEATURES_LIST
    ${HEADER_PATH}/camera_component.h
    ${SOURCE_PATH}/camera_component.cpp
    ${HEADER_PATH}/lensflare_component.h
    ${SOURCE_PATH}/lensflare_component.cpp
    ${HEADER_PATH}/light_component.h
    ${SOURCE_PATH}/light_component.cpp
    ${HEADER_PATH}/mesh_render_component.h
    ${SOURCE_PATH}/mesh_render_component.cpp
    ${HEADER_PATH}/render_component.h
    ${SOURCE_PATH}/render_component.cpp
    ${HEADER_PATH}/anim_component.h
    ${SOURCE_PATH}/anim_component.cpp
    ${HEADER_PATH}/skinned_mesh_render_component.h
    ${SOURCE_PATH}/skinned_mesh_render_component.cpp
    ${HEADER_PATH}/skybox_component.h
    ${SOURCE_PATH}/skybox_component.cpp
    ${HEADER_PATH}/sprite_component.h
    ${SOURCE_PATH}/sprite_component.cpp
    ${SOURCE_PATH}/mesh_component.cpp
    ${HEADER_PATH}/mesh_component.h
    ${SOURCE_PATH}/render_feature.cpp
    ${HEADER_PATH}/render_feature.h
)
SOURCE_GROUP("system\\renderer" FILES ${RENDER_FEATURES_LIST})

SET(SOUND_FEATURES_LIST
    ${SOURCE_PATH}/sound_component.cpp
    ${HEADER_PATH}/sound_component.h
    ${SOURCE_PATH}/sound_features.cpp
    ${HEADER_PATH}/sound_features.h
)
SOURCE_GROUP("system\\sound" FILES ${SOUND_FEATURES_LIST})

SET(INPUT_FEATURES_LIST
    ${HEADER_PATH}/input_feature.h
    ${SOURCE_PATH}/input_feature.cpp
)
SOURCE_GROUP("system\\input" FILES ${INPUT_FEATURES_LIST})

SET(NETWORK_FEATURES_LIST
    ${SOURCE_PATH}/network_features.cpp
    ${HEADER_PATH}/network_features.h
)
SOURCE_GROUP("system\\network" FILES ${NETWORK_FEATURES_LIST})

SET(PHYSIC_FEATURES_LIST
    ${SOURCE_PATH}/physics_body_component.cpp
    ${HEADER_PATH}/physics_body_component.h
    ${SOURCE_PATH}/physics_box_component.cpp
    ${HEADER_PATH}/physics_box_component.h
    ${SOURCE_PATH}/physics_capsule_component.cpp
    ${HEADER_PATH}/physics_capsule_component.h
    ${SOURCE_PATH}/physics_character_component.cpp
    ${HEADER_PATH}/physics_character_component.h
    ${SOURCE_PATH}/physics_heightmap_component.cpp
    ${HEADER_PATH}/physics_heightmap_component.h
    ${SOURCE_PATH}/physics_mesh_component.cpp
    ${HEADER_PATH}/physics_mesh_component.h
    ${SOURCE_PATH}/physics_shape_component.cpp
    ${HEADER_PATH}/physics_shape_component.h
    ${SOURCE_PATH}/physics_features.cpp
    ${HEADER_PATH}/physics_features.h
)
SOURCE_GROUP("system\\physic" FILES ${PHYSIC_FEATURES_LIST})

SET(SCRIGPU_SYSTEM_LIST
    ${SOURCE_PATH}/script_binder.cpp
    ${HEADER_PATH}/script_binder.h
    ${SOURCE_PATH}/script_bind_actor.cpp
    ${HEADER_PATH}/script_bind_actor.h
    ${SOURCE_PATH}/script_bind_array.cpp
    ${HEADER_PATH}/script_bind_array.h
    ${SOURCE_PATH}/script_bind_display.cpp
    ${HEADER_PATH}/script_bind_display.h
    ${SOURCE_PATH}/script_bind_file.cpp
    ${HEADER_PATH}/script_bind_file.h
    ${SOURCE_PATH}/script_bind_filesystem.cpp
    ${HEADER_PATH}/script_bind_filesystem.h
    ${SOURCE_PATH}/script_bind_input.cpp
    ${HEADER_PATH}/script_bind_input.h
    ${SOURCE_PATH}/script_bind_math.cpp
    ${HEADER_PATH}/script_bind_math.h
    ${SOURCE_PATH}/script_bind_string.cpp
    ${HEADER_PATH}/script_bind_string.h
    ${SOURCE_PATH}/script_bind_timer.cpp
    ${HEADER_PATH}/script_bind_timer.h
    ${SOURCE_PATH}/script_system.cpp
    ${HEADER_PATH}/script_system.h
    ${SOURCE_PATH}/script_object.cpp
    ${HEADER_PATH}/script_object.h
    ${SOURCE_PATH}/script_features.cpp
    ${HEADER_PATH}/script_features.h
    ${SOURCE_PATH}/script_component.cpp
    ${HEADER_PATH}/script_component.h
)
SOURCE_GROUP("system\\script" FILES ${SCRIGPU_SYSTEM_LIST})

ADD_LIBRARY(libplatform SHARED ${PLATFORM_CORE_LIST} ${PLATFORM_DEBUG_LIST} ${PLATFORM_IO_LIST} ${PLATFORM_MATH_LIST})
ADD_LIBRARY(libimage SHARED ${IMAGE_LIST})
ADD_LIBRARY(libmodel SHARED ${MODEL_LIST})
ADD_LIBRARY(libnetwork SHARED ${NETWORK_LIST})
IF(BUILD_SOUND)
    ADD_LIBRARY(libsound SHARED ${SOUND_SYSTEM_LIST})
    TARGET_LINK_LIBRARIES(libsound libplatform OpenAL32 libvorbis)
    IF(MINGW OR UNIX)
        SET_TARGET_PROPERTIES(libsound PROPERTIES OUTPUT_NAME sound)
    ENDIF()
    SET_TARGET_ATTRIBUTE(libsound "core")
ENDIF()

IF(BUILD_INPUT)
    ADD_LIBRARY(libinput SHARED ${INPUT_CORE_LIST} ${INPUT_MSW_LIST} ${INPUT_SDL_LIST})
    TARGET_LINK_LIBRARIES(libinput libplatform)
    IF(MINGW OR UNIX)
        SET_TARGET_PROPERTIES(libinput PROPERTIES OUTPUT_NAME input)
    ENDIF()
    SET_TARGET_ATTRIBUTE(libinput "core")
ENDIF()

IF(BUILD_PHYSIC)
    ADD_LIBRARY(libphysic SHARED ${PHYSICS_SYSTEM_LIST})
    TARGET_LINK_LIBRARIES(libphysic libplatform BulletDynamics BulletCollision BulletSoftBody LinearMath)
    IF(MINGW OR UNIX)
        SET_TARGET_PROPERTIES(libphysic PROPERTIES OUTPUT_NAME physic)
    ENDIF()
    SET_TARGET_ATTRIBUTE(libphysic "core")
ENDIF()

IF(BUILD_GUI)
    ADD_LIBRARY(libgui SHARED ${GUI_LIST} ${MYGUI_LIST})
    TARGET_LINK_LIBRARIES(libgui libplatform librenderer MyGUIEngine glew)
    IF(MINGW OR UNIX)
        SET_TARGET_PROPERTIES(libgui PROPERTIES OUTPUT_NAME gui)
    ENDIF()
    SET_TARGET_ATTRIBUTE(libgui "core")
ENDIF()

IF(BUILD_RENDERER)
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

        ${RENDERER_SYSTEM}
    )
    TARGET_LINK_LIBRARIES(librenderer libplatform libmodel libimage freetype)
    SET_TARGET_ATTRIBUTE(librenderer "core")
    SET_TARGET_PROPERTIES(librenderer PROPERTIES OUTPUT_NAME renderer)
ENDIF()

ADD_LIBRARY(ray SHARED
    ${GAMEAPP_LIST}
    ${GUI_FEATURES_LIST}
    ${RENDER_FEATURES_LIST}
    ${SOUND_FEATURES_LIST}
    ${INPUT_FEATURES_LIST}
    ${PHYSIC_FEATURES_LIST}
    ${NETWORK_FEATURES_LIST}
    ${SCRIGPU_SYSTEM_LIST}
)

TARGET_LINK_LIBRARIES(libplatform libiconv tinyxml)
TARGET_LINK_LIBRARIES(libimage libplatform libpng libjpeg zlib unzip)
TARGET_LINK_LIBRARIES(libmodel libplatform libiconv)
TARGET_LINK_LIBRARIES(ray libplatform libimage libinput libmodel libnetwork librenderer)

IF(BUILD_PLATFORM_WINDOWS)
    TARGET_LINK_LIBRARIES(libplatform dbghelp)
    TARGET_LINK_LIBRARIES(libplatform psapi)
    TARGET_LINK_LIBRARIES(libplatform version)

    TARGET_LINK_LIBRARIES(libinput winmm)
    TARGET_LINK_LIBRARIES(libinput imm32)

    TARGET_LINK_LIBRARIES(librenderer glew)

    IF(BUILD_OPENGL_ES)
        TARGET_LINK_LIBRARIES(librenderer libEGL)
        TARGET_LINK_LIBRARIES(librenderer libGLESv2)
    ENDIF()
ELSEIF(BUILD_PLATFORM_ANDROID)
    TARGET_LINK_LIBRARIES(librenderer EGL)
    TARGET_LINK_LIBRARIES(librenderer GLESv2)
    TARGET_LINK_LIBRARIES(librenderer GLESv3)
ELSEIF(BUILD_PLATFORM_LINUX)
    TARGET_LINK_LIBRARIES(librenderer GL)
    TARGET_LINK_LIBRARIES(librenderer X11)
ENDIF()

IF(BUILD_MULTITHREADING)
    TARGET_LINK_LIBRARIES(libphysic BulletMultiThreaded)
ENDIF()

IF(BUILD_SCRIPT)
    TARGET_LINK_LIBRARIES(ray angelscript)
ENDIF()

IF(BUILD_PHYSIC)
    TARGET_LINK_LIBRARIES(ray libphysic)
ENDIF()

IF(BUILD_GUI)
    TARGET_LINK_LIBRARIES(ray libgui)
ENDIF()

IF(BUILD_SOUND)
    TARGET_LINK_LIBRARIES(ray libsound)
ENDIF()

IF(BUILD_PLATFORM_SDL2)
    TARGET_LINK_LIBRARIES(ray SDL2)
ENDIF()

SET_TARGET_ATTRIBUTE(libplatform "core")
SET_TARGET_ATTRIBUTE(libimage "core")
SET_TARGET_ATTRIBUTE(libmodel "core")
SET_TARGET_ATTRIBUTE(libnetwork "core")
SET_TARGET_ATTRIBUTE(ray "engine")

IF(MINGW OR UNIX)
    SET_TARGET_PROPERTIES(libplatform PROPERTIES OUTPUT_NAME platform)
    SET_TARGET_PROPERTIES(libimage PROPERTIES OUTPUT_NAME image)
    SET_TARGET_PROPERTIES(libmodel PROPERTIES OUTPUT_NAME model)
    SET_TARGET_PROPERTIES(libnetwork PROPERTIES OUTPUT_NAME network)
ENDIF()