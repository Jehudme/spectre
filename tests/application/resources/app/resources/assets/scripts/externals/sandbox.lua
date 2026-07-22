local ffi = require('ffi')

ffi.cdef[[
    typedef struct { uintptr_t token; } sandbox_properties_handle_t;
    typedef struct { uintptr_t token; } sandbox_file_handle_t;
    typedef enum { SANDBOX_FORMAT_JSON = 0, SANDBOX_FORMAT_BEVE, SANDBOX_FORMAT_TOML, SANDBOX_FORMAT_YAML } sandbox_properties_format_t;
    void sandbox_runtime_run(ecs_world_t* ecs);
    void sandbox_runtime_start(ecs_world_t* ecs);
    void sandbox_runtime_stop(ecs_world_t* ecs);
    void sandbox_runtime_pause(ecs_world_t* ecs);
    void sandbox_runtime_resume(ecs_world_t* ecs);
    bool sandbox_application_is_running(ecs_world_t* ecs);
    bool sandbox_filesystem_mount(ecs_world_t* ecs, const char* physical_path, const char* virtual_mount_point, bool read_only);
    bool sandbox_filesystem_unmount(ecs_world_t* ecs, const char* mount_point);
    sandbox_file_handle_t sandbox_filesystem_open_read(ecs_world_t* ecs, const char* virtual_path);
    sandbox_file_handle_t sandbox_filesystem_open_write(ecs_world_t* ecs, const char* virtual_path, bool append, bool force_path);
    size_t sandbox_filesystem_read(ecs_world_t* ecs, sandbox_file_handle_t handle, void* buffer, size_t bytes_to_read);
    size_t sandbox_filesystem_write(ecs_world_t* ecs, sandbox_file_handle_t handle, const void* buffer, size_t bytes_to_write);
    bool sandbox_filesystem_eof(ecs_world_t* ecs, sandbox_file_handle_t handle);
    size_t sandbox_filesystem_tell(ecs_world_t* ecs, sandbox_file_handle_t handle);
    bool sandbox_filesystem_seek(ecs_world_t* ecs, sandbox_file_handle_t handle, size_t position);
    size_t sandbox_filesystem_size(ecs_world_t* ecs, sandbox_file_handle_t handle);
    void sandbox_filesystem_close_handle(ecs_world_t* ecs, sandbox_file_handle_t handle);
    bool sandbox_filesystem_create_file(ecs_world_t* ecs, const char* virtual_path, bool force_path);
    bool sandbox_filesystem_remove_file(ecs_world_t* ecs, const char* virtual_path);
    bool sandbox_filesystem_copy(ecs_world_t* ecs, const char* source_virtual_path, const char* dest_virtual_path, bool overwrite, bool force_path);
    bool sandbox_filesystem_move(ecs_world_t* ecs, const char* source_virtual_path, const char* dest_virtual_path, bool overwrite, bool force_path);
    bool sandbox_filesystem_create_directory(ecs_world_t* ecs, const char* virtual_path, bool force_path);
    bool sandbox_filesystem_remove_directory(ecs_world_t* ecs, const char* virtual_path);
    bool sandbox_filesystem_exists(ecs_world_t* ecs, const char* virtual_path);
    bool sandbox_filesystem_is_file(ecs_world_t* ecs, const char* virtual_path);
    bool sandbox_filesystem_is_directory(ecs_world_t* ecs, const char* virtual_path);
    bool sandbox_filesystem_is_readonly(ecs_world_t* ecs, const char* virtual_path);
    size_t sandbox_filesystem_file_size(ecs_world_t* ecs, const char* virtual_path);
    int64_t sandbox_filesystem_last_modified(ecs_world_t* ecs, const char* virtual_path);
    bool sandbox_filesystem_list_files(ecs_world_t* ecs, const char* virtual_path, bool recursive, char*** out_files, size_t* out_count);
    void sandbox_filesystem_free_file_list(ecs_world_t* ecs, char** files, size_t count);
    bool sandbox_filesystem_read_all_bytes(ecs_world_t* ecs, const char* virtual_path, uint8_t** out_data, size_t* out_size);
    void sandbox_filesystem_free_bytes(ecs_world_t* ecs, uint8_t* data);
    sandbox_properties_handle_t sandbox_configuration_get_properties(ecs_world_t* ecs);
    void sandbox_logs_trace(ecs_world_t* ecs, const char* msg);
    void sandbox_logs_debug(ecs_world_t* ecs, const char* msg);
    void sandbox_logs_info(ecs_world_t* ecs, const char* msg);
    void sandbox_logs_warn(ecs_world_t* ecs, const char* msg);
    void sandbox_logs_error(ecs_world_t* ecs, const char* msg);
    bool sandbox_stage_service(const sandbox_service_info_t* info);
    bool sandbox_stage_module(const sandbox_module_info_t* info);
    void sandbox_index_library(ecs_world_t* ecs, const char* library_path);
    sandbox_bootstrapper_t* sandbox_get_bootstrapper(ecs_world_t* ecs);
    bool sandbox_bootstrapper_activate(sandbox_bootstrapper_t* bootstrapper, ecs_world_t* ecs, const char* architecture, const char* name, int version_major, int version_minor, int version_patch);
    bool sandbox_bootstrapper_activate_string(sandbox_bootstrapper_t* bootstrapper, ecs_world_t* ecs, const char* module_str);
    bool sandbox_bootstrapper_boot(sandbox_bootstrapper_t* bootstrapper, ecs_world_t* ecs);
]]

