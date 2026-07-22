---@class sandbox
sandbox = {}

local ffi = require('ffi')

pcall(function() ffi.cdef[[
    typedef struct { uintptr_t token; } sandbox_properties_handle_t;
    typedef struct { uintptr_t token; } sandbox_file_handle_t;
    typedef enum { SANDBOX_FORMAT_JSON = 0, SANDBOX_FORMAT_BEVE, SANDBOX_FORMAT_TOML, SANDBOX_FORMAT_YAML } sandbox_properties_format_t;
    typedef struct ecs_world_t ecs_world_t;
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


    // Properties API
    sandbox_properties_handle_t sandbox_properties_create(void);
    void sandbox_properties_destroy(sandbox_properties_handle_t* props);
    bool sandbox_properties_load(sandbox_properties_handle_t props, const char* data, size_t data_length, sandbox_properties_format_t format);
    char* sandbox_properties_dump(sandbox_properties_handle_t props, sandbox_properties_format_t format);
    void  sandbox_properties_free_string(char* str);
    void sandbox_properties_clear(sandbox_properties_handle_t props, const char* path_str);
    bool sandbox_properties_has(sandbox_properties_handle_t props, const char* path_str);
    void sandbox_properties_keys(sandbox_properties_handle_t props, const char* path_str, void (*callback)(const char* key, void* ctx), void* ctx);
    void sandbox_properties_merge(sandbox_properties_handle_t props, const char* path_str, sandbox_properties_handle_t other);
    sandbox_properties_handle_t sandbox_properties_sub(sandbox_properties_handle_t props, const char* path_str);
    bool sandbox_properties_get_int64(sandbox_properties_handle_t props, const char* path_str, int64_t* out_val);
    bool sandbox_properties_get_uint64(sandbox_properties_handle_t props, const char* path_str, uint64_t* out_val);
    bool sandbox_properties_get_double(sandbox_properties_handle_t props, const char* path_str, double* out_val);
    bool sandbox_properties_get_bool(sandbox_properties_handle_t props, const char* path_str, bool* out_val);
    void sandbox_properties_read_string(sandbox_properties_handle_t props, const char* path_str, void (*callback)(const char* value, void* user_data), void* user_data);
    void sandbox_properties_read_int64_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(int64_t value, void* user_data), void* user_data);
    void sandbox_properties_read_uint64_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(uint64_t value, void* user_data), void* user_data);
    void sandbox_properties_read_double_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(double value, void* user_data), void* user_data);
    void sandbox_properties_read_bool_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(bool value, void* user_data), void* user_data);
    void sandbox_properties_read_string_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(const char* value, void* user_data), void* user_data);
    void sandbox_properties_set_int64(sandbox_properties_handle_t props, const char* path_str, int64_t val);
    void sandbox_properties_set_uint64(sandbox_properties_handle_t props, const char* path_str, uint64_t val);
    void sandbox_properties_set_double(sandbox_properties_handle_t props, const char* path_str, double val);
    void sandbox_properties_set_bool(sandbox_properties_handle_t props, const char* path_str, bool val);
    void sandbox_properties_set_string(sandbox_properties_handle_t props, const char* path_str, const char* val);
    void sandbox_properties_set_int64_array(sandbox_properties_handle_t props, const char* path_str, const int64_t* values, size_t count);
    void sandbox_properties_set_uint64_array(sandbox_properties_handle_t props, const char* path_str, const uint64_t* values, size_t count);
    void sandbox_properties_set_double_array(sandbox_properties_handle_t props, const char* path_str, const double* values, size_t count);
    void sandbox_properties_set_bool_array(sandbox_properties_handle_t props, const char* path_str, const bool* values, size_t count);
    void sandbox_properties_set_string_array(sandbox_properties_handle_t props, const char* path_str, const char** values, size_t count);
]] end)

local sandbox = {}

-- ========================================
-- Runtime API
-- ========================================
---@class sandbox.runtime
sandbox.runtime = {}

---@param world ecs_world_t
function sandbox.runtime.run(world)
    return ffi.C.sandbox_runtime_run((type(world) == "table" and world.ptr) and world.ptr or world)
end

---@param world ecs_world_t
function sandbox.runtime.start(world)
    return ffi.C.sandbox_runtime_start((type(world) == "table" and world.ptr) and world.ptr or world)
end

