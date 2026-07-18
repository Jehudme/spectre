local spectre = {}
local ffi = require('ffi')
local flecs = require('wrappers.flecs')

spectre.spectre_prefabs_serialize_entity = ffi.C.spectre_prefabs_serialize_entity
spectre.spectre_prefabs_deserialize_entity = ffi.C.spectre_prefabs_deserialize_entity
spectre.spectre_prefabs_register_prefab = ffi.C.spectre_prefabs_register_prefab
spectre.spectre_prefabs_has_prefab = ffi.C.spectre_prefabs_has_prefab
spectre.spectre_prefabs_is_prefab = ffi.C.spectre_prefabs_is_prefab
spectre.spectre_prefabs_find_prefab = ffi.C.spectre_prefabs_find_prefab
spectre.spectre_prefabs_create_entity_from_props = ffi.C.spectre_prefabs_create_entity_from_props
spectre.spectre_prefabs_create_entity_from_prefab = ffi.C.spectre_prefabs_create_entity_from_prefab
spectre.spectre_prefabs_create_entity_from_name = ffi.C.spectre_prefabs_create_entity_from_name
spectre.spectre_renderer_deserialize_renderer = ffi.C.spectre_renderer_deserialize_renderer
spectre.spectre_renderer_serialize_renderer = ffi.C.spectre_renderer_serialize_renderer
spectre.spectre_renderer_register_renderer = ffi.C.spectre_renderer_register_renderer
spectre.spectre_renderer_is_renderer = ffi.C.spectre_renderer_is_renderer
spectre.spectre_resources_deserialize_resource = ffi.C.spectre_resources_deserialize_resource
spectre.spectre_resources_serialize_resource = ffi.C.spectre_resources_serialize_resource
spectre.spectre_resources_register_resource_loader = ffi.C.spectre_resources_register_resource_loader
spectre.spectre_resources_register_resource = ffi.C.spectre_resources_register_resource
spectre.spectre_resources_has_resource_loader = ffi.C.spectre_resources_has_resource_loader
spectre.spectre_resources_has_resource = ffi.C.spectre_resources_has_resource
spectre.spectre_resources_is_resource = ffi.C.spectre_resources_is_resource
spectre.spectre_resources_find_resource_loader = ffi.C.spectre_resources_find_resource_loader
spectre.spectre_resources_find_resource = ffi.C.spectre_resources_find_resource
spectre.spectre_resources_is_resource_loaded = ffi.C.spectre_resources_is_resource_loaded
spectre.spectre_resources_load_resource = ffi.C.spectre_resources_load_resource
spectre.spectre_resources_free_resource = ffi.C.spectre_resources_free_resource
spectre.spectre_resources_get_resource = ffi.C.spectre_resources_get_resource
spectre.spectre_scenes_serialize_state = ffi.C.spectre_scenes_serialize_state
spectre.spectre_scenes_deserialize_state = ffi.C.spectre_scenes_deserialize_state
spectre.spectre_scenes_serialize_scene = ffi.C.spectre_scenes_serialize_scene
spectre.spectre_scenes_deserialize_scene = ffi.C.spectre_scenes_deserialize_scene
spectre.spectre_scenes_register_state = ffi.C.spectre_scenes_register_state
spectre.spectre_scenes_register_scene = ffi.C.spectre_scenes_register_scene
spectre.spectre_scenes_find_state = ffi.C.spectre_scenes_find_state
spectre.spectre_scenes_find_scene = ffi.C.spectre_scenes_find_scene
spectre.spectre_scenes_has_state = ffi.C.spectre_scenes_has_state
spectre.spectre_scenes_has_scene = ffi.C.spectre_scenes_has_scene
spectre.spectre_scenes_is_state = ffi.C.spectre_scenes_is_state
spectre.spectre_scenes_is_scene = ffi.C.spectre_scenes_is_scene
spectre.spectre_scenes_find_current_state = ffi.C.spectre_scenes_find_current_state
spectre.spectre_scenes_find_current_scenes = ffi.C.spectre_scenes_find_current_scenes
spectre.spectre_scenes_push_state = ffi.C.spectre_scenes_push_state
spectre.spectre_scenes_pop_state = ffi.C.spectre_scenes_pop_state
spectre.spectre_scenes_execute_recursive = ffi.C.spectre_scenes_execute_recursive
spectre.spectre_scripts_has_script = ffi.C.spectre_scripts_has_script
spectre.spectre_scripts_is_script = ffi.C.spectre_scripts_is_script
spectre.spectre_scripts_find_script = ffi.C.spectre_scripts_find_script
spectre.spectre_scripts_include_code = ffi.C.spectre_scripts_include_code
spectre.spectre_scripts_execute_script = ffi.C.spectre_scripts_execute_script
spectre.spectre_scripts_serialize_scripts = ffi.C.spectre_scripts_serialize_scripts
spectre.spectre_scripts_deserialize_scripts = ffi.C.spectre_scripts_deserialize_scripts
spectre.spectre_scripts_execute_on_create = ffi.C.spectre_scripts_execute_on_create
spectre.spectre_scripts_execute_on_destroy = ffi.C.spectre_scripts_execute_on_destroy
spectre.spectre_scripts_execute_on_update = ffi.C.spectre_scripts_execute_on_update
spectre.spectre_scripts_execute_on_enter = ffi.C.spectre_scripts_execute_on_enter
spectre.spectre_scripts_execute_on_exit = ffi.C.spectre_scripts_execute_on_exit
spectre.spectre_serializer_register_serializer = ffi.C.spectre_serializer_register_serializer
spectre.spectre_serializer_has_serializer = ffi.C.spectre_serializer_has_serializer
spectre.spectre_serializer_is_serializer = ffi.C.spectre_serializer_is_serializer
spectre.spectre_serializer_find_serializer = ffi.C.spectre_serializer_find_serializer
spectre.spectre_serializer_serialize_entity = ffi.C.spectre_serializer_serialize_entity
spectre.spectre_serializer_deserialize_entity = ffi.C.spectre_serializer_deserialize_entity
spectre.spectre_window_deserialize_window = ffi.C.spectre_window_deserialize_window
spectre.spectre_window_serialize_window = ffi.C.spectre_window_serialize_window
spectre.spectre_window_register_window = ffi.C.spectre_window_register_window
spectre.spectre_window_should_close = ffi.C.spectre_window_should_close
spectre.spectre_window_set_should_close = ffi.C.spectre_window_set_should_close
spectre.spectre_window_set_size = ffi.C.spectre_window_set_size
spectre.spectre_window_set_position = ffi.C.spectre_window_set_position
spectre.spectre_window_set_size_limits = ffi.C.spectre_window_set_size_limits
spectre.spectre_window_get_width = ffi.C.spectre_window_get_width
spectre.spectre_window_get_height = ffi.C.spectre_window_get_height
spectre.spectre_window_get_aspect_ratio = ffi.C.spectre_window_get_aspect_ratio
spectre.spectre_window_get_position_x = ffi.C.spectre_window_get_position_x
spectre.spectre_window_get_position_y = ffi.C.spectre_window_get_position_y
spectre.spectre_window_set_title = ffi.C.spectre_window_set_title
spectre.spectre_window_set_vsync = ffi.C.spectre_window_set_vsync
spectre.spectre_window_set_fullscreen = ffi.C.spectre_window_set_fullscreen
spectre.spectre_window_set_borderless = ffi.C.spectre_window_set_borderless
spectre.spectre_window_set_resizable = ffi.C.spectre_window_set_resizable
spectre.spectre_window_set_always_on_top = ffi.C.spectre_window_set_always_on_top
spectre.spectre_window_get_title = ffi.C.spectre_window_get_title
spectre.spectre_window_is_vsync = ffi.C.spectre_window_is_vsync
spectre.spectre_window_is_fullscreen = ffi.C.spectre_window_is_fullscreen
spectre.spectre_window_is_borderless = ffi.C.spectre_window_is_borderless
spectre.spectre_window_is_resizable = ffi.C.spectre_window_is_resizable
spectre.spectre_window_minimize = ffi.C.spectre_window_minimize
spectre.spectre_window_maximize = ffi.C.spectre_window_maximize
spectre.spectre_window_restore = ffi.C.spectre_window_restore
spectre.spectre_window_show = ffi.C.spectre_window_show
spectre.spectre_window_hide = ffi.C.spectre_window_hide
spectre.spectre_window_request_attention = ffi.C.spectre_window_request_attention
spectre.spectre_window_is_minimized = ffi.C.spectre_window_is_minimized
spectre.spectre_window_is_maximized = ffi.C.spectre_window_is_maximized
spectre.spectre_window_is_visible = ffi.C.spectre_window_is_visible
spectre.spectre_window_is_focused = ffi.C.spectre_window_is_focused
spectre.spectre_window_is_hovered = ffi.C.spectre_window_is_hovered
spectre.spectre_window_set_cursor_visible = ffi.C.spectre_window_set_cursor_visible
spectre.spectre_window_set_cursor_locked = ffi.C.spectre_window_set_cursor_locked
spectre.spectre_window_is_cursor_visible = ffi.C.spectre_window_is_cursor_visible
spectre.spectre_window_is_cursor_locked = ffi.C.spectre_window_is_cursor_locked
spectre.spectre_window_get_native_handle = ffi.C.spectre_window_get_native_handle
spectre.spectre_window_is_key_down = ffi.C.spectre_window_is_key_down
spectre.spectre_window_is_key_pressed = ffi.C.spectre_window_is_key_pressed
spectre.spectre_window_is_key_released = ffi.C.spectre_window_is_key_released
spectre.spectre_window_get_mouse_x = ffi.C.spectre_window_get_mouse_x
spectre.spectre_window_get_mouse_y = ffi.C.spectre_window_get_mouse_y
spectre.spectre_window_get_mouse_delta_x = ffi.C.spectre_window_get_mouse_delta_x
spectre.spectre_window_get_mouse_delta_y = ffi.C.spectre_window_get_mouse_delta_y
---@return spectre_serializer_relation_t|nil
function flecs.EntityMeta:get_spectre_serializer_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_serializer_relation_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_serializer_relation_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_serializer_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_serializer_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_serializer_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_relation_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_resource_component_t|nil
function flecs.EntityMeta:get_spectre_resource_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_resource_component_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_resource_component_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_resource_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_resource_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_resource_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_component_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_resource_loader_component_t|nil
function flecs.EntityMeta:get_spectre_resource_loader_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_loader_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_resource_loader_component_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_resource_loader_component_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_loader_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_resource_loader_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_loader_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_resource_loader_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_loader_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_resource_loader_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resource_loader_component_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_disable_rendering_t|nil
function flecs.EntityMeta:get_spectre_disable_rendering_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_disable_rendering_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_disable_rendering_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_disable_rendering_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_disable_rendering_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_disable_rendering_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_disable_rendering_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_disable_rendering_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_disable_rendering_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_disable_rendering_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_disable_rendering_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_state_t|nil
function flecs.EntityMeta:get_spectre_state_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_state_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_state_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_state_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_state_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_state_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_scene_t|nil
function flecs.EntityMeta:get_spectre_scene_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scene_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scene_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_scene_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scene_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_scene_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scene_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_scene_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scene_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_scene_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scene_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_state_use_scene_relation_t|nil
function flecs.EntityMeta:get_spectre_state_use_scene_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_use_scene_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_state_use_scene_relation_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_state_use_scene_relation_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_use_scene_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_state_use_scene_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_use_scene_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_state_use_scene_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_use_scene_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_state_use_scene_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_state_use_scene_relation_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_color_t|nil
function flecs.EntityMeta:get_spectre_color_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_color_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_color_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_color_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_color_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_color_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_color_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_color_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_color_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_color_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_color_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_renderable_t|nil
function flecs.EntityMeta:get_spectre_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_renderable_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_renderable_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderable_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_2D_transform_component_t|nil
function flecs.EntityMeta:get_spectre_2D_transform_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_2D_transform_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_2D_transform_component_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_2D_transform_component_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_2D_transform_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_2D_transform_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_2D_transform_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_2D_transform_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_2D_transform_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_2D_transform_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_2D_transform_component_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_rectange_renderable_t|nil
function flecs.EntityMeta:get_spectre_rectange_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_rectange_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_rectange_renderable_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_rectange_renderable_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_rectange_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_rectange_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_rectange_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_rectange_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_rectange_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_rectange_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_rectange_renderable_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_circle_renderable_t|nil
function flecs.EntityMeta:get_spectre_circle_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_circle_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_circle_renderable_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_circle_renderable_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_circle_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_circle_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_circle_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_circle_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_circle_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_circle_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_circle_renderable_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_polygone_renderable_t|nil
function flecs.EntityMeta:get_spectre_polygone_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_polygone_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_polygone_renderable_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_polygone_renderable_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_polygone_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_polygone_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_polygone_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_polygone_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_polygone_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_polygone_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_polygone_renderable_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_custom_polygone_renderable_t|nil
function flecs.EntityMeta:get_spectre_custom_polygone_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_custom_polygone_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_custom_polygone_renderable_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_custom_polygone_renderable_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_custom_polygone_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_custom_polygone_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_custom_polygone_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_custom_polygone_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_custom_polygone_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_custom_polygone_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_custom_polygone_renderable_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_ligne_renderable_t|nil
function flecs.EntityMeta:get_spectre_ligne_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_ligne_renderable_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_ligne_renderable_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_ligne_renderable_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_ligne_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_ligne_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_ligne_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_ligne_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_ligne_renderable_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_ligne_renderable_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_ligne_renderable_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_script_t|nil
function flecs.EntityMeta:get_spectre_script_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_script_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_script_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_script_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_script_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_script_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_script_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_script_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_script_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_script_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_script_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_use_script_on_enter_relation_t|nil
function flecs.EntityMeta:get_spectre_use_script_on_enter_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_enter_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_enter_relation_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_use_script_on_enter_relation_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_enter_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_use_script_on_enter_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_enter_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_use_script_on_enter_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_enter_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_use_script_on_enter_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_enter_relation_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_use_script_on_exit_relation_t|nil
function flecs.EntityMeta:get_spectre_use_script_on_exit_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_exit_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_exit_relation_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_use_script_on_exit_relation_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_exit_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_use_script_on_exit_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_exit_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_use_script_on_exit_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_exit_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_use_script_on_exit_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_exit_relation_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_use_script_on_create_relation_t|nil
function flecs.EntityMeta:get_spectre_use_script_on_create_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_create_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_create_relation_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_use_script_on_create_relation_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_create_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_use_script_on_create_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_create_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_use_script_on_create_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_create_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_use_script_on_create_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_create_relation_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_use_script_on_destroy_relation_t|nil
function flecs.EntityMeta:get_spectre_use_script_on_destroy_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_destroy_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_destroy_relation_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_use_script_on_destroy_relation_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_destroy_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_use_script_on_destroy_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_destroy_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_use_script_on_destroy_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_destroy_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_use_script_on_destroy_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_destroy_relation_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_use_script_on_update_relation_t|nil
function flecs.EntityMeta:get_spectre_use_script_on_update_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_update_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_update_relation_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_use_script_on_update_relation_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_update_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_use_script_on_update_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_update_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_use_script_on_update_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_update_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_use_script_on_update_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_update_relation_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_use_script_on_render_relation_t|nil
function flecs.EntityMeta:get_spectre_use_script_on_render_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_render_relation_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_use_script_on_render_relation_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_use_script_on_render_relation_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_render_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_use_script_on_render_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_render_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_use_script_on_render_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_render_relation_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_use_script_on_render_relation_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_use_script_on_render_relation_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_input_state_t|nil
function flecs.EntityMeta:get_spectre_input_state_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_input_state_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_input_state_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_input_state_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_input_state_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_input_state_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_input_state_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_input_state_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_input_state_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_input_state_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_input_state_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_window_component_t|nil
function flecs.EntityMeta:get_spectre_window_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_component_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_window_component_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_window_component_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_window_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_window_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_component_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_window_component_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_component_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_prefab_flag_t|nil
function flecs.EntityMeta:get_spectre_prefab_flag_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefab_flag_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_prefab_flag_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_prefab_flag_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefab_flag_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_prefab_flag_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefab_flag_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_prefab_flag_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefab_flag_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_prefab_flag_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefab_flag_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_prefabs_api_t|nil
function flecs.EntityMeta:get_spectre_prefabs_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_prefabs_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_prefabs_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_prefabs_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_prefabs_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_prefabs_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_prefabs_service_t|nil
function flecs.EntityMeta:get_spectre_prefabs_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_prefabs_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_prefabs_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_prefabs_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_prefabs_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_prefabs_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_prefabs_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_renderer_api_t|nil
function flecs.EntityMeta:get_spectre_renderer_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_renderer_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_renderer_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_renderer_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_renderer_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_renderer_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_renderer_service_t|nil
function flecs.EntityMeta:get_spectre_renderer_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_renderer_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_renderer_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_renderer_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_renderer_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_renderer_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_renderer_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_resources_api_t|nil
function flecs.EntityMeta:get_spectre_resources_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_resources_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_resources_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_resources_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_resources_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_resources_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_resources_service_t|nil
function flecs.EntityMeta:get_spectre_resources_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_resources_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_resources_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_resources_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_resources_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_resources_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_resources_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_scenes_api_t|nil
function flecs.EntityMeta:get_spectre_scenes_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scenes_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_scenes_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_scenes_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_scenes_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_scenes_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_scenes_service_t|nil
function flecs.EntityMeta:get_spectre_scenes_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scenes_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_scenes_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_scenes_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_scenes_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_scenes_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scenes_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_scripts_api_t|nil
function flecs.EntityMeta:get_spectre_scripts_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scripts_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_scripts_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_scripts_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_scripts_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_scripts_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_scripts_service_t|nil
function flecs.EntityMeta:get_spectre_scripts_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_scripts_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_scripts_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_scripts_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_scripts_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_scripts_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_scripts_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_serializer_api_t|nil
function flecs.EntityMeta:get_spectre_serializer_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_serializer_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_serializer_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_serializer_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_serializer_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_serializer_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_serializer_service_t|nil
function flecs.EntityMeta:get_spectre_serializer_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_serializer_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_serializer_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_serializer_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_serializer_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_serializer_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_serializer_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_window_api_t|nil
function flecs.EntityMeta:get_spectre_window_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_window_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_window_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_window_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_window_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_window_api_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return spectre_window_service_t|nil
function flecs.EntityMeta:get_spectre_window_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('spectre_window_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_spectre_window_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_spectre_window_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_spectre_window_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_spectre_window_service_t()
    local comp_id = flecs.get_component_id(self.world, 'spectre_window_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

function spectre.Entity(world, id)
    return flecs.Entity(world, id)
end

return spectre