local sandbox = {}

-- ========================================
-- Runtime API
-- ========================================
sandbox.runtime = {}

function sandbox.runtime.run(world)
    return ffi.C.sandbox_runtime_run((world and world.ptr) and world.ptr or world)
end

function sandbox.runtime.start(world)
    return ffi.C.sandbox_runtime_start((world and world.ptr) and world.ptr or world)
end

function sandbox.runtime.stop(world)
    return ffi.C.sandbox_runtime_stop((world and world.ptr) and world.ptr or world)
end

function sandbox.runtime.pause(world)
    return ffi.C.sandbox_runtime_pause((world and world.ptr) and world.ptr or world)
end

function sandbox.runtime.resume(world)
    return ffi.C.sandbox_runtime_resume((world and world.ptr) and world.ptr or world)
end

-- ========================================
-- Application API
-- ========================================
sandbox.application = {}

function sandbox.application.is_running(world)
    return ffi.C.sandbox_application_is_running((world and world.ptr) and world.ptr or world)
end

-- ========================================
-- Filesystem API
-- ========================================
sandbox.filesystem = {}

function sandbox.filesystem.mount(world, physical_path, virtual_mount_point, read_only)
    return ffi.C.sandbox_filesystem_mount((world and world.ptr) and world.ptr or world, physical_path, virtual_mount_point, read_only)
end

function sandbox.filesystem.unmount(world, mount_point)
    return ffi.C.sandbox_filesystem_unmount((world and world.ptr) and world.ptr or world, mount_point)
end

function sandbox.filesystem.open_read(world, virtual_path)
    return ffi.C.sandbox_filesystem_open_read((world and world.ptr) and world.ptr or world, virtual_path)
end

function sandbox.filesystem.open_write(world, virtual_path, append, force_path)
    return ffi.C.sandbox_filesystem_open_write((world and world.ptr) and world.ptr or world, virtual_path, append, force_path)
end

function sandbox.filesystem.read(world, handle, buffer, bytes_to_read)
    return ffi.C.sandbox_filesystem_read((world and world.ptr) and world.ptr or world, handle, buffer, bytes_to_read)
end

function sandbox.filesystem.write(world, handle, buffer, bytes_to_write)
    return ffi.C.sandbox_filesystem_write((world and world.ptr) and world.ptr or world, handle, buffer, bytes_to_write)
end

function sandbox.filesystem.eof(world, handle)
    return ffi.C.sandbox_filesystem_eof((world and world.ptr) and world.ptr or world, handle)
end

function sandbox.filesystem.tell(world, handle)
    return ffi.C.sandbox_filesystem_tell((world and world.ptr) and world.ptr or world, handle)
end

function sandbox.filesystem.seek(world, handle, position)
    return ffi.C.sandbox_filesystem_seek((world and world.ptr) and world.ptr or world, handle, position)
end

function sandbox.filesystem.size(world, handle)
    return ffi.C.sandbox_filesystem_size((world and world.ptr) and world.ptr or world, handle)
end

function sandbox.filesystem.close_handle(world, handle)
    return ffi.C.sandbox_filesystem_close_handle((world and world.ptr) and world.ptr or world, handle)
end

function sandbox.filesystem.create_file(world, virtual_path, force_path)
    return ffi.C.sandbox_filesystem_create_file((world and world.ptr) and world.ptr or world, virtual_path, force_path)
end

function sandbox.filesystem.remove_file(world, virtual_path)
    return ffi.C.sandbox_filesystem_remove_file((world and world.ptr) and world.ptr or world, virtual_path)
end

function sandbox.filesystem.copy(world, source_virtual_path, dest_virtual_path, overwrite, force_path)
    return ffi.C.sandbox_filesystem_copy((world and world.ptr) and world.ptr or world, source_virtual_path, dest_virtual_path, overwrite, force_path)
end

function sandbox.filesystem.move(world, source_virtual_path, dest_virtual_path, overwrite, force_path)
    return ffi.C.sandbox_filesystem_move((world and world.ptr) and world.ptr or world, source_virtual_path, dest_virtual_path, overwrite, force_path)
end

