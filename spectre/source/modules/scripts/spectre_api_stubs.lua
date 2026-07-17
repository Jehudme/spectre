---@meta

---@class spectre_input_state_t
---@field keys_down[8] number
---@field keys_pressed[8] number
---@field keys_released[8] number
---@field mouse_position number
---@field mouse_delta number

---@class spectre_window_component_t
---@field width number
---@field height number
---@field position_x number
---@field position_y number
---@field min_width number
---@field min_height number
---@field max_width number
---@field max_height number
---@field title number
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
---@field native_handle number

---@class spectre_resource_component_t
---@field path number
---@field instance number

---@class spectre_resource_loader_component_t
---@field resource) number
---@field resource) number

---@class spectre_2D_transform_component_t
---@field position_z number
---@field scale_y number
---@field origin_y number
---@field rotation number

---@class spectre_component_t
---@field dummy number

---@class spectre_world
local spectre_world = {}

---@param entity number
---@return spectre_input_state_t
function spectre_world:get_mut_input(entity) end

---@return spectre_input_state_t
function spectre_world:get_mut_singleton_input() end

---@param entity number
---@return spectre_input_state_t
function spectre_world:get_input(entity) end

---@return spectre_input_state_t
function spectre_world:get_singleton_input() end

---@param entity number
---@param value spectre_input_state_t
function spectre_world:set_input(entity, value) end

---@param entity number
---@return boolean
function spectre_world:has_input(entity) end

---@param entity number
---@return spectre_window_component_t
function spectre_world:get_mut_window(entity) end

---@return spectre_window_component_t
function spectre_world:get_mut_singleton_window() end

---@param entity number
---@return spectre_window_component_t
function spectre_world:get_window(entity) end

---@return spectre_window_component_t
function spectre_world:get_singleton_window() end

---@param entity number
---@param value spectre_window_component_t
function spectre_world:set_window(entity, value) end

---@param entity number
---@return boolean
function spectre_world:has_window(entity) end

---@param entity number
---@return spectre_resource_component_t
function spectre_world:get_mut_resource(entity) end

---@return spectre_resource_component_t
function spectre_world:get_mut_singleton_resource() end

---@param entity number
---@return spectre_resource_component_t
function spectre_world:get_resource(entity) end

---@return spectre_resource_component_t
function spectre_world:get_singleton_resource() end

---@param entity number
---@param value spectre_resource_component_t
function spectre_world:set_resource(entity, value) end

---@param entity number
---@return boolean
function spectre_world:has_resource(entity) end

---@param entity number
---@return spectre_resource_loader_component_t
function spectre_world:get_mut_resource_loader(entity) end

---@return spectre_resource_loader_component_t
function spectre_world:get_mut_singleton_resource_loader() end

---@param entity number
---@return spectre_resource_loader_component_t
function spectre_world:get_resource_loader(entity) end

---@return spectre_resource_loader_component_t
function spectre_world:get_singleton_resource_loader() end

---@param entity number
---@param value spectre_resource_loader_component_t
function spectre_world:set_resource_loader(entity, value) end

---@param entity number
---@return boolean
function spectre_world:has_resource_loader(entity) end

---@param entity number
---@return spectre_2D_transform_component_t
function spectre_world:get_mut_2D_transform(entity) end

---@return spectre_2D_transform_component_t
function spectre_world:get_mut_singleton_2D_transform() end

---@param entity number
---@return spectre_2D_transform_component_t
function spectre_world:get_2D_transform(entity) end

---@return spectre_2D_transform_component_t
function spectre_world:get_singleton_2D_transform() end

---@param entity number
---@param value spectre_2D_transform_component_t
function spectre_world:set_2D_transform(entity, value) end

---@param entity number
---@return boolean
function spectre_world:has_2D_transform(entity) end

---@param entity number
---@return spectre_component_t
function spectre_world:get_mut_component_t(entity) end

---@return spectre_component_t
function spectre_world:get_mut_singleton_component_t() end

---@param entity number
---@return spectre_component_t
function spectre_world:get_component_t(entity) end

---@return spectre_component_t
function spectre_world:get_singleton_component_t() end

---@param entity number
---@param value spectre_component_t
function spectre_world:set_component_t(entity, value) end

---@param entity number
---@return boolean
function spectre_world:has_component_t(entity) end

