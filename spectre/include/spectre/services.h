#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <flecs.h>
#include "../components.h"

// ==============================================================================
// Scenes Module
// ==============================================================================
sandbox_properties_handle_t spectre_scenes_serialize_state(ecs_world_t* world, ecs_entity_t state);
ecs_entity_t spectre_scenes_deserialize_state(ecs_world_t* world, sandbox_properties_handle_t props);
sandbox_properties_handle_t spectre_scenes_serialize_scene(ecs_world_t* world, ecs_entity_t scene);
ecs_entity_t spectre_scenes_deserialize_scene(ecs_world_t* world, sandbox_properties_handle_t props);
void spectre_scenes_register_state(ecs_world_t* world, sandbox_properties_handle_t props);
void spectre_scenes_register_scene(ecs_world_t* world, sandbox_properties_handle_t props);
ecs_entity_t spectre_scenes_find_state(ecs_world_t* world, const char* name);
ecs_entity_t spectre_scenes_find_scene(ecs_world_t* world, const char* name);
bool spectre_scenes_has_state(ecs_world_t* world, const char* name);
bool spectre_scenes_has_scene(ecs_world_t* world, const char* name);
bool spectre_scenes_is_state(ecs_world_t* world, ecs_entity_t entity);
bool spectre_scenes_is_scene(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_scenes_find_current_state(ecs_world_t* world);
ecs_query_t* spectre_scenes_find_current_scenes(ecs_world_t* world);
void spectre_scenes_push_state(ecs_world_t* world, ecs_entity_t state);
void spectre_scenes_pop_state(ecs_world_t* world);

// ==============================================================================
// Components / Prefabs Module
// ==============================================================================
sandbox_properties_handle_t spectre_components_serialize_entity(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_components_deserialize_entity(ecs_world_t* world, sandbox_properties_handle_t props);
void spectre_components_register_prefab(ecs_world_t* world, sandbox_properties_handle_t props);
bool spectre_components_has_prefab(ecs_world_t* world, const char* name);
bool spectre_components_is_prefab(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_components_find_prefab(ecs_world_t* world, const char* name);
ecs_entity_t spectre_components_create_entity_from_props(ecs_world_t* world, sandbox_properties_handle_t props);
ecs_entity_t spectre_components_create_entity_from_prefab(ecs_world_t* world, ecs_entity_t prefab);
ecs_entity_t spectre_components_create_entity_from_name(ecs_world_t* world, const char* name);

// ==============================================================================
// Serializer Module
// ==============================================================================
void spectre_serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_t* serializer);
bool spectre_serializer_has_serializer(ecs_world_t* world, const char* type);
ecs_entity_t spectre_serializer_find_serializer(ecs_world_t* world, const char* type);
sandbox_properties_handle_t spectre_serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);
ecs_entity_t spectre_serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, sandbox_properties_handle_t props);

// ==============================================================================
// Renderer Module
// ==============================================================================
ecs_entity_t spectre_renderer_deserialize_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
sandbox_properties_handle_t spectre_renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer);
void spectre_renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
bool spectre_renderer_is_renderer(ecs_world_t* world);