function sandbox.filesystem.create_directory(world, virtual_path, force_path)
    return ffi.C.sandbox_filesystem_create_directory((world and world.ptr) and world.ptr or world, virtual_path, force_path)
end

function sandbox.filesystem.remove_directory(world, virtual_path)
    return ffi.C.sandbox_filesystem_remove_directory((world and world.ptr) and world.ptr or world, virtual_path)
end

function sandbox.filesystem.exists(world, virtual_path)
    return ffi.C.sandbox_filesystem_exists((world and world.ptr) and world.ptr or world, virtual_path)
end

function sandbox.filesystem.is_file(world, virtual_path)
    return ffi.C.sandbox_filesystem_is_file((world and world.ptr) and world.ptr or world, virtual_path)
end

function sandbox.filesystem.is_directory(world, virtual_path)
    return ffi.C.sandbox_filesystem_is_directory((world and world.ptr) and world.ptr or world, virtual_path)
end

function sandbox.filesystem.is_readonly(world, virtual_path)
    return ffi.C.sandbox_filesystem_is_readonly((world and world.ptr) and world.ptr or world, virtual_path)
end

function sandbox.filesystem.file_size(world, virtual_path)
    return ffi.C.sandbox_filesystem_file_size((world and world.ptr) and world.ptr or world, virtual_path)
end

function sandbox.filesystem.last_modified(world, virtual_path)
    return ffi.C.sandbox_filesystem_last_modified((world and world.ptr) and world.ptr or world, virtual_path)
end

function sandbox.filesystem.list_files(world, virtual_path, recursive, out_files, out_count)
    return ffi.C.sandbox_filesystem_list_files((world and world.ptr) and world.ptr or world, virtual_path, recursive, out_files, out_count)
end

function sandbox.filesystem.free_file_list(world, files, count)
    return ffi.C.sandbox_filesystem_free_file_list((world and world.ptr) and world.ptr or world, files, count)
end

function sandbox.filesystem.read_all_bytes(world, virtual_path, out_data, out_size)
    return ffi.C.sandbox_filesystem_read_all_bytes((world and world.ptr) and world.ptr or world, virtual_path, out_data, out_size)
end

function sandbox.filesystem.free_bytes(world, data)
    return ffi.C.sandbox_filesystem_free_bytes((world and world.ptr) and world.ptr or world, data)
end

-- ========================================
-- Configuration API
-- ========================================
sandbox.configuration = {}

function sandbox.configuration.get_properties(world)
    return ffi.C.sandbox_configuration_get_properties((world and world.ptr) and world.ptr or world)
end

-- ========================================
-- Logs API
-- ========================================
sandbox.logs = {}

function sandbox.logs.trace(world, msg)
    return ffi.C.sandbox_logs_trace((world and world.ptr) and world.ptr or world, tostring(msg))
end

function sandbox.logs.debug(world, msg)
    return ffi.C.sandbox_logs_debug((world and world.ptr) and world.ptr or world, tostring(msg))
end

function sandbox.logs.info(world, msg)
    return ffi.C.sandbox_logs_info((world and world.ptr) and world.ptr or world, tostring(msg))
end

function sandbox.logs.warn(world, msg)
    return ffi.C.sandbox_logs_warn((world and world.ptr) and world.ptr or world, tostring(msg))
end

function sandbox.logs.error(world, msg)
    return ffi.C.sandbox_logs_error((world and world.ptr) and world.ptr or world, tostring(msg))
end

-- ========================================
-- Stage API
-- ========================================
sandbox.stage = {}

function sandbox.stage.service(info)
    return ffi.C.sandbox_stage_service(info)
end

function sandbox.stage.module(info)
    return ffi.C.sandbox_stage_module(info)
end

-- ========================================
-- Index API
-- ========================================
sandbox.index = {}

function sandbox.index.library(world, library_path)
    return ffi.C.sandbox_index_library((world and world.ptr) and world.ptr or world, library_path)
end

-- ========================================
-- Get API
-- ========================================
sandbox.get = {}

function sandbox.get.bootstrapper(world)
    return ffi.C.sandbox_get_bootstrapper((world and world.ptr) and world.ptr or world)
end

-- ========================================
-- Bootstrapper API
-- ========================================
sandbox.bootstrapper = {}

function sandbox.bootstrapper.activate(bootstrapper, world, architecture, name, version_major, version_minor, version_patch)
    return ffi.C.sandbox_bootstrapper_activate(bootstrapper, (world and world.ptr) and world.ptr or world, architecture, name, version_major, version_minor, version_patch)
end

function sandbox.bootstrapper.activate_string(bootstrapper, world, module_str)
    return ffi.C.sandbox_bootstrapper_activate_string(bootstrapper, (world and world.ptr) and world.ptr or world, module_str)
end

