local ffi = require('ffi')

ffi.cdef[[
    typedef struct { uintptr_t token; } sandbox_properties_handle_t;
    typedef struct { uintptr_t token; } sandbox_file_handle_t;
    typedef enum { SANDBOX_FORMAT_JSON = 0, SANDBOX_FORMAT_BEVE, SANDBOX_FORMAT_TOML, SANDBOX_FORMAT_YAML } sandbox_properties_format_t;
    typedef struct spectre_serializer_component { void* deserialize; void* serialize; } spectre_serializer_component;
    typedef ecs_entity_t (*spectre_component_registration_fn_t)(ecs_world_t* world);
    typedef struct spectre_resource_loader_component_t { void* load_fn; void* free_fn; } spectre_resource_loader_component_t;
    typedef struct spectre_script_argument_t { int dummy; } spectre_script_argument_t;
    typedef enum spectre_script_argument_type_t { DUMMY_ARG = 0 } spectre_script_argument_type_t;
    typedef void (*spectre_recursive_callback_t)(ecs_world_t*, ecs_entity_t, void*);
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
    void spectre_resources_deserialize_resource(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t spectre_resources_serialize_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
    void spectre_resources_register_resource(ecs_world_t* world, sandbox_properties_handle_t props);
    bool spectre_resources_has_resource_loader(ecs_world_t* world, const char* type);
    bool spectre_resources_has_resource(ecs_world_t* world, const char* name);
    bool spectre_resources_is_resource(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_resources_find_resource_loader(ecs_world_t* world, const char* type);
    ecs_entity_t spectre_resources_find_resource(ecs_world_t* world, const char* name);
    bool spectre_resources_is_resource_loaded(ecs_world_t* world, ecs_entity_t resource);
    void spectre_resources_load_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
    void spectre_resources_free_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
    void* spectre_resources_get_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
    void spectre_resources_deserialize_resource(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t spectre_resources_serialize_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
    void spectre_resources_register_resource_loader(ecs_world_t* world, const char* type, spectre_resource_loader_component_t loader);
    void spectre_resources_register_resource(ecs_world_t* world, sandbox_properties_handle_t props);
    bool spectre_resources_has_resource_loader(ecs_world_t* world, const char* type);
    bool spectre_resources_has_resource(ecs_world_t* world, const char* name);
    bool spectre_resources_is_resource(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_resources_find_resource_loader(ecs_world_t* world, const char* type);
    ecs_entity_t spectre_resources_find_resource(ecs_world_t* world, const char* name);
    bool spectre_resources_is_resource_loaded(ecs_world_t* world, ecs_entity_t resource);
    void spectre_resources_load_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
    void spectre_resources_free_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
    void* spectre_resources_get_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
    bool spectre_serializer_has_serializer(ecs_world_t* world, const char* type);
    bool spectre_serializer_is_serializer(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_serializer_find_serializer(ecs_world_t* world, const char* type);
    void spectre_serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_component* serializer);
    bool spectre_serializer_has_serializer(ecs_world_t* world, const char* type);
    bool spectre_serializer_is_serializer(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_serializer_find_serializer(ecs_world_t* world, const char* type);
    sandbox_properties_handle_t spectre_serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);
    void spectre_serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity, sandbox_properties_handle_t props);
    void spectre_renderer_deserialize_renderer(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t spectre_renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer);
    void spectre_renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
    bool spectre_renderer_is_renderer(ecs_world_t* world);
    void spectre_renderer_deserialize_renderer(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t spectre_renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer);
    void spectre_renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
    bool spectre_renderer_is_renderer(ecs_world_t* world);
    sandbox_properties_handle_t spectre_scenes_serialize_state(ecs_world_t* world, ecs_entity_t state);
    void spectre_scenes_deserialize_state(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t spectre_scenes_serialize_scene(ecs_world_t* world, ecs_entity_t scene);
    void spectre_scenes_deserialize_scene(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    void spectre_scenes_register_state(ecs_world_t* world, sandbox_properties_handle_t props);
    void spectre_scenes_register_scene(ecs_world_t* world, sandbox_properties_handle_t props);
    ecs_entity_t spectre_scenes_find_state(ecs_world_t* world, const char* name);
    ecs_entity_t spectre_scenes_find_scene(ecs_world_t* world, const char* name);
    bool spectre_scenes_has_state(ecs_world_t* world, const char* name);
    bool spectre_scenes_has_scene(ecs_world_t* world, const char* name);
    bool spectre_scenes_is_state(ecs_world_t* world, ecs_entity_t entity);
    bool spectre_scenes_is_scene(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_scenes_find_current_state(ecs_world_t* world);
    ecs_query_t* spectre_scenes_find_current_scenes(ecs_world_t* world);
    void spectre_scenes_push_state(ecs_world_t* world, ecs_entity_t state);
    void spectre_scenes_pop_state(ecs_world_t* world);
    sandbox_properties_handle_t spectre_scenes_serialize_state(ecs_world_t* world, ecs_entity_t state);
    void spectre_scenes_deserialize_state(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t spectre_scenes_serialize_scene(ecs_world_t* world, ecs_entity_t scene);
    void spectre_scenes_deserialize_scene(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    void spectre_scenes_register_state(ecs_world_t* world, sandbox_properties_handle_t props);
    void spectre_scenes_register_scene(ecs_world_t* world, sandbox_properties_handle_t props);
    ecs_entity_t spectre_scenes_find_state(ecs_world_t* world, const char* name);
    ecs_entity_t spectre_scenes_find_scene(ecs_world_t* world, const char* name);
    bool spectre_scenes_has_state(ecs_world_t* world, const char* name);
    bool spectre_scenes_has_scene(ecs_world_t* world, const char* name);
    bool spectre_scenes_is_state(ecs_world_t* world, ecs_entity_t entity);
    bool spectre_scenes_is_scene(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_scenes_find_current_state(ecs_world_t* world);
    void spectre_scenes_push_state(ecs_world_t* world, ecs_entity_t state);
    void spectre_scenes_pop_state(ecs_world_t* world);
    void spectre_scenes_execute_recursive(ecs_world_t* world, ecs_entity_t entity, spectre_recursive_callback_t callback, void* payload);
    void spectre_window_deserialize_window(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t spectre_window_serialize_window(ecs_world_t* world, ecs_entity_t window);
    void spectre_window_register_window(ecs_world_t* world, sandbox_properties_handle_t props);
    bool spectre_window_should_close(ecs_world_t* world);
    void spectre_window_set_should_close(ecs_world_t* world, bool close);
    void spectre_window_set_size(ecs_world_t* world, uint32_t width, uint32_t height);
    void spectre_window_set_position(ecs_world_t* world, int32_t x, int32_t y);
    uint32_t spectre_window_get_width(ecs_world_t* world);
    uint32_t spectre_window_get_height(ecs_world_t* world);
    float spectre_window_get_aspect_ratio(ecs_world_t* world);
    int32_t spectre_window_get_position_x(ecs_world_t* world);
    int32_t spectre_window_get_position_y(ecs_world_t* world);
    void spectre_window_set_title(ecs_world_t* world, const char* title);
    void spectre_window_set_vsync(ecs_world_t* world, bool enabled);
    void spectre_window_set_fullscreen(ecs_world_t* world, bool fullscreen);
    void spectre_window_set_borderless(ecs_world_t* world, bool borderless);
    void spectre_window_set_resizable(ecs_world_t* world, bool resizable);
    void spectre_window_set_always_on_top(ecs_world_t* world, bool always_on_top);
    const char* spectre_window_get_title(ecs_world_t* world);
    bool spectre_window_is_vsync(ecs_world_t* world);
    bool spectre_window_is_fullscreen(ecs_world_t* world);
    bool spectre_window_is_borderless(ecs_world_t* world);
    bool spectre_window_is_resizable(ecs_world_t* world);
    void spectre_window_minimize(ecs_world_t* world);
    void spectre_window_maximize(ecs_world_t* world);
    void spectre_window_restore(ecs_world_t* world);
    void spectre_window_show(ecs_world_t* world);
    void spectre_window_hide(ecs_world_t* world);
    void spectre_window_request_attention(ecs_world_t* world);
    bool spectre_window_is_minimized(ecs_world_t* world);
    bool spectre_window_is_maximized(ecs_world_t* world);
    bool spectre_window_is_visible(ecs_world_t* world);
    bool spectre_window_is_focused(ecs_world_t* world);
    bool spectre_window_is_hovered(ecs_world_t* world);
    void spectre_window_set_cursor_visible(ecs_world_t* world, bool visible);
    void spectre_window_set_cursor_locked(ecs_world_t* world, bool locked);
    bool spectre_window_is_cursor_visible(ecs_world_t* world);
    bool spectre_window_is_cursor_locked(ecs_world_t* world);
    void* spectre_window_get_native_handle(ecs_world_t* world);
    bool spectre_window_is_key_down(ecs_world_t* world, int keycode);
    bool spectre_window_is_key_pressed(ecs_world_t* world, int keycode);
    bool spectre_window_is_key_released(ecs_world_t* world, int keycode);
    float spectre_window_get_mouse_x(ecs_world_t* world);
    float spectre_window_get_mouse_y(ecs_world_t* world);
    float spectre_window_get_mouse_delta_x(ecs_world_t* world);
    float spectre_window_get_mouse_delta_y(ecs_world_t* world);
    void spectre_window_deserialize_window(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t spectre_window_serialize_window(ecs_world_t* world, ecs_entity_t window);
    void spectre_window_register_window(ecs_world_t* world, sandbox_properties_handle_t props);
    bool spectre_window_should_close(ecs_world_t* world);
    void spectre_window_set_should_close(ecs_world_t* world, bool close);
    void spectre_window_set_size(ecs_world_t* world, uint32_t width, uint32_t height);
    void spectre_window_set_position(ecs_world_t* world, int32_t x, int32_t y);
    void spectre_window_set_size_limits(ecs_world_t* world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);
    void spectre_window_set_title(ecs_world_t* world, const char* title);
    void spectre_window_set_vsync(ecs_world_t* world, bool enabled);
    void spectre_window_set_fullscreen(ecs_world_t* world, bool fullscreen);
    void spectre_window_set_borderless(ecs_world_t* world, bool borderless);
    void spectre_window_set_resizable(ecs_world_t* world, bool resizable);
    void spectre_window_set_always_on_top(ecs_world_t* world, bool always_on_top);
    bool spectre_window_is_vsync(ecs_world_t* world);
    bool spectre_window_is_fullscreen(ecs_world_t* world);
    bool spectre_window_is_borderless(ecs_world_t* world);
    bool spectre_window_is_resizable(ecs_world_t* world);
    void spectre_window_minimize(ecs_world_t* world);
    void spectre_window_maximize(ecs_world_t* world);
    void spectre_window_restore(ecs_world_t* world);
    void spectre_window_show(ecs_world_t* world);
    void spectre_window_hide(ecs_world_t* world);
    void spectre_window_request_attention(ecs_world_t* world);
    bool spectre_window_is_minimized(ecs_world_t* world);
    bool spectre_window_is_maximized(ecs_world_t* world);
    bool spectre_window_is_visible(ecs_world_t* world);
    bool spectre_window_is_focused(ecs_world_t* world);
    bool spectre_window_is_hovered(ecs_world_t* world);
    void spectre_window_set_cursor_visible(ecs_world_t* world, bool visible);
    void spectre_window_set_cursor_locked(ecs_world_t* world, bool locked);
    bool spectre_window_is_cursor_visible(ecs_world_t* world);
    bool spectre_window_is_cursor_locked(ecs_world_t* world);
    void* spectre_window_get_native_handle(ecs_world_t* world);
    bool spectre_window_is_key_down(ecs_world_t* world, int keycode);
    bool spectre_window_is_key_pressed(ecs_world_t* world, int keycode);
    bool spectre_window_is_key_released(ecs_world_t* world, int keycode);
    ecs_entity_t spectre_components_find_component(ecs_world_t* world, const char* name);
    bool spectre_components_has_component(ecs_world_t* world, const char* name);
    bool spectre_components_is_component(ecs_world_t* world, ecs_entity_t entity);
    void spectre_components_register_component(ecs_world_t* world, const char* name, spectre_component_registration_fn_t registration_fn, spectre_serializer_component serializer);
    ecs_entity_t spectre_components_find_component(ecs_world_t* world, const char* name);
    bool spectre_components_has_component(ecs_world_t* world, const char* name);
    bool spectre_components_is_component(ecs_world_t* world, ecs_entity_t entity);
    bool spectre_scripts_is_script(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_scripts_find_script(ecs_world_t* world, const char* function_name);
    void spectre_scripts_include_code(ecs_world_t* world, const char* path);
    sandbox_properties_handle_t spectre_scripts_serialize_scripts(ecs_world_t* world, ecs_entity_t entity);
    void spectre_scripts_deserialize_scripts(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    void spectre_scripts_execute_on_create(ecs_world_t* world, ecs_entity_t entity);
    void spectre_scripts_execute_on_destroy(ecs_world_t* world, ecs_entity_t entity);
    void spectre_scripts_execute_on_update(ecs_world_t* world, ecs_entity_t entity);
    void spectre_scripts_execute_on_enter(ecs_world_t* world, ecs_entity_t entity);
    void spectre_scripts_execute_on_exit(ecs_world_t* world, ecs_entity_t entity);
    bool spectre_scripts_has_script(ecs_world_t* world, const char* function_name, const spectre_script_argument_type_t* arg_types, size_t arg_count);
    bool spectre_scripts_is_script(ecs_world_t* world, ecs_entity_t entity);
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
    sandbox_properties_handle_t spectre_prefabs_serialize_entity(ecs_world_t* world, ecs_entity_t entity);
    void spectre_prefabs_deserialize_entity(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    void spectre_prefabs_register_prefab(ecs_world_t* world, const char* name, sandbox_properties_handle_t props);
    bool spectre_prefabs_has_prefab(ecs_world_t* world, const char* name);
    bool spectre_prefabs_is_prefab(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_prefabs_find_prefab(ecs_world_t* world, const char* name);
    ecs_entity_t spectre_prefabs_create_entity_from_props(ecs_world_t* world, sandbox_properties_handle_t props);
    ecs_entity_t spectre_prefabs_create_entity_from_prefab(ecs_world_t* world, ecs_entity_t prefab);
    ecs_entity_t spectre_prefabs_create_entity_from_name(ecs_world_t* world, const char* name);
    sandbox_properties_handle_t spectre_prefabs_serialize_entity(ecs_world_t* world, ecs_entity_t entity);
    void spectre_prefabs_deserialize_entity(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    void spectre_prefabs_register_prefab(ecs_world_t* world, const char* name, sandbox_properties_handle_t props);
    bool spectre_prefabs_has_prefab(ecs_world_t* world, const char* name);
    bool spectre_prefabs_is_prefab(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_prefabs_find_prefab(ecs_world_t* world, const char* name);
    ecs_entity_t spectre_prefabs_create_entity_from_props(ecs_world_t* world, sandbox_properties_handle_t props);
    ecs_entity_t spectre_prefabs_create_entity_from_prefab(ecs_world_t* world, ecs_entity_t prefab);
    ecs_entity_t spectre_prefabs_create_entity_from_name(ecs_world_t* world, const char* name);
    bool sandbox_stage_service(const sandbox_service_info_t* info);
    bool sandbox_stage_module(const sandbox_module_info_t* info);
    void sandbox_index_library(ecs_world_t* ecs, const char* library_path);
    sandbox_bootstrapper_t* sandbox_get_bootstrapper(ecs_world_t* ecs);
    bool sandbox_bootstrapper_activate(sandbox_bootstrapper_t* bootstrapper, ecs_world_t* ecs, const char* architecture, const char* name, int version_major, int version_minor, int version_patch);
    bool sandbox_bootstrapper_activate_string(sandbox_bootstrapper_t* bootstrapper, ecs_world_t* ecs, const char* module_str);
    bool sandbox_bootstrapper_boot(sandbox_bootstrapper_t* bootstrapper, ecs_world_t* ecs);
]]

local sandbox = {}

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

sandbox.application = {}
function sandbox.application.is_running(world)
    return ffi.C.sandbox_application_is_running((world and world.ptr) and world.ptr or world)
end

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

sandbox.configuration = {}
function sandbox.configuration.get_properties(world)
    return ffi.C.sandbox_configuration_get_properties((world and world.ptr) and world.ptr or world)
end

sandbox.logs = {}
function sandbox.logs.trace(world, msg)
    return ffi.C.sandbox_logs_trace(world.ptr or world, tostring(msg))
end
function sandbox.logs.debug(world, msg)
    return ffi.C.sandbox_logs_debug(world.ptr or world, tostring(msg))
end
function sandbox.logs.info(world, msg)
    return ffi.C.sandbox_logs_info(world.ptr or world, tostring(msg))
end
function sandbox.logs.warn(world, msg)
    return ffi.C.sandbox_logs_warn(world.ptr or world, tostring(msg))
end
function sandbox.logs.error(world, msg)
    return ffi.C.sandbox_logs_error(world.ptr or world, tostring(msg))
end

sandbox.stage = {}
function sandbox.stage.service(info)
    return ffi.C.sandbox_stage_service(info)
end
function sandbox.stage.module(info)
    return ffi.C.sandbox_stage_module(info)
end

sandbox.index = {}
function sandbox.index.library(world, library_path)
    return ffi.C.sandbox_index_library((world and world.ptr) and world.ptr or world, library_path)
end

sandbox.get = {}
function sandbox.get.bootstrapper(world)
    return ffi.C.sandbox_get_bootstrapper((world and world.ptr) and world.ptr or world)
end

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


-- Properties Wrapper Class
sandbox.Properties = {}
sandbox.Properties.__index = sandbox.Properties

function sandbox.Properties.new()
    local instance = { handle = ffi.C.sandbox_properties_create() }
    setmetatable(instance, sandbox.Properties)
    return instance
end

function sandbox.Properties:destroy()
    -- Need pointer to handle to destroy it properly
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

function sandbox.Properties:get_handle()
    return self.handle
end

return sandbox
