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

typedef uint32_t sandbox_requirement_kind_t;
enum {
SANDBOX_REQUIREMENT_KIND_SERVICE = 0,
SANDBOX_REQUIREMENT_KIND_MODULE
};
typedef uint32_t sandbox_requirement_strictness_t;
enum {
SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED = 0,
SANDBOX_REQUIREMENT_STRICTNESS_EXPECTED
};
typedef struct {
sandbox_requirement_kind_t kind;
sandbox_requirement_strictness_t strictness;
const char* name;
const char* architecture;
int32_t version_major;
int32_t version_minor;
int32_t version_patch;
} sandbox_requirement_info_t;
typedef struct {
uint32_t struct_size;
const char* name;
const char* description;
const char* architecture;
int32_t version_major;
int32_t version_minor;
void (*init_fn)(ecs_world_t* ecs);
} sandbox_service_info_t;
typedef struct {
uint32_t struct_size;
const char* name;
const char* description;
const char* architecture;
int32_t version_major;
int32_t version_minor;
int32_t version_patch;
const sandbox_service_info_t* service;
const sandbox_requirement_info_t* requirements;
size_t requirement_count;
void (*init_fn)(ecs_world_t* ecs);
} sandbox_module_info_t;
_Bool
sandbox_stage_service(const sandbox_service_info_t* info);
_Bool
sandbox_stage_module(const sandbox_module_info_t* info);
void sandbox_index_library(ecs_world_t* ecs, const char* library_path);
typedef struct sandbox_bootstrapper sandbox_bootstrapper_t;
typedef struct {
sandbox_bootstrapper_t* internal_bootstrapper;
} sandbox_bootstrapper_component_t;
extern ecs_entity_t FLECS_IDsandbox_bootstrapper_component_tID_;
sandbox_bootstrapper_t* sandbox_get_bootstrapper(ecs_world_t* ecs);
_Bool
sandbox_bootstrapper_activate(sandbox_bootstrapper_t* bootstrapper, ecs_world_t* ecs, const char* architecture, const char* name, int version_major, int version_minor, int version_patch);
_Bool
sandbox_bootstrapper_activate_string(sandbox_bootstrapper_t* bootstrapper, ecs_world_t* ecs, const char* module_str);
_Bool
sandbox_bootstrapper_boot(sandbox_bootstrapper_t* bootstrapper, ecs_world_t* ecs);
typedef struct sandbox_application_api_t {
_Bool
(*is_running)(ecs_world_t* ecs);
} sandbox_application_api_t;
typedef struct sandbox_application_service_t { sandbox_application_api_t* api; const sandbox_service_info_t* info; } sandbox_application_service_t; extern ecs_entity_t FLECS_IDsandbox_application_service_tID_; extern sandbox_service_info_t sandbox_application_service_t_info;
;
_Bool
sandbox_application_is_running(ecs_world_t* ecs);
typedef int32_t sandbox_status_t;
typedef enum {
SANDBOX_FORMAT_JSON = 0,
SANDBOX_FORMAT_BEVE,
SANDBOX_FORMAT_TOML,
SANDBOX_FORMAT_YAML
} sandbox_properties_format_t;
typedef struct { uintptr_t token; } sandbox_properties_handle_t;
sandbox_properties_handle_t sandbox_properties_create(void);
void sandbox_properties_destroy(sandbox_properties_handle_t* props);
_Bool
sandbox_properties_load(sandbox_properties_handle_t props, const char* data, size_t data_length, sandbox_properties_format_t format);
char* sandbox_properties_dump(sandbox_properties_handle_t props, sandbox_properties_format_t format);
void sandbox_properties_free_string(char* str);
void sandbox_properties_clear(sandbox_properties_handle_t props, const char* path_str);
_Bool
sandbox_properties_has(sandbox_properties_handle_t props, const char* path_str);
void sandbox_properties_keys(sandbox_properties_handle_t props, const char* path_str, void (*callback)(const char* key, void* ctx), void* ctx);
void sandbox_properties_merge(sandbox_properties_handle_t props, const char* path_str, sandbox_properties_handle_t other);
sandbox_properties_handle_t sandbox_properties_sub(sandbox_properties_handle_t props, const char* path_str);
_Bool
sandbox_properties_get_int64(sandbox_properties_handle_t props, const char* path_str, int64_t* out_val);
_Bool
sandbox_properties_get_uint64(sandbox_properties_handle_t props, const char* path_str, uint64_t* out_val);
_Bool
sandbox_properties_get_double(sandbox_properties_handle_t props, const char* path_str, double* out_val);
_Bool
sandbox_properties_get_bool(sandbox_properties_handle_t props, const char* path_str,
_Bool
* out_val);
void sandbox_properties_read_string(sandbox_properties_handle_t props, const char* path_str, void (*callback)(const char* value, void* user_data), void* user_data);
void sandbox_properties_read_int64_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(int64_t value, void* user_data), void* user_data);
void sandbox_properties_read_uint64_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(uint64_t value, void* user_data), void* user_data);
void sandbox_properties_read_double_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(double value, void* user_data), void* user_data);
void sandbox_properties_read_bool_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(
_Bool
value, void* user_data), void* user_data);
void sandbox_properties_read_string_array(sandbox_properties_handle_t props, const char* path_str, void (*callback)(const char* value, void* user_data), void* user_data);
void sandbox_properties_set_int64(sandbox_properties_handle_t props, const char* path_str, int64_t val);
void sandbox_properties_set_uint64(sandbox_properties_handle_t props, const char* path_str, uint64_t val);
void sandbox_properties_set_double(sandbox_properties_handle_t props, const char* path_str, double val);
void sandbox_properties_set_bool(sandbox_properties_handle_t props, const char* path_str,
_Bool
val);
void sandbox_properties_set_string(sandbox_properties_handle_t props, const char* path_str, const char* val);
void sandbox_properties_set_int64_array(sandbox_properties_handle_t props, const char* path_str, const int64_t* values, size_t count);
void sandbox_properties_set_uint64_array(sandbox_properties_handle_t props, const char* path_str, const uint64_t* values, size_t count);
void sandbox_properties_set_double_array(sandbox_properties_handle_t props, const char* path_str, const double* values, size_t count);
void sandbox_properties_set_bool_array(sandbox_properties_handle_t props, const char* path_str, const
_Bool
* values, size_t count);
void sandbox_properties_set_string_array(sandbox_properties_handle_t props, const char* path_str, const char** values, size_t count);
typedef struct {
sandbox_properties_handle_t (*get_properties)(ecs_world_t* ecs);
} sandbox_configuration_api_t;
typedef struct sandbox_configuration_service_t { sandbox_configuration_api_t* api; const sandbox_service_info_t* info; } sandbox_configuration_service_t; extern ecs_entity_t FLECS_IDsandbox_configuration_service_tID_; extern sandbox_service_info_t sandbox_configuration_service_t_info;
sandbox_properties_handle_t sandbox_configuration_get_properties(ecs_world_t* ecs);
typedef struct { uintptr_t token; } sandbox_file_handle_t;
typedef struct sandbox_filesystem_api_t {
_Bool
(*mount)(ecs_world_t* ecs, const char* physical_path, const char* virtual_mount_point,
_Bool
read_only);
_Bool
(*unmount)(ecs_world_t* ecs, const char* mount_point);
sandbox_file_handle_t (*open_read)(ecs_world_t* ecs, const char* virtual_path);
sandbox_file_handle_t (*open_write)(ecs_world_t* ecs, const char* virtual_path,
_Bool
append,
_Bool
force_path);
size_t (*read)(ecs_world_t* ecs, sandbox_file_handle_t handle, void* buffer, size_t bytes_to_read);
size_t (*write)(ecs_world_t* ecs, sandbox_file_handle_t handle, const void* buffer, size_t bytes_to_write);
_Bool
(*eof)(ecs_world_t* ecs, sandbox_file_handle_t handle);
size_t (*tell)(ecs_world_t* ecs, sandbox_file_handle_t handle);
_Bool
(*seek)(ecs_world_t* ecs, sandbox_file_handle_t handle, size_t position);
size_t (*size)(ecs_world_t* ecs, sandbox_file_handle_t handle);
void (*close_handle)(ecs_world_t* ecs, sandbox_file_handle_t handle);
_Bool
(*create_file)(ecs_world_t* ecs, const char* virtual_path,
_Bool
force_path);
_Bool
(*remove_file)(ecs_world_t* ecs, const char* virtual_path);
_Bool
(*copy)(ecs_world_t* ecs, const char* source_virtual_path, const char* dest_virtual_path,
_Bool
overwrite,
_Bool
force_path);
_Bool
(*move)(ecs_world_t* ecs, const char* source_virtual_path, const char* dest_virtual_path,
_Bool
overwrite,
_Bool
force_path);
_Bool
(*create_directory)(ecs_world_t* ecs, const char* virtual_path,
_Bool
force_path);
_Bool
(*remove_directory)(ecs_world_t* ecs, const char* virtual_path);
_Bool
(*exists)(ecs_world_t* ecs, const char* virtual_path);
_Bool
(*is_file)(ecs_world_t* ecs, const char* virtual_path);
_Bool
(*is_directory)(ecs_world_t* ecs, const char* virtual_path);
_Bool
(*is_readonly)(ecs_world_t* ecs, const char* virtual_path);
size_t (*file_size)(ecs_world_t* ecs, const char* virtual_path);
int64_t (*last_modified)(ecs_world_t* ecs, const char* virtual_path);
_Bool
(*list_files)(ecs_world_t* ecs, const char* virtual_path,
_Bool
recursive, char*** out_files, size_t* out_count);
void (*free_file_list)(ecs_world_t* ecs, char** files, size_t count);
_Bool
(*read_all_bytes)(ecs_world_t* ecs, const char* virtual_path, uint8_t** out_data, size_t* out_size);
void (*free_bytes)(ecs_world_t* ecs, uint8_t* data);
} sandbox_filesystem_api_t;
typedef struct sandbox_filesystem_service_t { sandbox_filesystem_api_t* api; const sandbox_service_info_t* info; } sandbox_filesystem_service_t; extern ecs_entity_t FLECS_IDsandbox_filesystem_service_tID_; extern sandbox_service_info_t sandbox_filesystem_service_t_info;
;
_Bool
sandbox_filesystem_mount(ecs_world_t* ecs, const char* physical_path, const char* virtual_mount_point,
_Bool
read_only);
_Bool
sandbox_filesystem_unmount(ecs_world_t* ecs, const char* mount_point);
sandbox_file_handle_t sandbox_filesystem_open_read(ecs_world_t* ecs, const char* virtual_path);
sandbox_file_handle_t sandbox_filesystem_open_write(ecs_world_t* ecs, const char* virtual_path,
_Bool
append,
_Bool
force_path);
size_t sandbox_filesystem_read(ecs_world_t* ecs, sandbox_file_handle_t handle, void* buffer, size_t bytes_to_read);
size_t sandbox_filesystem_write(ecs_world_t* ecs, sandbox_file_handle_t handle, const void* buffer, size_t bytes_to_write);
_Bool
sandbox_filesystem_eof(ecs_world_t* ecs, sandbox_file_handle_t handle);
size_t sandbox_filesystem_tell(ecs_world_t* ecs, sandbox_file_handle_t handle);
_Bool
sandbox_filesystem_seek(ecs_world_t* ecs, sandbox_file_handle_t handle, size_t position);
size_t sandbox_filesystem_size(ecs_world_t* ecs, sandbox_file_handle_t handle);
void sandbox_filesystem_close_handle(ecs_world_t* ecs, sandbox_file_handle_t handle);
_Bool
sandbox_filesystem_create_file(ecs_world_t* ecs, const char* virtual_path,
_Bool
force_path);
_Bool
sandbox_filesystem_remove_file(ecs_world_t* ecs, const char* virtual_path);
_Bool
sandbox_filesystem_copy(ecs_world_t* ecs, const char* source_virtual_path, const char* dest_virtual_path,
_Bool
overwrite,
_Bool
force_path);
_Bool
sandbox_filesystem_move(ecs_world_t* ecs, const char* source_virtual_path, const char* dest_virtual_path,
_Bool
overwrite,
_Bool
force_path);
_Bool
sandbox_filesystem_create_directory(ecs_world_t* ecs, const char* virtual_path,
_Bool
force_path);
_Bool
sandbox_filesystem_remove_directory(ecs_world_t* ecs, const char* virtual_path);
_Bool
sandbox_filesystem_exists(ecs_world_t* ecs, const char* virtual_path);
_Bool
sandbox_filesystem_is_file(ecs_world_t* ecs, const char* virtual_path);
_Bool
sandbox_filesystem_is_directory(ecs_world_t* ecs, const char* virtual_path);
_Bool
sandbox_filesystem_is_readonly(ecs_world_t* ecs, const char* virtual_path);
size_t sandbox_filesystem_file_size(ecs_world_t* ecs, const char* virtual_path);
int64_t sandbox_filesystem_last_modified(ecs_world_t* ecs, const char* virtual_path);
_Bool
sandbox_filesystem_list_files(ecs_world_t* ecs, const char* virtual_path,
_Bool
recursive, char*** out_files, size_t* out_count);
void sandbox_filesystem_free_file_list(ecs_world_t* ecs, char** files, size_t count);
_Bool
sandbox_filesystem_read_all_bytes(ecs_world_t* ecs, const char* virtual_path, uint8_t** out_data, size_t* out_size);
void sandbox_filesystem_free_bytes(ecs_world_t* ecs, uint8_t* data);
typedef struct {
void (*trace)(ecs_world_t* ecs, const char* msg);
void (*debug)(ecs_world_t* ecs, const char* msg);
void (*info)(ecs_world_t* ecs, const char* msg);
void (*warn)(ecs_world_t* ecs, const char* msg);
void (*error)(ecs_world_t* ecs, const char* msg);
} sandbox_logs_api_t;
void sandbox_logs_trace(ecs_world_t* ecs, const char* msg);
void sandbox_logs_debug(ecs_world_t* ecs, const char* msg);
void sandbox_logs_info(ecs_world_t* ecs, const char* msg);
void sandbox_logs_warn(ecs_world_t* ecs, const char* msg);
void sandbox_logs_error(ecs_world_t* ecs, const char* msg);
typedef struct sandbox_logs_service_t { sandbox_logs_api_t* api; const sandbox_service_info_t* info; } sandbox_logs_service_t; extern ecs_entity_t FLECS_IDsandbox_logs_service_tID_; extern sandbox_service_info_t sandbox_logs_service_t_info;
typedef struct sandbox_runtime_api_t {
void (*run)(ecs_world_t* ecs);
void (*start)(ecs_world_t* ecs);
void (*stop)(ecs_world_t* ecs);
void (*pause)(ecs_world_t* ecs);
void (*resume)(ecs_world_t* ecs);
} sandbox_runtime_api_t;
typedef struct sandbox_runtime_service_t { sandbox_runtime_api_t* api; const sandbox_service_info_t* info; } sandbox_runtime_service_t; extern ecs_entity_t FLECS_IDsandbox_runtime_service_tID_; extern sandbox_service_info_t sandbox_runtime_service_t_info;
void sandbox_runtime_run(ecs_world_t* ecs);
void sandbox_runtime_start(ecs_world_t* ecs);
void sandbox_runtime_stop(ecs_world_t* ecs);
void sandbox_runtime_pause(ecs_world_t* ecs);
void sandbox_runtime_resume(ecs_world_t* ecs);
