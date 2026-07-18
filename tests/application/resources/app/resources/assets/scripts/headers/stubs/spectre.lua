---@meta

local ffi = {}
ffi.C = {}
function ffi.cdef(def) end
function ffi.new(ct, ...) end
function ffi.cast(ct, val) end
function ffi.string(ptr, len) end

---@class spectre_serializer_component
---@field props) any
---@field entity) any
local spectre_serializer_component = {}

---@class spectre_serializer_relation_t
---@field dummy any
local spectre_serializer_relation_t = {}

---@class spectre_resource_component_t
---@field path string
---@field instance any
local spectre_resource_component_t = {}

---@class spectre_resource_loader_component_t
---@field resource) any
---@field resource) any
local spectre_resource_loader_component_t = {}

---@class spectre_disable_rendering_t
---@field dummy any
local spectre_disable_rendering_t = {}

---@class spectre_state_t
---@field dummy any
local spectre_state_t = {}

---@class spectre_scene_t
---@field dummy any
local spectre_scene_t = {}

---@class spectre_state_use_scene_relation_t
---@field layer_index number
local spectre_state_use_scene_relation_t = {}

---@class spectre_color_t
---@field a any
local spectre_color_t = {}

---@class spectre_renderable_t
---@field dummy any
local spectre_renderable_t = {}

---@class spectre_2D_transform_component_t
---@field position_z any
---@field scale_y any
---@field origin_y any
---@field rotation number
local spectre_2D_transform_component_t = {}

---@class spectre_rectange_renderable_t
---@field width number
---@field height number
---@field fill_color any
---@field outline_color any
---@field outline_thickness number
local spectre_rectange_renderable_t = {}

---@class spectre_circle_renderable_t
---@field radius number
---@field fill_color any
---@field outline_color any
---@field outline_thickness number
local spectre_circle_renderable_t = {}

---@class spectre_polygone_renderable_t
---@field radius number
---@field point_count number
---@field fill_color any
---@field outline_color any
---@field outline_thickness number
local spectre_polygone_renderable_t = {}

---@class spectre_custom_polygone_renderable_t
---@field vertices any
---@field vertex_count number
---@field fill_color any
---@field outline_color any
---@field outline_thickness number
local spectre_custom_polygone_renderable_t = {}

---@class spectre_ligne_renderable_t
---@field position_y2 any
---@field color any
---@field thickness number
local spectre_ligne_renderable_t = {}

---@class spectre_script_t
---@field function_name string
---@field arguments_name string
---@field argument_types any
---@field argument_count number
local spectre_script_t = {}

---@class spectre_use_script_on_enter_relation_t
---@field arguments any
---@field argument_count number
local spectre_use_script_on_enter_relation_t = {}

---@class spectre_use_script_on_exit_relation_t
---@field arguments any
---@field argument_count number
local spectre_use_script_on_exit_relation_t = {}

---@class spectre_use_script_on_create_relation_t
---@field arguments any
---@field argument_count number
local spectre_use_script_on_create_relation_t = {}

---@class spectre_use_script_on_destroy_relation_t
---@field arguments any
---@field argument_count number
local spectre_use_script_on_destroy_relation_t = {}

---@class spectre_use_script_on_update_relation_t
---@field arguments any
---@field argument_count number
local spectre_use_script_on_update_relation_t = {}

---@class spectre_use_script_on_render_relation_t
---@field arguments any
---@field argument_count number
local spectre_use_script_on_render_relation_t = {}

---@class spectre_input_state_t
---@field keys_down any
---@field keys_pressed any
---@field keys_released any
---@field mouse_position any
---@field mouse_delta any
local spectre_input_state_t = {}

---@class spectre_window_component_t
---@field width number
---@field height number
---@field position_x number
---@field position_y number
---@field min_width number
---@field min_height number
---@field max_width number
---@field max_height number
---@field title string
---@field vsync boolean
---@field fullscreen boolean
---@field borderless boolean
---@field resizable boolean
---@field always_on_top boolean
---@field minimized boolean
---@field maximized boolean
---@field visible boolean
---@field focused boolean
---@field hovered boolean
---@field cursor_visible boolean
---@field cursor_locked boolean
---@field native_handle any
local spectre_window_component_t = {}

---@class spectre_prefab_flag_t
---@field dummy any
local spectre_prefab_flag_t = {}

---@class spectre_prefabs_api_t
---@field entity) any
---@field props) any
---@field props) any
---@field name) string
---@field entity) any
---@field name) string
---@field props) any
---@field prefab) any
---@field name) string
local spectre_prefabs_api_t = {}