function sandbox.bootstrapper.boot(bootstrapper, world)
    return ffi.C.sandbox_bootstrapper_boot(bootstrapper, (world and world.ptr) and world.ptr or world)
end
-- ========================================
-- Properties Wrapper Class
-- ========================================
sandbox.Properties = {}
sandbox.Properties.__index = sandbox.Properties

function sandbox.Properties.new()
    local instance = { handle = ffi.C.sandbox_properties_create() }
    setmetatable(instance, sandbox.Properties)
    return instance
end

function sandbox.Properties:destroy()
    local ptr = ffi.new("sandbox_properties_handle_t[1]", self.handle)
    ffi.C.sandbox_properties_destroy(ptr)
end

function sandbox.Properties:load(data, fmt)
    return ffi.C.sandbox_properties_load(self.handle, data, #data, fmt or 0)
end

function sandbox.Properties:dump(fmt)
    local str = ffi.C.sandbox_properties_dump(self.handle, fmt or 0)
    if str ~= nil then
        local res = ffi.string(str)
        ffi.C.sandbox_properties_free_string(str)
        return res
    end
    return nil
end

function sandbox.Properties:clear(path)
    ffi.C.sandbox_properties_clear(self.handle, path)
end

function sandbox.Properties:has(path)
    return ffi.C.sandbox_properties_has(self.handle, path)
end

function sandbox.Properties:sub(path)
    local sub_handle = ffi.C.sandbox_properties_sub(self.handle, path)
    local instance = { handle = sub_handle }
    setmetatable(instance, sandbox.Properties)
    return instance
end

function sandbox.Properties:get_int64(path)
    local out = ffi.new("int64_t[1]")
    if ffi.C.sandbox_properties_get_int64(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

function sandbox.Properties:get_uint64(path)
    local out = ffi.new("uint64_t[1]")
    if ffi.C.sandbox_properties_get_uint64(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

function sandbox.Properties:get_double(path)
    local out = ffi.new("double[1]")
    if ffi.C.sandbox_properties_get_double(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

function sandbox.Properties:get_bool(path)
    local out = ffi.new("bool[1]")
    if ffi.C.sandbox_properties_get_bool(self.handle, path, out) then
        return out[0]
    end
    return nil
end

function sandbox.Properties:read_string(path)
    local res = nil
    local cb = ffi.cast("void (*)(const char*, void*)", function(val, ctx)
        if val ~= nil then
            res = ffi.string(val)
        end
    end)
    ffi.C.sandbox_properties_read_string(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_int64_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(int64_t, void*)", function(val, ctx) table.insert(res, tonumber(val)) end)
    ffi.C.sandbox_properties_read_int64_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_uint64_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(uint64_t, void*)", function(val, ctx) table.insert(res, tonumber(val)) end)
    ffi.C.sandbox_properties_read_uint64_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_double_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(double, void*)", function(val, ctx) table.insert(res, tonumber(val)) end)
    ffi.C.sandbox_properties_read_double_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_bool_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(bool, void*)", function(val, ctx) table.insert(res, val) end)
    ffi.C.sandbox_properties_read_bool_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:read_string_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(const char*, void*)", function(val, ctx) 
        if val ~= nil then table.insert(res, ffi.string(val)) end 
    end)
    ffi.C.sandbox_properties_read_string_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:set_int64(path, val)
    ffi.C.sandbox_properties_set_int64(self.handle, path, val)
end

function sandbox.Properties:set_uint64(path, val)
    ffi.C.sandbox_properties_set_uint64(self.handle, path, val)
end

function sandbox.Properties:set_double(path, val)
    ffi.C.sandbox_properties_set_double(self.handle, path, val)
end

function sandbox.Properties:set_bool(path, val)
    ffi.C.sandbox_properties_set_bool(self.handle, path, val)
end

function sandbox.Properties:set_string(path, val)
    ffi.C.sandbox_properties_set_string(self.handle, path, val)
end

function sandbox.Properties:set_int64_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("int64_t[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_int64_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:set_uint64_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("uint64_t[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_uint64_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:set_double_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("double[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_double_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:set_bool_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("bool[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_bool_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:set_string_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("const char*[?]", count)
    for i=1, count do c_arr[i-1] = ffi.cast("const char*", arr[i]) end
    ffi.C.sandbox_properties_set_string_array(self.handle, path, c_arr, count)
end

function sandbox.Properties:keys(path)
    local res = {}
    local cb = ffi.cast("void (*)(const char*, void*)", function(key, ctx) 
        if key ~= nil then table.insert(res, ffi.string(key)) end 
    end)
    ffi.C.sandbox_properties_keys(self.handle, path, cb, nil)
    cb:free()
    return res
end

function sandbox.Properties:merge(path, other_props)
    ffi.C.sandbox_properties_merge(self.handle, path, other_props.handle)
end

function sandbox.Properties:get_handle()
    return self.handle
end

return sandbox
