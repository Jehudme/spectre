---@meta

local ffi = {}
ffi.C = {}
function ffi.cdef(def) end
function ffi.new(ct, ...) end
function ffi.cast(ct, val) end
function ffi.string(ptr, len) end

---@class sandbox_application_api_t
---@field ecs) any
local sandbox_application_api_t = {}

---@class sandbox_application_service_t
---@field api any
---@field info any
local sandbox_application_service_t = {}

---@class sandbox_configuration_service_t
---@field api any
---@field info any
local sandbox_configuration_service_t = {}

---@class sandbox_filesystem_api_t
---@field read_only) number
---@field mount_point) string
---@field virtual_path) string
---@field force_path) any
---@field bytes_to_read) any
---@field bytes_to_write) any
---@field handle) any
---@field handle) any
---@field position) any
---@field handle) any
---@field handle) any
---@field force_path) any
---@field virtual_path) string
---@field force_path) any
---@field force_path) any
---@field force_path) any
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field virtual_path) string
---@field out_count) string
---@field count) any
---@field out_size) string
---@field data) any
local sandbox_filesystem_api_t = {}

---@class sandbox_filesystem_service_t
---@field api any
---@field info any
local sandbox_filesystem_service_t = {}

---@class sandbox_logs_service_t
---@field api any
---@field info any
local sandbox_logs_service_t = {}

---@class sandbox_runtime_api_t
---@field ecs) any
---@field ecs) any
---@field ecs) any
---@field ecs) any
---@field ecs) any
local sandbox_runtime_api_t = {}

---@class sandbox_runtime_service_t
---@field api any
---@field info any
local sandbox_runtime_service_t = {}

---@param info any
---@return boolean
function ffi.C.sandbox_stage_service(info) end

---@param info any
---@return boolean
function ffi.C.sandbox_stage_module(info) end

---@param ecs any
---@param library_path string
function ffi.C.sandbox_index_library(ecs, library_path) end

---@param ecs any
---@return any
function ffi.C.sandbox_get_bootstrapper(ecs) end

---@param bootstrapper any
---@param ecs any
---@param architecture string
---@param name string
---@param version_major number
---@param version_minor number
---@param version_patch number
---@return boolean
function ffi.C.sandbox_bootstrapper_activate(bootstrapper, ecs, architecture, name, version_major, version_minor, version_patch) end

---@param bootstrapper any
---@param ecs any
---@param module_str string
---@return boolean
function ffi.C.sandbox_bootstrapper_activate_string(bootstrapper, ecs, module_str) end

---@param bootstrapper any
---@param ecs any
---@return boolean
function ffi.C.sandbox_bootstrapper_boot(bootstrapper, ecs) end

---@param ecs any
---@return boolean
function ffi.C.sandbox_application_is_running(ecs) end

---@return any
function ffi.C.sandbox_properties_create() end

---@param props any
function ffi.C.sandbox_properties_destroy(props) end

---@param props any
---@param data string
---@param data_length number
---@param format any
---@return boolean
function ffi.C.sandbox_properties_load(props, data, data_length, format) end

---@param props any
---@param format any
---@return string
function ffi.C.sandbox_properties_dump(props, format) end

---@param str string
function ffi.C.sandbox_properties_free_string(str) end

---@param props any
---@param path_str string
function ffi.C.sandbox_properties_clear(props, path_str) end

---@param props any
---@param path_str string
---@return boolean
function ffi.C.sandbox_properties_has(props, path_str) end

---@param props any
---@param path_str string
---@param other any
function ffi.C.sandbox_properties_merge(props, path_str, other) end

---@param props any
---@param path_str string
---@return any
function ffi.C.sandbox_properties_sub(props, path_str) end

---@param props any
---@param path_str string
---@param out_val any
---@return boolean
function ffi.C.sandbox_properties_get_int64(props, path_str, out_val) end

---@param props any
---@param path_str string
---@param out_val any
---@return boolean
function ffi.C.sandbox_properties_get_uint64(props, path_str, out_val) end

---@param props any
---@param path_str string
---@param out_val any
---@return boolean
function ffi.C.sandbox_properties_get_double(props, path_str, out_val) end

---@param props any
---@param path_str string
---@param out_val any
---@return boolean
function ffi.C.sandbox_properties_get_bool(props, path_str, out_val) end

---@param props any
---@param path_str string
---@param val number
function ffi.C.sandbox_properties_set_int64(props, path_str, val) end

---@param props any
---@param path_str string
---@param val number
function ffi.C.sandbox_properties_set_uint64(props, path_str, val) end

---@param props any
---@param path_str string
---@param val number
function ffi.C.sandbox_properties_set_double(props, path_str, val) end

---@param props any
---@param path_str string
---@param val boolean
function ffi.C.sandbox_properties_set_bool(props, path_str, val) end

---@param props any
---@param path_str string
---@param val string
function ffi.C.sandbox_properties_set_string(props, path_str, val) end

---@param props any
---@param path_str string
---@param values any
---@param count number
function ffi.C.sandbox_properties_set_int64_array(props, path_str, values, count) end

---@param props any
---@param path_str string
---@param values any
---@param count number
function ffi.C.sandbox_properties_set_uint64_array(props, path_str, values, count) end