// ==============================================================================
// Resource Module
// ==============================================================================
ecs_entity_t spectre_resource_deserialize_resource(ecs_world_t* world, sandbox_properties_handle_t props);
sandbox_properties_handle_t spectre_resource_serialize_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
void spectre_resource_register_resource_loader(ecs_world_t* world, const char* type, spectre_resource_loader_t loader);
void spectre_resource_register_resource(ecs_world_t* world, sandbox_properties_handle_t props);
bool spectre_resource_has_resource_loader(ecs_world_t* world, const char* type);
bool spectre_resource_has_resource(ecs_world_t* world, const char* name);
bool spectre_resource_is_resource(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_resource_find_resource_loader(ecs_world_t* world, const char* type);
ecs_entity_t spectre_resource_find_resource(ecs_world_t* world, const char* name);
bool spectre_resource_is_resource_loaded(ecs_world_t* world, ecs_entity_t resource);
void spectre_resource_load_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
void spectre_resource_free_resource(ecs_world_t* world, ecs_entity_t resourceEntity);

// ==============================================================================
// Scripts Module
// ==============================================================================
sandbox_properties_handle_t spectre_script_serialize_script(ecs_world_t* world, ecs_entity_t script);
ecs_entity_t spectre_script_deserialize_script(ecs_world_t* world, sandbox_properties_handle_t props);
void spectre_script_register_script(ecs_world_t* world, sandbox_properties_handle_t props);
bool spectre_script_has_script(ecs_world_t* world, const char* name);
bool spectre_script_is_script(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_script_find_script(ecs_world_t* world, const char* name);
void spectre_script_include_code(ecs_world_t* world, const char* path);
void spectre_script_execute_script(ecs_world_t* world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count);

// ==============================================================================
// Window Module
// ==============================================================================
ecs_entity_t spectre_window_deserialize_window(ecs_world_t* world, sandbox_properties_handle_t props);
sandbox_properties_handle_t spectre_window_serialize_window(ecs_world_t* world, ecs_entity_t window);
void spectre_window_register_window(ecs_world_t* world, sandbox_properties_handle_t props);
bool spectre_window_should_close(ecs_world_t* world);
void spectre_window_set_should_close(ecs_world_t* world, bool close);
void spectre_window_set_size(ecs_world_t* world, uint32_t width, uint32_t height);
void spectre_window_set_position(ecs_world_t* world, int32_t x, int32_t y);
void spectre_window_set_size_limits(ecs_world_t* world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);
uint32_t spectre_window_get_width(ecs_world_t* world);
uint32_t spectre_window_get_height(ecs_world_t* world);
float spectre_window_get_aspect_ratio(ecs_world_t* world);
int32_t spectre_window_get_position_x(ecs_world_t* world);
int32_t spectre_window_get_position_y(ecs_world_t* world);
void spectre_window_set_title(ecs_world_t* world, const char* title);
void spectre_window_set_vsync(ecs_world_t* world, bool enabled);
void spectre_window_set_fullscreen(ecs_world_t* world, bool fullscreen);
void spectre_window_set_borderless(ecs_world_t* world, bool borderless);
void spectre_window_set_resizable(ecs_world_t* world, bool resizable);
void spectre_window_set_always_on_top(ecs_world_t* world, bool always_on_top);
const char* spectre_window_get_title(ecs_world_t* world);
bool spectre_window_is_vsync(ecs_world_t* world);
bool spectre_window_is_fullscreen(ecs_world_t* world);
bool spectre_window_is_borderless(ecs_world_t* world);
bool spectre_window_is_resizable(ecs_world_t* world);
void spectre_window_minimize(ecs_world_t* world);
void spectre_window_maximize(ecs_world_t* world);
void spectre_window_restore(ecs_world_t* world);
void spectre_window_show(ecs_world_t* world);
void spectre_window_hide(ecs_world_t* world);
void spectre_window_request_attention(ecs_world_t* world);
bool spectre_window_is_minimized(ecs_world_t* world);
bool spectre_window_is_maximized(ecs_world_t* world);
bool spectre_window_is_visible(ecs_world_t* world);
bool spectre_window_is_focused(ecs_world_t* world);
bool spectre_window_is_hovered(ecs_world_t* world);
void spectre_window_set_cursor_visible(ecs_world_t* world, bool visible);
void spectre_window_set_cursor_locked(ecs_world_t* world, bool locked);
bool spectre_window_is_cursor_visible(ecs_world_t* world);
bool spectre_window_is_cursor_locked(ecs_world_t* world);
void* spectre_window_get_native_handle(ecs_world_t* world);
bool spectre_window_is_key_down(ecs_world_t* world, int keycode);
bool spectre_window_is_key_pressed(ecs_world_t* world, int keycode);
bool spectre_window_is_key_released(ecs_world_t* world, int keycode);
float spectre_window_get_mouse_x(ecs_world_t* world);
float spectre_window_get_mouse_y(ecs_world_t* world);
float spectre_window_get_mouse_delta_x(ecs_world_t* world);
float spectre_window_get_mouse_delta_y(ecs_world_t* world);

#ifdef __cplusplus
}
#endif