---@class spectre_prefabs_service_t
---@field api any
---@field info any
local spectre_prefabs_service_t = {}

---@class spectre_renderer_api_t
---@field props) any
---@field renderer) any
---@field props) any
---@field world) any
local spectre_renderer_api_t = {}

---@class spectre_renderer_service_t
---@field api any
---@field info any
local spectre_renderer_service_t = {}

---@class spectre_resources_api_t
---@field props) any
---@field resourceEntity) any
---@field loader) any
---@field props) any
---@field type) string
---@field name) string
---@field entity) any
---@field type) string
---@field name) string
---@field resource) any
---@field resourceEntity) any
---@field resourceEntity) any
---@field resourceEntity) any
local spectre_resources_api_t = {}

---@class spectre_resources_service_t
---@field api any
---@field info any
local spectre_resources_service_t = {}

---@class spectre_scenes_api_t
---@field state) any
---@field props) any
---@field scene) any
---@field props) any
---@field props) any
---@field props) any
---@field name) string
---@field name) string
---@field name) string
---@field name) string
---@field entity) any
---@field entity) any
---@field world) any
---@field world) any
---@field state) any
---@field world) any
---@field payload) any
local spectre_scenes_api_t = {}

---@class spectre_scenes_service_t
---@field api any
---@field info any
local spectre_scenes_service_t = {}

---@class spectre_scripts_api_t
---@field arg_count) any
---@field entity) any
---@field function_name) string
---@field path) string
---@field arg_count) any
---@field entity) any
---@field props) any
---@field entity) any
---@field entity) any
---@field entity) any
---@field entity) any
---@field entity) any
local spectre_scripts_api_t = {}

---@class spectre_scripts_service_t
---@field api any
---@field info any
local spectre_scripts_service_t = {}

---@class spectre_serializer_api_t
---@field serializer) string
---@field type) string
---@field entity) any
---@field type) string
---@field entity) any
---@field props) any
local spectre_serializer_api_t = {}

---@class spectre_serializer_service_t
---@field api any
---@field info any
local spectre_serializer_service_t = {}

---@class spectre_window_api_t
---@field props) any
---@field window) any
---@field props) any
---@field world) any
---@field close) any
---@field height) number
---@field y) number
---@field max_height) number
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field title) string
---@field enabled) any
---@field fullscreen) any
---@field borderless) any
---@field resizable) any
---@field always_on_top) any
---@field world) string
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field world) any
---@field visible) any
---@field locked) any
---@field world) any
---@field world) any
---@field world) any
---@field keycode) number
---@field keycode) number
---@field keycode) number
---@field world) any
---@field world) any
---@field world) any
---@field world) any
local spectre_window_api_t = {}

---@class spectre_window_service_t
---@field api any
---@field info any
local spectre_window_service_t = {}

---@param world any
---@param entity any
---@return any
function ffi.C.spectre_prefabs_serialize_entity(world, entity) end

---@param world any
---@param target any
---@param props any
function ffi.C.spectre_prefabs_deserialize_entity(world, target, props) end

---@param world any
---@param name string
---@param props any
function ffi.C.spectre_prefabs_register_prefab(world, name, props) end

---@param world any
---@param name string
---@return boolean
function ffi.C.spectre_prefabs_has_prefab(world, name) end

---@param world any
---@param entity any
---@return boolean
function ffi.C.spectre_prefabs_is_prefab(world, entity) end

---@param world any
---@param name string
---@return any
function ffi.C.spectre_prefabs_find_prefab(world, name) end

---@param world any
---@param props any
---@return any
function ffi.C.spectre_prefabs_create_entity_from_props(world, props) end

---@param world any
---@param prefab any
---@return any
function ffi.C.spectre_prefabs_create_entity_from_prefab(world, prefab) end

---@param world any
---@param name string
---@return any
function ffi.C.spectre_prefabs_create_entity_from_name(world, name) end

---@param world any
---@param target any
---@param props any
function ffi.C.spectre_renderer_deserialize_renderer(world, target, props) end

---@param world any
---@param renderer any
---@return any
function ffi.C.spectre_renderer_serialize_renderer(world, renderer) end

---@param world any
---@param props any
function ffi.C.spectre_renderer_register_renderer(world, props) end

---@param world any
---@return boolean
function ffi.C.spectre_renderer_is_renderer(world) end

---@param world any
---@param target any
---@param props any
function ffi.C.spectre_resources_deserialize_resource(world, target, props) end

