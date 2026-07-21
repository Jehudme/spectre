local ffi = require('ffi')

ffi.cdef[[
typedef struct spectre_component_t {
  char dummy;
} spectre_component_t;
typedef struct spectre_prefab_flag_t {
  char dummy;
} spectre_prefab_flag_t;
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
  float *vertices;
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
typedef struct spectre_use_loader_relation_t {
  char dummy;
} spectre_use_loader_relation_t;
typedef struct spectre_use_resource_relation_t {
  char dummy;
} spectre_use_resource_relation_t;
typedef struct spectre_resource_loaded_flag_t {
  char dummy;
} spectre_resource_loaded_flag_t;
typedef struct spectre_resource_component_t {
  const char *path;
  void *instance;
} spectre_resource_component_t;
typedef struct spectre_resource_loader_component_t {
  void* load_fn;
  void* free_fn;
} spectre_resource_loader_component_t;
typedef struct spectre_disable_rendering_t {
  char dummy;
} spectre_disable_rendering_t;
typedef struct spectre_state_t {
  char dummy;
} spectre_state_t;
typedef struct spectre_scene_t {
  char dummy;
} spectre_scene_t;
typedef struct spectre_state_use_scene_relation_t {
  int layer_index;
} spectre_state_use_scene_relation_t;

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
    bool boolean_value;
    double number_value;
    long long integer_value;
    const char *string_value;
    void *table_pointer;
    void *userdata_pointer;
    const char *entity;
  } value;
} spectre_script_argument_t;
typedef struct spectre_script_t {
  const char *function_name;
  const char **arguments_name;
  const spectre_script_argument_type_t *argument_types;
  uint32_t argument_count;
} spectre_script_t;

typedef struct spectre_use_script_on_enter_relation_t {
  spectre_script_argument_t *arguments;
  int argument_count;
} spectre_use_script_on_enter_relation_t;
typedef struct spectre_use_script_on_exit_relation_t {
  spectre_script_argument_t *arguments;
  int argument_count;
} spectre_use_script_on_exit_relation_t;
typedef struct spectre_use_script_on_create_relation_t {
  spectre_script_argument_t *arguments;
  int argument_count;
} spectre_use_script_on_create_relation_t;
typedef struct spectre_use_script_on_destroy_relation_t {
  spectre_script_argument_t *arguments;
  int argument_count;
} spectre_use_script_on_destroy_relation_t;
typedef struct spectre_use_script_on_update_relation_t {
  spectre_script_argument_t *arguments;
  int argument_count;
} spectre_use_script_on_update_relation_t;
typedef struct spectre_use_script_on_render_relation_t {
  spectre_script_argument_t *arguments;
  int argument_count;
} spectre_use_script_on_render_relation_t;

typedef struct spectre_serializer_component {
  void* deserialize;
  void* serialize;
} spectre_serializer_component;
typedef struct spectre_serializer_relation_t {
  char dummy;
} spectre_serializer_relation_t;

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
  const char *title;
  bool vsync;
  bool fullscreen;
  bool borderless;
  bool resizable;
  bool always_on_top;
  bool minimized;
  bool maximized;
  bool visible;
  bool focused;
  bool hovered;
  bool cursor_visible;
  bool cursor_locked;
  void *native_handle;
} spectre_window_component_t;
]]

local spectre = {}
spectre.ComponentIDs = {}

function spectre.get_component_id(world, name)
    local id = spectre.ComponentIDs[name]
    if not id then
        id = ffi.C.ecs_lookup_symbol(world, name, true, true)
        spectre.ComponentIDs[name] = id
    end
    return id
end

---@class spectre.Entity
---@field world any
---@field id number
local Entity = {}
Entity.__index = Entity

---@param world any
---@param id number
---@return spectre.Entity
function spectre.Entity(world, id)
    return setmetatable({ world = world, id = id }, Entity)
end

---@return spectre_window_component_t|nil
function Entity:get_spectre_window_component_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_window_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_window_component_t*', ptr) end
    return nil
end

function Entity:mark_spectre_window_component_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_window_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

---@return spectre_2D_transform_component_t|nil
function Entity:get_spectre_2D_transform_component_t()
    local comp_id = spectre.get_component_id(self.world, 'spectre_2D_transform_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_2D_transform_component_t*', ptr) end
    return nil
end

function Entity:mark_spectre_2D_transform_component_t_modified()
    local comp_id = spectre.get_component_id(self.world, 'spectre_2D_transform_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

return spectre