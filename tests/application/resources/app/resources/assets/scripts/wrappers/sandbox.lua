local sandbox = {}
local ffi = require('ffi')
local flecs = require('wrappers.flecs')

sandbox.sandbox_stage_service = ffi.C.sandbox_stage_service
sandbox.sandbox_stage_module = ffi.C.sandbox_stage_module
sandbox.sandbox_index_library = ffi.C.sandbox_index_library
sandbox.sandbox_get_bootstrapper = ffi.C.sandbox_get_bootstrapper
sandbox.sandbox_bootstrapper_activate = ffi.C.sandbox_bootstrapper_activate
sandbox.sandbox_bootstrapper_activate_string = ffi.C.sandbox_bootstrapper_activate_string
sandbox.sandbox_bootstrapper_boot = ffi.C.sandbox_bootstrapper_boot
sandbox.sandbox_application_is_running = ffi.C.sandbox_application_is_running
sandbox.sandbox_properties_create = ffi.C.sandbox_properties_create
sandbox.sandbox_properties_destroy = ffi.C.sandbox_properties_destroy
sandbox.sandbox_properties_load = ffi.C.sandbox_properties_load
sandbox.sandbox_properties_dump = ffi.C.sandbox_properties_dump
sandbox.sandbox_properties_free_string = ffi.C.sandbox_properties_free_string
sandbox.sandbox_properties_clear = ffi.C.sandbox_properties_clear
sandbox.sandbox_properties_has = ffi.C.sandbox_properties_has
sandbox.sandbox_properties_merge = ffi.C.sandbox_properties_merge
sandbox.sandbox_properties_sub = ffi.C.sandbox_properties_sub
sandbox.sandbox_properties_get_int64 = ffi.C.sandbox_properties_get_int64
sandbox.sandbox_properties_get_uint64 = ffi.C.sandbox_properties_get_uint64
sandbox.sandbox_properties_get_double = ffi.C.sandbox_properties_get_double
sandbox.sandbox_properties_get_bool = ffi.C.sandbox_properties_get_bool
sandbox.sandbox_properties_set_int64 = ffi.C.sandbox_properties_set_int64
sandbox.sandbox_properties_set_uint64 = ffi.C.sandbox_properties_set_uint64
sandbox.sandbox_properties_set_double = ffi.C.sandbox_properties_set_double
sandbox.sandbox_properties_set_bool = ffi.C.sandbox_properties_set_bool
sandbox.sandbox_properties_set_string = ffi.C.sandbox_properties_set_string
sandbox.sandbox_properties_set_int64_array = ffi.C.sandbox_properties_set_int64_array
sandbox.sandbox_properties_set_uint64_array = ffi.C.sandbox_properties_set_uint64_array
sandbox.sandbox_properties_set_double_array = ffi.C.sandbox_properties_set_double_array
sandbox.sandbox_properties_set_bool_array = ffi.C.sandbox_properties_set_bool_array
sandbox.sandbox_properties_set_string_array = ffi.C.sandbox_properties_set_string_array
sandbox.sandbox_configuration_get_properties = ffi.C.sandbox_configuration_get_properties
sandbox.sandbox_filesystem_mount = ffi.C.sandbox_filesystem_mount
sandbox.sandbox_filesystem_unmount = ffi.C.sandbox_filesystem_unmount
sandbox.sandbox_filesystem_open_read = ffi.C.sandbox_filesystem_open_read
sandbox.sandbox_filesystem_open_write = ffi.C.sandbox_filesystem_open_write
sandbox.sandbox_filesystem_read = ffi.C.sandbox_filesystem_read
sandbox.sandbox_filesystem_write = ffi.C.sandbox_filesystem_write
sandbox.sandbox_filesystem_eof = ffi.C.sandbox_filesystem_eof
sandbox.sandbox_filesystem_tell = ffi.C.sandbox_filesystem_tell
sandbox.sandbox_filesystem_seek = ffi.C.sandbox_filesystem_seek
sandbox.sandbox_filesystem_size = ffi.C.sandbox_filesystem_size
sandbox.sandbox_filesystem_close_handle = ffi.C.sandbox_filesystem_close_handle
sandbox.sandbox_filesystem_create_file = ffi.C.sandbox_filesystem_create_file
sandbox.sandbox_filesystem_remove_file = ffi.C.sandbox_filesystem_remove_file
sandbox.sandbox_filesystem_copy = ffi.C.sandbox_filesystem_copy
sandbox.sandbox_filesystem_move = ffi.C.sandbox_filesystem_move
sandbox.sandbox_filesystem_create_directory = ffi.C.sandbox_filesystem_create_directory
sandbox.sandbox_filesystem_remove_directory = ffi.C.sandbox_filesystem_remove_directory
sandbox.sandbox_filesystem_exists = ffi.C.sandbox_filesystem_exists
sandbox.sandbox_filesystem_is_file = ffi.C.sandbox_filesystem_is_file
sandbox.sandbox_filesystem_is_directory = ffi.C.sandbox_filesystem_is_directory
sandbox.sandbox_filesystem_is_readonly = ffi.C.sandbox_filesystem_is_readonly
sandbox.sandbox_filesystem_file_size = ffi.C.sandbox_filesystem_file_size
sandbox.sandbox_filesystem_last_modified = ffi.C.sandbox_filesystem_last_modified
sandbox.sandbox_filesystem_list_files = ffi.C.sandbox_filesystem_list_files
sandbox.sandbox_filesystem_free_file_list = ffi.C.sandbox_filesystem_free_file_list
sandbox.sandbox_filesystem_read_all_bytes = ffi.C.sandbox_filesystem_read_all_bytes
sandbox.sandbox_filesystem_free_bytes = ffi.C.sandbox_filesystem_free_bytes
sandbox.sandbox_logs_trace = ffi.C.sandbox_logs_trace
sandbox.sandbox_logs_debug = ffi.C.sandbox_logs_debug
sandbox.sandbox_logs_info = ffi.C.sandbox_logs_info
sandbox.sandbox_logs_warn = ffi.C.sandbox_logs_warn
sandbox.sandbox_logs_error = ffi.C.sandbox_logs_error
sandbox.sandbox_runtime_run = ffi.C.sandbox_runtime_run
sandbox.sandbox_runtime_start = ffi.C.sandbox_runtime_start
sandbox.sandbox_runtime_stop = ffi.C.sandbox_runtime_stop
sandbox.sandbox_runtime_pause = ffi.C.sandbox_runtime_pause
sandbox.sandbox_runtime_resume = ffi.C.sandbox_runtime_resume
---@return sandbox_application_api_t|nil
function flecs.EntityMeta:get_sandbox_application_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_application_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_sandbox_application_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_sandbox_application_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_sandbox_application_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_sandbox_application_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return sandbox_application_service_t|nil
function flecs.EntityMeta:get_sandbox_application_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_application_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_sandbox_application_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_sandbox_application_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_sandbox_application_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_sandbox_application_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_application_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return sandbox_configuration_service_t|nil
function flecs.EntityMeta:get_sandbox_configuration_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_configuration_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_configuration_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_sandbox_configuration_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_configuration_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_sandbox_configuration_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_configuration_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_sandbox_configuration_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_configuration_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_sandbox_configuration_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_configuration_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return sandbox_filesystem_api_t|nil
function flecs.EntityMeta:get_sandbox_filesystem_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_filesystem_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_sandbox_filesystem_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_sandbox_filesystem_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_sandbox_filesystem_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_sandbox_filesystem_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return sandbox_filesystem_service_t|nil
function flecs.EntityMeta:get_sandbox_filesystem_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_filesystem_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_sandbox_filesystem_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_sandbox_filesystem_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_sandbox_filesystem_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_sandbox_filesystem_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_filesystem_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return sandbox_logs_service_t|nil
function flecs.EntityMeta:get_sandbox_logs_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_logs_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_logs_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_sandbox_logs_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_logs_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_sandbox_logs_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_logs_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_sandbox_logs_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_logs_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_sandbox_logs_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_logs_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return sandbox_runtime_api_t|nil
function flecs.EntityMeta:get_sandbox_runtime_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_api_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_runtime_api_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_sandbox_runtime_api_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_sandbox_runtime_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_sandbox_runtime_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_api_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_sandbox_runtime_api_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_api_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

---@return sandbox_runtime_service_t|nil
function flecs.EntityMeta:get_sandbox_runtime_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_service_t')
    if comp_id == 0 then return nil end
    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)
    if ptr ~= nil then return ffi.cast('sandbox_runtime_service_t*', ptr) end
    return nil
end

function flecs.EntityMeta:mark_sandbox_runtime_service_t_modified()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_modified_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:add_sandbox_runtime_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_add_id(self.world, self.id, comp_id)
    end
end

function flecs.EntityMeta:remove_sandbox_runtime_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_service_t')
    if comp_id ~= 0 then
        ffi.C.ecs_remove_id(self.world, self.id, comp_id)
    end
end

---@return boolean
function flecs.EntityMeta:has_sandbox_runtime_service_t()
    local comp_id = flecs.get_component_id(self.world, 'sandbox_runtime_service_t')
    if comp_id ~= 0 then
        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true
    end
    return false
end

function sandbox.Entity(world, id)
    return flecs.Entity(world, id)
end

return sandbox