---@param world any
---@param resourceEntity any
---@return any
function ffi.C.spectre_resources_serialize_resource(world, resourceEntity) end

---@param world any
---@param type_ string
---@param loader any
function ffi.C.spectre_resources_register_resource_loader(world, type_, loader) end

---@param world any
---@param props any
function ffi.C.spectre_resources_register_resource(world, props) end

---@param world any
---@param type_ string
---@return boolean
function ffi.C.spectre_resources_has_resource_loader(world, type_) end

---@param world any
---@param name string
---@return boolean
function ffi.C.spectre_resources_has_resource(world, name) end

---@param world any
---@param entity any
---@return boolean
function ffi.C.spectre_resources_is_resource(world, entity) end

---@param world any
---@param type_ string
---@return any
function ffi.C.spectre_resources_find_resource_loader(world, type_) end

---@param world any
---@param name string
---@return any
function ffi.C.spectre_resources_find_resource(world, name) end

---@param world any
---@param resource any
---@return boolean
function ffi.C.spectre_resources_is_resource_loaded(world, resource) end

---@param world any
---@param resourceEntity any
function ffi.C.spectre_resources_load_resource(world, resourceEntity) end

---@param world any
---@param resourceEntity any
function ffi.C.spectre_resources_free_resource(world, resourceEntity) end

---@param world any
---@param resourceEntity any
---@return any
function ffi.C.spectre_resources_get_resource(world, resourceEntity) end

---@param world any
---@param state any
---@return any
function ffi.C.spectre_scenes_serialize_state(world, state) end

---@param world any
---@param target any
---@param props any
function ffi.C.spectre_scenes_deserialize_state(world, target, props) end

---@param world any
---@param scene any
---@return any
function ffi.C.spectre_scenes_serialize_scene(world, scene) end

---@param world any
---@param target any
---@param props any
function ffi.C.spectre_scenes_deserialize_scene(world, target, props) end

---@param world any
---@param props any
function ffi.C.spectre_scenes_register_state(world, props) end

---@param world any
---@param props any
function ffi.C.spectre_scenes_register_scene(world, props) end

---@param world any
---@param name string
---@return any
function ffi.C.spectre_scenes_find_state(world, name) end

---@param world any
---@param name string
---@return any
function ffi.C.spectre_scenes_find_scene(world, name) end

---@param world any
---@param name string
---@return boolean
function ffi.C.spectre_scenes_has_state(world, name) end

---@param world any
---@param name string
---@return boolean
function ffi.C.spectre_scenes_has_scene(world, name) end

---@param world any
---@param entity any
---@return boolean
function ffi.C.spectre_scenes_is_state(world, entity) end

---@param world any
---@param entity any
---@return boolean
function ffi.C.spectre_scenes_is_scene(world, entity) end

---@param world any
---@return any
function ffi.C.spectre_scenes_find_current_state(world) end

---@param world any
---@return any
function ffi.C.spectre_scenes_find_current_scenes(world) end

---@param world any
---@param state any
function ffi.C.spectre_scenes_push_state(world, state) end

---@param world any
function ffi.C.spectre_scenes_pop_state(world) end

---@param world any
---@param entity any
---@param callback any
---@param payload any
function ffi.C.spectre_scenes_execute_recursive(world, entity, callback, payload) end

---@param world any
---@param function_name string
---@param arg_types any
---@param arg_count number
---@return boolean
function ffi.C.spectre_scripts_has_script(world, function_name, arg_types, arg_count) end

---@param world any
---@param entity any
---@return boolean
function ffi.C.spectre_scripts_is_script(world, entity) end

---@param world any
---@param function_name string
---@return any
function ffi.C.spectre_scripts_find_script(world, function_name) end

---@param world any
---@param path string
function ffi.C.spectre_scripts_include_code(world, path) end

---@param world any
---@param function_name string
---@param args any
---@param arg_count number
function ffi.C.spectre_scripts_execute_script(world, function_name, args, arg_count) end

---@param world any
---@param entity any
---@return any
function ffi.C.spectre_scripts_serialize_scripts(world, entity) end

---@param world any
---@param target any
---@param props any
function ffi.C.spectre_scripts_deserialize_scripts(world, target, props) end

---@param world any
---@param entity any
function ffi.C.spectre_scripts_execute_on_create(world, entity) end

---@param world any
---@param entity any
function ffi.C.spectre_scripts_execute_on_destroy(world, entity) end

