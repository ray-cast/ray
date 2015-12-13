SET(HEADER_PATH ${PROJECT_SOURCE_DIR}/include/ray)
SET(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source/engine)

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
    ${SOURCE_PATH}/game_scene.cpp
    ${HEADER_PATH}/game_scene.h
    ${HEADER_PATH}/game_types.h
)
SOURCE_GROUP("system\\app" FILES ${GAMEAPP_LIST})

SET(GUI_FEATURES_LIST
    ${HEADER_PATH}/gui_feature.h
    ${SOURCE_PATH}/gui_feature.cpp
    ${HEADER_PATH}/gui_listbox_component.h
    ${SOURCE_PATH}/gui_listbox_component.cpp
    ${HEADER_PATH}/gui_combobox_component.h
    ${SOURCE_PATH}/gui_combobox_component.cpp
    ${HEADER_PATH}/gui_editbox_component.h
    ${SOURCE_PATH}/gui_editbox_component.cpp
    ${HEADER_PATH}/gui_button_component.h
    ${SOURCE_PATH}/gui_button_component.cpp
    ${HEADER_PATH}/gui_menu_component.h
    ${SOURCE_PATH}/gui_menu_component.cpp
    ${HEADER_PATH}/gui_menuitem_component.h
    ${SOURCE_PATH}/gui_menuitem_component.cpp
    ${HEADER_PATH}/gui_camera_component.h
    ${SOURCE_PATH}/gui_camera_component.cpp
    ${HEADER_PATH}/gui_widget_component.h
    ${SOURCE_PATH}/gui_widget_component.cpp
    ${HEADER_PATH}/gui_scrollbar_component.h
    ${SOURCE_PATH}/gui_scrollbar_component.cpp
    ${HEADER_PATH}/gui_scrollview_component.h
    ${SOURCE_PATH}/gui_scrollview_component.cpp
    ${HEADER_PATH}/gui_label_component.h
    ${SOURCE_PATH}/gui_label_component.cpp
    ${HEADER_PATH}/gui_window_component.h
    ${SOURCE_PATH}/gui_window_component.cpp
    ${HEADER_PATH}/gui_image_component.h
    ${SOURCE_PATH}/gui_image_component.cpp
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
    ${SOURCE_PATH}/sound_feature.cpp
    ${HEADER_PATH}/sound_feature.h
    ${SOURCE_PATH}/sound_component.cpp
    ${HEADER_PATH}/sound_component.h
    ${SOURCE_PATH}/sound_listener_component.cpp
    ${HEADER_PATH}/sound_listener_component.h
)
SOURCE_GROUP("system\\sound" FILES ${SOUND_FEATURES_LIST})

SET(INPUT_FEATURES_LIST
    ${HEADER_PATH}/input_feature.h
    ${SOURCE_PATH}/input_feature.cpp
)
SOURCE_GROUP("system\\input" FILES ${INPUT_FEATURES_LIST})

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

TARGET_LINK_LIBRARIES(ray libplatform libimage libinput libmodel)

IF(BUILD_SCRIPT)
    TARGET_LINK_LIBRARIES(ray angelscript pthread)
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

IF(BUILD_RENDERER)
    TARGET_LINK_LIBRARIES(ray librenderer)
ENDIF()

IF(BUILD_PLATFORM_SDL2)
    TARGET_LINK_LIBRARIES(ray SDL2)
ENDIF()

SET_TARGET_ATTRIBUTE(ray "engine")