---@param world ecs_world_t
function sandbox.runtime.stop(world)
    return ffi.C.sandbox_runtime_stop((type(world) == "table" and world.ptr) and world.ptr or world)
end

---@param world ecs_world_t
function sandbox.runtime.pause(world)
    return ffi.C.sandbox_runtime_pause((type(world) == "table" and world.ptr) and world.ptr or world)
end

---@param world ecs_world_t
function sandbox.runtime.resume(world)
    return ffi.C.sandbox_runtime_resume((type(world) == "table" and world.ptr) and world.ptr or world)
end

-- ========================================
-- Application API
-- ========================================
---@class sandbox.application
sandbox.application = {}

---@param world ecs_world_t
---@return boolean
function sandbox.application.is_running(world)
    return ffi.C.sandbox_application_is_running((type(world) == "table" and world.ptr) and world.ptr or world)
end

-- ========================================
-- Filesystem API
-- ========================================
---@class sandbox.filesystem
sandbox.filesystem = {}

---@param world ecs_world_t
---@param physical_path string
---@param virtual_mount_point string
---@param read_only boolean
function sandbox.filesystem.mount(world, physical_path, virtual_mount_point, read_only)
    return ffi.C.sandbox_filesystem_mount((type(world) == "table" and world.ptr) and world.ptr or world, physical_path, virtual_mount_point, read_only)
end

---@param world ecs_world_t
---@param mount_point string
function sandbox.filesystem.unmount(world, mount_point)
    return ffi.C.sandbox_filesystem_unmount((type(world) == "table" and world.ptr) and world.ptr or world, mount_point)
end

