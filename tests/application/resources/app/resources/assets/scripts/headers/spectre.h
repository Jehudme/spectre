typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef struct ecs_world_t ecs_world_t;
typedef uint64_t ecs_entity_t;
typedef struct ecs_query_t ecs_query_t;
typedef struct sandbox_properties_t sandbox_properties_t;
typedef sandbox_properties_t* sandbox_properties_handle_t;
typedef struct sandbox_service_info_t sandbox_service_info_t;

typedef struct spectre_serializer_component {
void (*deserialize)(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t props);
sandbox_properties_handle_t (*serialize)(ecs_world_t* world, ecs_entity_t entity);
} spectre_serializer_component;
typedef struct spectre_serializer_relation_t {
char dummy;
} spectre_serializer_relation_t;
typedef struct spectre_use_loader_relation_t { char dummy; } spectre_resource_loader_relation;
typedef struct spectre_use_resource_relation_t { char dummy; } spectre_use_resource_relation;
typedef struct spectre_resource_loaded_flag_t { char dummy; } spectre_resource_flag_t;
typedef struct spectre_resource_component_t {
const char* path;
void* instance;
} spectre_resource_component_t;
typedef struct spectre_resource_loader_component_t {
void (*load_fn)(ecs_world_t* world, spectre_resource_component_t* resource);
void (*free_fn)(ecs_world_t* world, spectre_resource_component_t* resource);
} spectre_resource_loader_component_t;
typedef struct spectre_disable_rendering_t {
char dummy;
} spectre_disable_rendering_t;
typedef struct spectre_state_t { char dummy; } spectre_state_t;
typedef struct spectre_scene_t { char dummy; } spectre_scene_t;
typedef struct spectre_state_use_scene_relation_t { int layer_index; } spectre_state_use_scene_relation_t;
typedef void (*spectre_recursive_callback_t)(ecs_world_t* world, ecs_entity_t entity, void* payload);
typedef struct spectre_color_t {
float r, g, b, a;
} spectre_color_t;
typedef struct spectre_renderable_t {
char dummy;
} spectre_renderable_t;
typedef struct spectre_2D_transform_component_t {
float position_x, position_y, position_z;
float scale_x, scale_y;
float origin_x, origin_y;
float rotation;
} spectre_2D_transform_component_t;
typedef struct spectre_rectange_renderable_t {
float width;
float height;
spectre_color_t fill_color;
spectre_color_t outline_color;
float outline_thickness;
} spectre_rectange_renderable_t;
typedef struct spectre_circle_renderable_t {
float radius;
spectre_color_t fill_color;
spectre_color_t outline_color;
float outline_thickness;
} spectre_circle_renderable_t;
typedef struct spectre_polygone_renderable_t {
float radius;
uint32_t point_count;
spectre_color_t fill_color;
spectre_color_t outline_color;
float outline_thickness;
} spectre_polygone_renderable_t;
typedef struct spectre_custom_polygone_renderable_t {
float* vertices;
uint32_t vertex_count;
spectre_color_t fill_color;
spectre_color_t outline_color;
float outline_thickness;
} spectre_custom_polygone_renderable_t;
typedef struct spectre_ligne_renderable_t {
double position_x1, position_y1, position_x2, position_y2;
spectre_color_t color;
float thickness;
} spectre_ligne_renderable_t;
typedef enum spectre_script_argument_type_t {
SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL,
SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN,
SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER,
SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER,
SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING,
SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE,
SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA,
SPECTRE_SCRIPT_ARGUMENT_TYPE_ENTITY
} spectre_script_argument_type_t;
typedef struct spectre_script_argument_t {
spectre_script_argument_type_t type;
union {
_Bool
boolean_value;
double number_value;
long long integer_value;
const char* string_value;
void* table_pointer;
void* userdata_pointer;
const char* entity;
} value;
} spectre_script_argument_t;
typedef struct spectre_script_t {
const char* function_name;
const char** arguments_name;
const spectre_script_argument_type_t* argument_types;
uint32_t argument_count;
} spectre_script_t;
typedef struct spectre_use_script_on_enter_relation_t { spectre_script_argument_t* arguments; int argument_count; } spectre_use_script_on_enter_relation_t;
typedef struct spectre_use_script_on_exit_relation_t { spectre_script_argument_t* arguments; int argument_count; } spectre_use_script_on_exit_relation_t;
typedef struct spectre_use_script_on_create_relation_t { spectre_script_argument_t* arguments; int argument_count;} spectre_use_script_on_create_relation_t;
typedef struct spectre_use_script_on_destroy_relation_t { spectre_script_argument_t* arguments; int argument_count; } spectre_use_script_on_destroy_relation_t;
typedef struct spectre_use_script_on_update_relation_t { spectre_script_argument_t* arguments; int argument_count; } spectre_use_script_on_update_relation_t;
typedef struct spectre_use_script_on_render_relation_t { spectre_script_argument_t* arguments; int argument_count; } spectre_use_script_on_render_relation_t;
typedef struct spectre_input_state_t {
uint64_t keys_down[8];
uint64_t keys_pressed[8];
uint64_t keys_released[8];
float mouse_position[2];
float mouse_delta[2];
} spectre_input_state_t;
typedef struct spectre_window_component_t {
uint32_t width;
uint32_t height;
int32_t position_x;
int32_t position_y;
uint32_t min_width;
uint32_t min_height;
uint32_t max_width;
uint32_t max_height;
const char* title;
_Bool
vsync;
_Bool
fullscreen;
_Bool
borderless;
_Bool
resizable;
_Bool
always_on_top;
_Bool
minimized;
_Bool
maximized;
_Bool
visible;
_Bool
focused;
_Bool
hovered;
_Bool
cursor_visible;
_Bool
cursor_locked;
void* native_handle;
} spectre_window_component_t;
typedef struct spectre_component_t { char dummy; } spectre_component_serializer_t;
typedef struct spectre_prefab_flag_t {
char dummy;
} spectre_prefab_flag_t;
typedef struct spectre_prefabs_api_t {
sandbox_properties_handle_t (*serialize_entity)(ecs_world_t* world, ecs_entity_t entity);
void (*deserialize_entity)(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
void (*register_prefab)(ecs_world_t* world, const char* name, sandbox_properties_handle_t props);
_Bool
(*has_prefab)(ecs_world_t* world, const char* name);
_Bool
(*is_prefab)(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t (*find_prefab)(ecs_world_t* world, const char* name);
ecs_entity_t (*create_entity_from_props)(ecs_world_t* world, sandbox_properties_handle_t props);
ecs_entity_t (*create_entity_from_prefab)(ecs_world_t* world, ecs_entity_t prefab);
ecs_entity_t (*create_entity_from_name)(ecs_world_t* world, const char* name);
} spectre_prefabs_api_t;
typedef struct spectre_prefabs_service_t { spectre_prefabs_api_t* api; const sandbox_service_info_t* info; } spectre_prefabs_service_t; extern ecs_entity_t FLECS_IDspectre_prefabs_service_tID_; extern sandbox_service_info_t spectre_prefabs_service_t_info;
;
sandbox_properties_handle_t spectre_prefabs_serialize_entity(ecs_world_t* world, ecs_entity_t entity);
void spectre_prefabs_deserialize_entity(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
void spectre_prefabs_register_prefab(ecs_world_t* world, const char* name, sandbox_properties_handle_t props);
_Bool
spectre_prefabs_has_prefab(ecs_world_t* world, const char* name);
_Bool
spectre_prefabs_is_prefab(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_prefabs_find_prefab(ecs_world_t* world, const char* name);
ecs_entity_t spectre_prefabs_create_entity_from_props(ecs_world_t* world, sandbox_properties_handle_t props);
ecs_entity_t spectre_prefabs_create_entity_from_prefab(ecs_world_t* world, ecs_entity_t prefab);
ecs_entity_t spectre_prefabs_create_entity_from_name(ecs_world_t* world, const char* name);
typedef struct spectre_renderer_api_t {
void (*deserialize_renderer)(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
sandbox_properties_handle_t (*serialize_renderer)(ecs_world_t* world, ecs_entity_t renderer);
void (*register_renderer)(ecs_world_t* world, sandbox_properties_handle_t props);
_Bool
(*is_renderer)(ecs_world_t* world);
} spectre_renderer_api_t;
typedef struct spectre_renderer_service_t { spectre_renderer_api_t* api; const sandbox_service_info_t* info; } spectre_renderer_service_t; extern ecs_entity_t FLECS_IDspectre_renderer_service_tID_; extern sandbox_service_info_t spectre_renderer_service_t_info;
;
void spectre_renderer_deserialize_renderer(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
sandbox_properties_handle_t spectre_renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer);
void spectre_renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
_Bool
spectre_renderer_is_renderer(ecs_world_t* world);
typedef struct spectre_resources_api_t {
void (*deserialize_resource)(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
sandbox_properties_handle_t (*serialize_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
void (*register_resource_loader)(ecs_world_t* world, const char* type, spectre_resource_loader_component_t loader);
void (*register_resource)(ecs_world_t* world, sandbox_properties_handle_t props);
_Bool
(*has_resource_loader)(ecs_world_t* world, const char* type);
_Bool
(*has_resource)(ecs_world_t* world, const char* name);
_Bool
(*is_resource)(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t (*find_resource_loader)(ecs_world_t* world, const char* type);
ecs_entity_t (*find_resource)(ecs_world_t* world, const char* name);
_Bool
(*is_resource_loaded)(ecs_world_t* world, ecs_entity_t resource);
void (*load_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
void (*free_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
void* (*get_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
} spectre_resources_api_t;
typedef struct spectre_resources_service_t { spectre_resources_api_t* api; const sandbox_service_info_t* info; } spectre_resources_service_t; extern ecs_entity_t FLECS_IDspectre_resources_service_tID_; extern sandbox_service_info_t spectre_resources_service_t_info;
;
void spectre_resources_deserialize_resource(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
sandbox_properties_handle_t spectre_resources_serialize_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
void spectre_resources_register_resource_loader(ecs_world_t* world, const char* type, spectre_resource_loader_component_t loader);
void spectre_resources_register_resource(ecs_world_t* world, sandbox_properties_handle_t props);
_Bool
spectre_resources_has_resource_loader(ecs_world_t* world, const char* type);
_Bool
spectre_resources_has_resource(ecs_world_t* world, const char* name);
_Bool
spectre_resources_is_resource(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_resources_find_resource_loader(ecs_world_t* world, const char* type);
ecs_entity_t spectre_resources_find_resource(ecs_world_t* world, const char* name);
_Bool
spectre_resources_is_resource_loaded(ecs_world_t* world, ecs_entity_t resource);
void spectre_resources_load_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
void spectre_resources_free_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
void* spectre_resources_get_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
typedef struct spectre_scenes_api_t {
sandbox_properties_handle_t (*serialize_state)(ecs_world_t* world, ecs_entity_t state);
void (*deserialize_state)(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
sandbox_properties_handle_t (*serialize_scene)(ecs_world_t* world, ecs_entity_t scene);
void (*deserialize_scene)(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
void (*register_state)(ecs_world_t* world, sandbox_properties_handle_t props);
void (*register_scene)(ecs_world_t* world, sandbox_properties_handle_t props);
ecs_entity_t (*find_state)(ecs_world_t* world, const char* name);
ecs_entity_t (*find_scene)(ecs_world_t* world, const char* name);
_Bool
(*has_state)(ecs_world_t* world, const char* name);
_Bool
(*has_scene)(ecs_world_t* world, const char* name);
_Bool
(*is_state)(ecs_world_t* world, ecs_entity_t entity);
_Bool
(*is_scene)(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t (*find_current_state)(ecs_world_t* world);
ecs_query_t* (*find_current_scenes)(ecs_world_t* world);
void (*push_state)(ecs_world_t* world, ecs_entity_t state);
void (*pop_state)(ecs_world_t* world);
void (*execute_recursive)(ecs_world_t* world, ecs_entity_t entity, spectre_recursive_callback_t callback, void* payload);
} spectre_scenes_api_t;
typedef struct spectre_scenes_service_t { spectre_scenes_api_t* api; const sandbox_service_info_t* info; } spectre_scenes_service_t; extern ecs_entity_t FLECS_IDspectre_scenes_service_tID_; extern sandbox_service_info_t spectre_scenes_service_t_info;
;
sandbox_properties_handle_t spectre_scenes_serialize_state(ecs_world_t* world, ecs_entity_t state);
void spectre_scenes_deserialize_state(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
sandbox_properties_handle_t spectre_scenes_serialize_scene(ecs_world_t* world, ecs_entity_t scene);
void spectre_scenes_deserialize_scene(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
void spectre_scenes_register_state(ecs_world_t* world, sandbox_properties_handle_t props);
void spectre_scenes_register_scene(ecs_world_t* world, sandbox_properties_handle_t props);
ecs_entity_t spectre_scenes_find_state(ecs_world_t* world, const char* name);
ecs_entity_t spectre_scenes_find_scene(ecs_world_t* world, const char* name);
_Bool
spectre_scenes_has_state(ecs_world_t* world, const char* name);
_Bool
spectre_scenes_has_scene(ecs_world_t* world, const char* name);
_Bool
spectre_scenes_is_state(ecs_world_t* world, ecs_entity_t entity);
_Bool
spectre_scenes_is_scene(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_scenes_find_current_state(ecs_world_t* world);
ecs_query_t* spectre_scenes_find_current_scenes(ecs_world_t* world);
void spectre_scenes_push_state(ecs_world_t* world, ecs_entity_t state);
void spectre_scenes_pop_state(ecs_world_t* world);
void spectre_scenes_execute_recursive(ecs_world_t* world, ecs_entity_t entity, spectre_recursive_callback_t callback, void* payload);
typedef struct spectre_scripts_api_t {
_Bool
(*has_script)(ecs_world_t* world, const char* function_name, const spectre_script_argument_type_t* arg_types, size_t arg_count);
_Bool
(*is_script)(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t (*find_script)(ecs_world_t* world, const char* function_name);
void (*include_code)(ecs_world_t* world, const char* path);
void (*execute_script)(ecs_world_t* world, const char* function_name, spectre_script_argument_t* args, size_t arg_count);
sandbox_properties_handle_t (*serialize_scripts)(ecs_world_t* world, ecs_entity_t entity);
void (*deserialize_scripts)(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
void (*execute_on_create)(ecs_world_t* world, ecs_entity_t entity);
void (*execute_on_destroy)(ecs_world_t* world, ecs_entity_t entity);
void (*execute_on_update)(ecs_world_t* world, ecs_entity_t entity);
void (*execute_on_enter)(ecs_world_t* world, ecs_entity_t entity);
void (*execute_on_exit)(ecs_world_t* world, ecs_entity_t entity);
} spectre_scripts_api_t;
typedef struct spectre_scripts_service_t { spectre_scripts_api_t* api; const sandbox_service_info_t* info; } spectre_scripts_service_t; extern ecs_entity_t FLECS_IDspectre_scripts_service_tID_; extern sandbox_service_info_t spectre_scripts_service_t_info;
;
_Bool
spectre_scripts_has_script(ecs_world_t* world, const char* function_name, const spectre_script_argument_type_t* arg_types, size_t arg_count);
_Bool
spectre_scripts_is_script(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_scripts_find_script(ecs_world_t* world, const char* function_name);
void spectre_scripts_include_code(ecs_world_t* world, const char* path);
void spectre_scripts_execute_script(ecs_world_t* world, const char* function_name, spectre_script_argument_t* args, size_t arg_count);
sandbox_properties_handle_t spectre_scripts_serialize_scripts(ecs_world_t* world, ecs_entity_t entity);
void spectre_scripts_deserialize_scripts(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
void spectre_scripts_execute_on_create(ecs_world_t* world, ecs_entity_t entity);
void spectre_scripts_execute_on_destroy(ecs_world_t* world, ecs_entity_t entity);
void spectre_scripts_execute_on_update(ecs_world_t* world, ecs_entity_t entity);
void spectre_scripts_execute_on_enter(ecs_world_t* world, ecs_entity_t entity);
void spectre_scripts_execute_on_exit(ecs_world_t* world, ecs_entity_t entity);
typedef struct spectre_serializer_api_t {
void (*register_serializer)(ecs_world_t* world, const char* type, const spectre_serializer_component* serializer);
_Bool
(*has_serializer)(ecs_world_t* world, const char* type);
_Bool
(*is_serializer)(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t (*find_serializer)(ecs_world_t* world, const char* type);
sandbox_properties_handle_t (*serialize_entity)(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);
void (*deserialize_entity)(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity, sandbox_properties_handle_t props);
} spectre_serializer_api_t;
typedef struct spectre_serializer_service_t { spectre_serializer_api_t* api; const sandbox_service_info_t* info; } spectre_serializer_service_t; extern ecs_entity_t FLECS_IDspectre_serializer_service_tID_; extern sandbox_service_info_t spectre_serializer_service_t_info;
;
void spectre_serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_component* serializer);
_Bool
spectre_serializer_has_serializer(ecs_world_t* world, const char* type);
_Bool
spectre_serializer_is_serializer(ecs_world_t* world, ecs_entity_t entity);
ecs_entity_t spectre_serializer_find_serializer(ecs_world_t* world, const char* type);
sandbox_properties_handle_t spectre_serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);
void spectre_serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity, sandbox_properties_handle_t props);
typedef struct spectre_window_api_t {
void (*deserialize_window)(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
sandbox_properties_handle_t (*serialize_window)(ecs_world_t* world, ecs_entity_t window);
void (*register_window)(ecs_world_t* world, sandbox_properties_handle_t props);
_Bool
(*should_close)(ecs_world_t* world);
void (*set_should_close)(ecs_world_t* world,
_Bool
close);
void (*set_size)(ecs_world_t* world, uint32_t width, uint32_t height);
void (*set_position)(ecs_world_t* world, int32_t x, int32_t y);
void (*set_size_limits)(ecs_world_t* world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);
uint32_t (*get_width)(ecs_world_t* world);
uint32_t (*get_height)(ecs_world_t* world);
float (*get_aspect_ratio)(ecs_world_t* world);
int32_t (*get_position_x)(ecs_world_t* world);
int32_t (*get_position_y)(ecs_world_t* world);
void (*set_title)(ecs_world_t* world, const char* title);
void (*set_vsync)(ecs_world_t* world,
_Bool
enabled);
void (*set_fullscreen)(ecs_world_t* world,
_Bool
fullscreen);
void (*set_borderless)(ecs_world_t* world,
_Bool
borderless);
void (*set_resizable)(ecs_world_t* world,
_Bool
resizable);
void (*set_always_on_top)(ecs_world_t* world,
_Bool
always_on_top);
const char* (*get_title)(ecs_world_t* world);
_Bool
(*is_vsync)(ecs_world_t* world);
_Bool
(*is_fullscreen)(ecs_world_t* world);
_Bool
(*is_borderless)(ecs_world_t* world);
_Bool
(*is_resizable)(ecs_world_t* world);
void (*minimize)(ecs_world_t* world);
void (*maximize)(ecs_world_t* world);
void (*restore)(ecs_world_t* world);
void (*show)(ecs_world_t* world);
void (*hide)(ecs_world_t* world);
void (*request_attention)(ecs_world_t* world);
_Bool
(*is_minimized)(ecs_world_t* world);
_Bool
(*is_maximized)(ecs_world_t* world);
_Bool
(*is_visible)(ecs_world_t* world);
_Bool
(*is_focused)(ecs_world_t* world);
_Bool
(*is_hovered)(ecs_world_t* world);
void (*set_cursor_visible)(ecs_world_t* world,
_Bool
visible);
void (*set_cursor_locked)(ecs_world_t* world,
_Bool
locked);
_Bool
(*is_cursor_visible)(ecs_world_t* world);
_Bool
(*is_cursor_locked)(ecs_world_t* world);
void* (*get_native_handle)(ecs_world_t* world);
_Bool
(*is_key_down)(ecs_world_t* world, int keycode);
_Bool
(*is_key_pressed)(ecs_world_t* world, int keycode);
_Bool
(*is_key_released)(ecs_world_t* world, int keycode);
float (*get_mouse_x)(ecs_world_t* world);
float (*get_mouse_y)(ecs_world_t* world);
float (*get_mouse_delta_x)(ecs_world_t* world);
float (*get_mouse_delta_y)(ecs_world_t* world);
} spectre_window_api_t;
typedef struct spectre_window_service_t { spectre_window_api_t* api; const sandbox_service_info_t* info; } spectre_window_service_t; extern ecs_entity_t FLECS_IDspectre_window_service_tID_; extern sandbox_service_info_t spectre_window_service_t_info;
;
void spectre_window_deserialize_window(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
sandbox_properties_handle_t spectre_window_serialize_window(ecs_world_t* world, ecs_entity_t window);
void spectre_window_register_window(ecs_world_t* world, sandbox_properties_handle_t props);
_Bool
spectre_window_should_close(ecs_world_t* world);
void spectre_window_set_should_close(ecs_world_t* world,
_Bool
close);
void spectre_window_set_size(ecs_world_t* world, uint32_t width, uint32_t height);
void spectre_window_set_position(ecs_world_t* world, int32_t x, int32_t y);
void spectre_window_set_size_limits(ecs_world_t* world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);
uint32_t spectre_window_get_width(ecs_world_t* world);
uint32_t spectre_window_get_height(ecs_world_t* world);
float spectre_window_get_aspect_ratio(ecs_world_t* world);
int32_t spectre_window_get_position_x(ecs_world_t* world);
int32_t spectre_window_get_position_y(ecs_world_t* world);
void spectre_window_set_title(ecs_world_t* world, const char* title);
void spectre_window_set_vsync(ecs_world_t* world,
_Bool
enabled);
void spectre_window_set_fullscreen(ecs_world_t* world,
_Bool
fullscreen);
void spectre_window_set_borderless(ecs_world_t* world,
_Bool
borderless);
void spectre_window_set_resizable(ecs_world_t* world,
_Bool
resizable);
void spectre_window_set_always_on_top(ecs_world_t* world,
_Bool
always_on_top);
const char* spectre_window_get_title(ecs_world_t* world);
_Bool
spectre_window_is_vsync(ecs_world_t* world);
_Bool
spectre_window_is_fullscreen(ecs_world_t* world);
_Bool
spectre_window_is_borderless(ecs_world_t* world);
_Bool
spectre_window_is_resizable(ecs_world_t* world);
void spectre_window_minimize(ecs_world_t* world);
void spectre_window_maximize(ecs_world_t* world);
void spectre_window_restore(ecs_world_t* world);
void spectre_window_show(ecs_world_t* world);
void spectre_window_hide(ecs_world_t* world);
void spectre_window_request_attention(ecs_world_t* world);
_Bool
spectre_window_is_minimized(ecs_world_t* world);
_Bool
spectre_window_is_maximized(ecs_world_t* world);
_Bool
spectre_window_is_visible(ecs_world_t* world);
_Bool
spectre_window_is_focused(ecs_world_t* world);
_Bool
spectre_window_is_hovered(ecs_world_t* world);
void spectre_window_set_cursor_visible(ecs_world_t* world,
_Bool
visible);
void spectre_window_set_cursor_locked(ecs_world_t* world,
_Bool
locked);
_Bool
spectre_window_is_cursor_visible(ecs_world_t* world);
_Bool
spectre_window_is_cursor_locked(ecs_world_t* world);
void* spectre_window_get_native_handle(ecs_world_t* world);
_Bool
spectre_window_is_key_down(ecs_world_t* world, int keycode);
_Bool
spectre_window_is_key_pressed(ecs_world_t* world, int keycode);
_Bool
spectre_window_is_key_released(ecs_world_t* world, int keycode);
float spectre_window_get_mouse_x(ecs_world_t* world);
float spectre_window_get_mouse_y(ecs_world_t* world);
float spectre_window_get_mouse_delta_x(ecs_world_t* world);
float spectre_window_get_mouse_delta_y(ecs_world_t* world);