---@param world any
---@param entity any
function ffi.C.spectre_scripts_execute_on_update(world, entity) end

---@param world any
---@param entity any
function ffi.C.spectre_scripts_execute_on_enter(world, entity) end

---@param world any
---@param entity any
function ffi.C.spectre_scripts_execute_on_exit(world, entity) end

---@param world any
---@param type_ string
---@param serializer any
function ffi.C.spectre_serializer_register_serializer(world, type_, serializer) end

---@param world any
---@param type_ string
---@return boolean
function ffi.C.spectre_serializer_has_serializer(world, type_) end

---@param world any
---@param entity any
---@return boolean
function ffi.C.spectre_serializer_is_serializer(world, entity) end

---@param world any
---@param type_ string
---@return any
function ffi.C.spectre_serializer_find_serializer(world, type_) end

---@param world any
---@param serializer any
---@param entity any
---@return any
function ffi.C.spectre_serializer_serialize_entity(world, serializer, entity) end

---@param world any
---@param serializer any
---@param entity any
---@param props any
function ffi.C.spectre_serializer_deserialize_entity(world, serializer, entity, props) end

---@param world any
---@param target any
---@param props any
function ffi.C.spectre_window_deserialize_window(world, target, props) end

---@param world any
---@param window any
---@return any
function ffi.C.spectre_window_serialize_window(world, window) end

---@param world any
---@param props any
function ffi.C.spectre_window_register_window(world, props) end

---@param world any
---@return boolean
function ffi.C.spectre_window_should_close(world) end

---@param world any
---@param close boolean
function ffi.C.spectre_window_set_should_close(world, close) end

---@param world any
---@param width number
---@param height number
function ffi.C.spectre_window_set_size(world, width, height) end

---@param world any
---@param x number
---@param y number
function ffi.C.spectre_window_set_position(world, x, y) end

---@param world any
---@param min_width number
---@param min_height number
---@param max_width number
---@param max_height number
function ffi.C.spectre_window_set_size_limits(world, min_width, min_height, max_width, max_height) end

---@param world any
---@return number
function ffi.C.spectre_window_get_width(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_height(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_aspect_ratio(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_position_x(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_position_y(world) end

---@param world any
---@param title string
function ffi.C.spectre_window_set_title(world, title) end

---@param world any
---@param enabled boolean
function ffi.C.spectre_window_set_vsync(world, enabled) end

---@param world any
---@param fullscreen boolean
function ffi.C.spectre_window_set_fullscreen(world, fullscreen) end

---@param world any
---@param borderless boolean
function ffi.C.spectre_window_set_borderless(world, borderless) end

---@param world any
---@param resizable boolean
function ffi.C.spectre_window_set_resizable(world, resizable) end

---@param world any
---@param always_on_top boolean
function ffi.C.spectre_window_set_always_on_top(world, always_on_top) end

---@param world any
---@return string
function ffi.C.spectre_window_get_title(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_vsync(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_fullscreen(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_borderless(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_resizable(world) end

---@param world any
function ffi.C.spectre_window_minimize(world) end

---@param world any
function ffi.C.spectre_window_maximize(world) end

---@param world any
function ffi.C.spectre_window_restore(world) end

---@param world any
function ffi.C.spectre_window_show(world) end

---@param world any
function ffi.C.spectre_window_hide(world) end

---@param world any
function ffi.C.spectre_window_request_attention(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_minimized(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_maximized(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_visible(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_focused(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_hovered(world) end

---@param world any
---@param visible boolean
function ffi.C.spectre_window_set_cursor_visible(world, visible) end

---@param world any
---@param locked boolean
function ffi.C.spectre_window_set_cursor_locked(world, locked) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_cursor_visible(world) end

---@param world any
---@return boolean
function ffi.C.spectre_window_is_cursor_locked(world) end

---@param world any
---@return any
function ffi.C.spectre_window_get_native_handle(world) end

---@param world any
---@param keycode number
---@return boolean
function ffi.C.spectre_window_is_key_down(world, keycode) end

---@param world any
---@param keycode number
---@return boolean
function ffi.C.spectre_window_is_key_pressed(world, keycode) end

---@param world any
---@param keycode number
---@return boolean
function ffi.C.spectre_window_is_key_released(world, keycode) end

---@param world any
---@return number
function ffi.C.spectre_window_get_mouse_x(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_mouse_y(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_mouse_delta_x(world) end

---@param world any
---@return number
function ffi.C.spectre_window_get_mouse_delta_y(world) end

return ffi