---@param world ecs_world_t
---@param virtual_path string
---@return sandbox_file_handle_t
function sandbox.filesystem.open_read(world, virtual_path)
    return ffi.C.sandbox_filesystem_open_read((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path)
end

---@param world ecs_world_t
---@param virtual_path string
---@param append boolean
---@param force_path boolean
---@return sandbox_file_handle_t
function sandbox.filesystem.open_write(world, virtual_path, append, force_path)
    return ffi.C.sandbox_filesystem_open_write((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path, append, force_path)
end

---@param world ecs_world_t
---@param handle sandbox_file_handle_t
---@param buffer any
---@param bytes_to_read integer
---@return integer
function sandbox.filesystem.read(world, handle, buffer, bytes_to_read)
    return ffi.C.sandbox_filesystem_read((type(world) == "table" and world.ptr) and world.ptr or world, handle, buffer, bytes_to_read)
end

---@param world ecs_world_t
---@param handle sandbox_file_handle_t
---@param buffer any
---@param bytes_to_write integer
---@return integer
function sandbox.filesystem.write(world, handle, buffer, bytes_to_write)
    return ffi.C.sandbox_filesystem_write((type(world) == "table" and world.ptr) and world.ptr or world, handle, buffer, bytes_to_write)
end

---@param world ecs_world_t
---@param handle sandbox_file_handle_t
---@return boolean
function sandbox.filesystem.eof(world, handle)
    return ffi.C.sandbox_filesystem_eof((type(world) == "table" and world.ptr) and world.ptr or world, handle)
end

---@param world ecs_world_t
---@param handle sandbox_file_handle_t
---@return integer
function sandbox.filesystem.tell(world, handle)
    return ffi.C.sandbox_filesystem_tell((type(world) == "table" and world.ptr) and world.ptr or world, handle)
end

---@param world ecs_world_t
---@param handle sandbox_file_handle_t
---@param position integer
function sandbox.filesystem.seek(world, handle, position)
    return ffi.C.sandbox_filesystem_seek((type(world) == "table" and world.ptr) and world.ptr or world, handle, position)
end

---@param world ecs_world_t
---@param handle sandbox_file_handle_t
---@return integer
function sandbox.filesystem.size(world, handle)
    return ffi.C.sandbox_filesystem_size((type(world) == "table" and world.ptr) and world.ptr or world, handle)
end

---@param world ecs_world_t
---@param handle sandbox_file_handle_t
function sandbox.filesystem.close_handle(world, handle)
    return ffi.C.sandbox_filesystem_close_handle((type(world) == "table" and world.ptr) and world.ptr or world, handle)
end

---@param world ecs_world_t
---@param virtual_path string
---@param force_path boolean
---@return boolean
function sandbox.filesystem.create_file(world, virtual_path, force_path)
    return ffi.C.sandbox_filesystem_create_file((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path, force_path)
end

---@param world ecs_world_t
---@param virtual_path string
function sandbox.filesystem.remove_file(world, virtual_path)
    return ffi.C.sandbox_filesystem_remove_file((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path)
end

---@param world ecs_world_t
---@param source_virtual_path string
---@param dest_virtual_path string
---@param overwrite boolean
---@param force_path boolean
function sandbox.filesystem.copy(world, source_virtual_path, dest_virtual_path, overwrite, force_path)
    return ffi.C.sandbox_filesystem_copy((type(world) == "table" and world.ptr) and world.ptr or world, source_virtual_path, dest_virtual_path, overwrite, force_path)
end

---@param world ecs_world_t
---@param source_virtual_path string
---@param dest_virtual_path string
---@param overwrite boolean
---@param force_path boolean
function sandbox.filesystem.move(world, source_virtual_path, dest_virtual_path, overwrite, force_path)
    return ffi.C.sandbox_filesystem_move((type(world) == "table" and world.ptr) and world.ptr or world, source_virtual_path, dest_virtual_path, overwrite, force_path)
end

---@param world ecs_world_t
---@param virtual_path string
---@param force_path boolean
---@return boolean
function sandbox.filesystem.create_directory(world, virtual_path, force_path)
    return ffi.C.sandbox_filesystem_create_directory((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path, force_path)
end

---@param world ecs_world_t
---@param virtual_path string
function sandbox.filesystem.remove_directory(world, virtual_path)
    return ffi.C.sandbox_filesystem_remove_directory((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path)
end

---@param world ecs_world_t
---@param virtual_path string
---@return boolean
function sandbox.filesystem.exists(world, virtual_path)
    return ffi.C.sandbox_filesystem_exists((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path)
end

---@param world ecs_world_t
---@param virtual_path string
---@return boolean
function sandbox.filesystem.is_file(world, virtual_path)
    return ffi.C.sandbox_filesystem_is_file((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path)
end

---@param world ecs_world_t
---@param virtual_path string
---@return boolean
function sandbox.filesystem.is_directory(world, virtual_path)
    return ffi.C.sandbox_filesystem_is_directory((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path)
end

---@param world ecs_world_t
---@param virtual_path string
---@return boolean
function sandbox.filesystem.is_readonly(world, virtual_path)
    return ffi.C.sandbox_filesystem_is_readonly((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path)
end

---@param world ecs_world_t
---@param virtual_path string
---@return integer
function sandbox.filesystem.file_size(world, virtual_path)
    return ffi.C.sandbox_filesystem_file_size((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path)
end

---@param world ecs_world_t
---@param virtual_path string
function sandbox.filesystem.last_modified(world, virtual_path)
    return ffi.C.sandbox_filesystem_last_modified((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path)
end

---@param world ecs_world_t
---@param virtual_path string
---@param recursive boolean
---@param out_files any
---@param out_count any
function sandbox.filesystem.list_files(world, virtual_path, recursive, out_files, out_count)
    return ffi.C.sandbox_filesystem_list_files((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path, recursive, out_files, out_count)
end

---@param world ecs_world_t
---@param files any
---@param count integer
function sandbox.filesystem.free_file_list(world, files, count)
    return ffi.C.sandbox_filesystem_free_file_list((type(world) == "table" and world.ptr) and world.ptr or world, files, count)
end

---@param world ecs_world_t
---@param virtual_path string
---@param out_data any
---@param out_size any
---@return string
function sandbox.filesystem.read_all_bytes(world, virtual_path, out_data, out_size)
    return ffi.C.sandbox_filesystem_read_all_bytes((type(world) == "table" and world.ptr) and world.ptr or world, virtual_path, out_data, out_size)
end

---@param world ecs_world_t
---@param data string
function sandbox.filesystem.free_bytes(world, data)
    return ffi.C.sandbox_filesystem_free_bytes((type(world) == "table" and world.ptr) and world.ptr or world, data)
end

-- ========================================
-- Configuration API
-- ========================================
---@class sandbox.configuration
sandbox.configuration = {}

---@param world ecs_world_t
---@return sandbox.Properties
function sandbox.configuration.get_properties(world)
    return ffi.C.sandbox_configuration_get_properties((type(world) == "table" and world.ptr) and world.ptr or world)
end

-- ========================================
-- Logs API
-- ========================================
---@class sandbox.logs
sandbox.logs = {}

---@param world ecs_world_t
---@param msg string
function sandbox.logs.trace(world, msg)
    return ffi.C.sandbox_logs_trace((type(world) == "table" and world.ptr) and world.ptr or world, tostring(msg))
end

---@param world ecs_world_t
---@param msg string
function sandbox.logs.debug(world, msg)
    return ffi.C.sandbox_logs_debug((type(world) == "table" and world.ptr) and world.ptr or world, tostring(msg))
end

---@param world ecs_world_t
---@param msg string
function sandbox.logs.info(world, msg)
    return ffi.C.sandbox_logs_info((type(world) == "table" and world.ptr) and world.ptr or world, tostring(msg))
end

---@param world ecs_world_t
---@param msg string
function sandbox.logs.warn(world, msg)
    return ffi.C.sandbox_logs_warn((type(world) == "table" and world.ptr) and world.ptr or world, tostring(msg))
end

---@param world ecs_world_t
---@param msg string
function sandbox.logs.error(world, msg)
    return ffi.C.sandbox_logs_error((type(world) == "table" and world.ptr) and world.ptr or world, tostring(msg))
end

-- ========================================
-- Stage API
-- ========================================
---@class sandbox.stage
sandbox.stage = {}

---@param info any
function sandbox.stage.service(info)
    return ffi.C.sandbox_stage_service(info)
end

---@param info any
function sandbox.stage.module(info)
    return ffi.C.sandbox_stage_module(info)
end

-- ========================================
-- Index API
-- ========================================
---@class sandbox.index
sandbox.index = {}

---@param world ecs_world_t
---@param library_path any
function sandbox.index.library(world, library_path)
    return ffi.C.sandbox_index_library((type(world) == "table" and world.ptr) and world.ptr or world, library_path)
end

-- ========================================
-- Get API
-- ========================================
---@class sandbox.get
sandbox.get = {}

---@param world ecs_world_t
function sandbox.get.bootstrapper(world)
    return ffi.C.sandbox_get_bootstrapper((type(world) == "table" and world.ptr) and world.ptr or world)
end

-- ========================================
-- Bootstrapper API
-- ========================================
---@class sandbox.bootstrapper
sandbox.bootstrapper = {}

---@param bootstrapper any
---@param world ecs_world_t
---@param architecture any
---@param name string
---@param version_major any
---@param version_minor any
---@param version_patch any
function sandbox.bootstrapper.activate(bootstrapper, world, architecture, name, version_major, version_minor, version_patch)
    return ffi.C.sandbox_bootstrapper_activate(bootstrapper, (type(world) == "table" and world.ptr) and world.ptr or world, architecture, name, version_major, version_minor, version_patch)
end

---@param bootstrapper any
---@param world ecs_world_t
---@param module_str any
function sandbox.bootstrapper.activate_string(bootstrapper, world, module_str)
    return ffi.C.sandbox_bootstrapper_activate_string(bootstrapper, (type(world) == "table" and world.ptr) and world.ptr or world, module_str)
end

---@param bootstrapper any
---@param world ecs_world_t
function sandbox.bootstrapper.boot(bootstrapper, world)
    return ffi.C.sandbox_bootstrapper_boot(bootstrapper, (type(world) == "table" and world.ptr) and world.ptr or world)
end
-- ========================================
-- Properties Wrapper Class
-- ========================================
---@class sandbox.Properties
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

---@param data string
---@param fmt integer
function sandbox.Properties:load(data, fmt)
    return ffi.C.sandbox_properties_load(self.handle, data, #data, fmt or 0)
end

---@param fmt integer
function sandbox.Properties:dump(fmt)
    local str = ffi.C.sandbox_properties_dump(self.handle, fmt or 0)
    if str ~= nil then
        local res = ffi.string(str)
        ffi.C.sandbox_properties_free_string(str)
        return res
    end
    return nil
end

---@param path string
function sandbox.Properties:clear(path)
    ffi.C.sandbox_properties_clear(self.handle, path)
end

---@param path string
function sandbox.Properties:has(path)
    return ffi.C.sandbox_properties_has(self.handle, path)
end

---@param path string
function sandbox.Properties:sub(path)
    local sub_handle = ffi.C.sandbox_properties_sub(self.handle, path)
    local instance = { handle = sub_handle }
    setmetatable(instance, sandbox.Properties)
    return instance
end

---@param path string
function sandbox.Properties:get_int64(path)
    local out = ffi.new("int64_t[1]")
    if ffi.C.sandbox_properties_get_int64(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

---@param path string
function sandbox.Properties:get_uint64(path)
    local out = ffi.new("uint64_t[1]")
    if ffi.C.sandbox_properties_get_uint64(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

---@param path string
function sandbox.Properties:get_double(path)
    local out = ffi.new("double[1]")
    if ffi.C.sandbox_properties_get_double(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

---@param path string
function sandbox.Properties:get_bool(path)
    local out = ffi.new("bool[1]")
    if ffi.C.sandbox_properties_get_bool(self.handle, path, out) then
        return out[0]
    end
    return nil
end

---@param path string
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

---@param path string
function sandbox.Properties:read_int64_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(int64_t, void*)", function(val, ctx) table.insert(res, tonumber(val)) end)
    ffi.C.sandbox_properties_read_int64_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

---@param path string
function sandbox.Properties:read_uint64_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(uint64_t, void*)", function(val, ctx) table.insert(res, tonumber(val)) end)
    ffi.C.sandbox_properties_read_uint64_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

---@param path string
function sandbox.Properties:read_double_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(double, void*)", function(val, ctx) table.insert(res, tonumber(val)) end)
    ffi.C.sandbox_properties_read_double_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

---@param path string
function sandbox.Properties:read_bool_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(bool, void*)", function(val, ctx) table.insert(res, val) end)
    ffi.C.sandbox_properties_read_bool_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

---@param path string
function sandbox.Properties:read_string_array(path)
    local res = {}
    local cb = ffi.cast("void (*)(const char*, void*)", function(val, ctx) 
        if val ~= nil then table.insert(res, ffi.string(val)) end 
    end)
    ffi.C.sandbox_properties_read_string_array(self.handle, path, cb, nil)
    cb:free()
    return res
end

---@param path string
---@param val any
function sandbox.Properties:set_int64(path, val)
    ffi.C.sandbox_properties_set_int64(self.handle, path, val)
end

---@param path string
---@param val any
function sandbox.Properties:set_uint64(path, val)
    ffi.C.sandbox_properties_set_uint64(self.handle, path, val)
end

---@param path string
---@param val any
function sandbox.Properties:set_double(path, val)
    ffi.C.sandbox_properties_set_double(self.handle, path, val)
end

---@param path string
---@param val any
function sandbox.Properties:set_bool(path, val)
    ffi.C.sandbox_properties_set_bool(self.handle, path, val)
end

---@param path string
---@param val any
function sandbox.Properties:set_string(path, val)
    ffi.C.sandbox_properties_set_string(self.handle, path, val)
end

---@param path string
---@param arr any
function sandbox.Properties:set_int64_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("int64_t[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_int64_array(self.handle, path, c_arr, count)
end

---@param path string
---@param arr any
function sandbox.Properties:set_uint64_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("uint64_t[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_uint64_array(self.handle, path, c_arr, count)
end

---@param path string
---@param arr any
function sandbox.Properties:set_double_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("double[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_double_array(self.handle, path, c_arr, count)
end

---@param path string
---@param arr any
function sandbox.Properties:set_bool_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("bool[?]", count)
    for i=1, count do c_arr[i-1] = arr[i] end
    ffi.C.sandbox_properties_set_bool_array(self.handle, path, c_arr, count)
end

---@param path string
---@param arr any
function sandbox.Properties:set_string_array(path, arr)
    local count = #arr
    local c_arr = ffi.new("const char*[?]", count)
    for i=1, count do c_arr[i-1] = ffi.cast("const char*", arr[i]) end
    ffi.C.sandbox_properties_set_string_array(self.handle, path, c_arr, count)
end

---@param path string
function sandbox.Properties:keys(path)
    local res = {}
    local cb = ffi.cast("void (*)(const char*, void*)", function(key, ctx) 
        if key ~= nil then table.insert(res, ffi.string(key)) end 
    end)
    ffi.C.sandbox_properties_keys(self.handle, path, cb, nil)
    cb:free()
    return res
end

---@param path string
---@param other_props any
function sandbox.Properties:merge(path, other_props)
    ffi.C.sandbox_properties_merge(self.handle, path, other_props.handle)
end

function sandbox.Properties:get_handle()
    return self.handle
end

return sandbox