---@param props any
---@param path_str string
---@param values any
---@param count number
function ffi.C.sandbox_properties_set_double_array(props, path_str, values, count) end

---@param props any
---@param path_str string
---@param values any
---@param count number
function ffi.C.sandbox_properties_set_bool_array(props, path_str, values, count) end

---@param props any
---@param path_str string
---@param values string
---@param count number
function ffi.C.sandbox_properties_set_string_array(props, path_str, values, count) end

---@param ecs any
---@return any
function ffi.C.sandbox_configuration_get_properties(ecs) end

---@param ecs any
---@param physical_path string
---@param virtual_mount_point string
---@param read_only boolean
---@return boolean
function ffi.C.sandbox_filesystem_mount(ecs, physical_path, virtual_mount_point, read_only) end

---@param ecs any
---@param mount_point string
---@return boolean
function ffi.C.sandbox_filesystem_unmount(ecs, mount_point) end

---@param ecs any
---@param virtual_path string
---@return any
function ffi.C.sandbox_filesystem_open_read(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@param append boolean
---@param force_path boolean
---@return any
function ffi.C.sandbox_filesystem_open_write(ecs, virtual_path, append, force_path) end

---@param ecs any
---@param handle any
---@param buffer any
---@param bytes_to_read number
---@return number
function ffi.C.sandbox_filesystem_read(ecs, handle, buffer, bytes_to_read) end

---@param ecs any
---@param handle any
---@param buffer any
---@param bytes_to_write number
---@return number
function ffi.C.sandbox_filesystem_write(ecs, handle, buffer, bytes_to_write) end

---@param ecs any
---@param handle any
---@return boolean
function ffi.C.sandbox_filesystem_eof(ecs, handle) end

---@param ecs any
---@param handle any
---@return number
function ffi.C.sandbox_filesystem_tell(ecs, handle) end

---@param ecs any
---@param handle any
---@param position number
---@return boolean
function ffi.C.sandbox_filesystem_seek(ecs, handle, position) end

---@param ecs any
---@param handle any
---@return number
function ffi.C.sandbox_filesystem_size(ecs, handle) end

---@param ecs any
---@param handle any
function ffi.C.sandbox_filesystem_close_handle(ecs, handle) end

---@param ecs any
---@param virtual_path string
---@param force_path boolean
---@return boolean
function ffi.C.sandbox_filesystem_create_file(ecs, virtual_path, force_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_remove_file(ecs, virtual_path) end

---@param ecs any
---@param source_virtual_path string
---@param dest_virtual_path string
---@param overwrite boolean
---@param force_path boolean
---@return boolean
function ffi.C.sandbox_filesystem_copy(ecs, source_virtual_path, dest_virtual_path, overwrite, force_path) end

---@param ecs any
---@param source_virtual_path string
---@param dest_virtual_path string
---@param overwrite boolean
---@param force_path boolean
---@return boolean
function ffi.C.sandbox_filesystem_move(ecs, source_virtual_path, dest_virtual_path, overwrite, force_path) end

---@param ecs any
---@param virtual_path string
---@param force_path boolean
---@return boolean
function ffi.C.sandbox_filesystem_create_directory(ecs, virtual_path, force_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_remove_directory(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_exists(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_is_file(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_is_directory(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return boolean
function ffi.C.sandbox_filesystem_is_readonly(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return number
function ffi.C.sandbox_filesystem_file_size(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@return number
function ffi.C.sandbox_filesystem_last_modified(ecs, virtual_path) end

---@param ecs any
---@param virtual_path string
---@param recursive boolean
---@param out_files string
---@param out_count any
---@return boolean
function ffi.C.sandbox_filesystem_list_files(ecs, virtual_path, recursive, out_files, out_count) end

---@param ecs any
---@param files string
---@param count number
function ffi.C.sandbox_filesystem_free_file_list(ecs, files, count) end

---@param ecs any
---@param virtual_path string
---@param out_data any
---@param out_size any
---@return boolean
function ffi.C.sandbox_filesystem_read_all_bytes(ecs, virtual_path, out_data, out_size) end

---@param ecs any
---@param data any
function ffi.C.sandbox_filesystem_free_bytes(ecs, data) end

---@param ecs any
---@param msg string
function ffi.C.sandbox_logs_trace(ecs, msg) end

---@param ecs any
---@param msg string
function ffi.C.sandbox_logs_debug(ecs, msg) end

---@param ecs any
---@param msg string
function ffi.C.sandbox_logs_info(ecs, msg) end

---@param ecs any
---@param msg string
function ffi.C.sandbox_logs_warn(ecs, msg) end

---@param ecs any
---@param msg string
function ffi.C.sandbox_logs_error(ecs, msg) end

---@param ecs any
function ffi.C.sandbox_runtime_run(ecs) end

---@param ecs any
function ffi.C.sandbox_runtime_start(ecs) end

---@param ecs any
function ffi.C.sandbox_runtime_stop(ecs) end

---@param ecs any
function ffi.C.sandbox_runtime_pause(ecs) end

---@param ecs any
function ffi.C.sandbox_runtime_resume(ecs) end

return ffi