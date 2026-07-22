local ffi = require('ffi')

pcall(function() ffi.cdef[[
    typedef struct { uintptr_t token; } sandbox_properties_handle_t;
    typedef struct spectre_serializer_component { void* deserialize; void* serialize; } spectre_serializer_component;
    typedef uint64_t ecs_entity_t;
    typedef struct ecs_world_t ecs_world_t;
    typedef struct ecs_query_t ecs_query_t;
    typedef ecs_entity_t (*spectre_component_registration_fn_t)(ecs_world_t* world);
    typedef struct spectre_resource_loader_component_t { void* load_fn; void* free_fn; } spectre_resource_loader_component_t;
    typedef struct spectre_script_argument_t { int dummy; } spectre_script_argument_t;
    typedef struct spectre_color_t { float r, g, b, a; } spectre_color_t;
    typedef struct spectre_renderable_t { char dummy; } spectre_renderable_t;
    typedef struct spectre_2D_transform_component_t {
        float position_x, position_y, position_z;
        float scale_x, scale_y;
        float origin_x, origin_y;
        float rotation;
    } spectre_2D_transform_component_t;
    typedef struct spectre_rectange_renderable_t {
        float width; float height;
        spectre_color_t fill_color; spectre_color_t outline_color;
        float outline_thickness;
    } spectre_rectange_renderable_t;
    typedef struct spectre_scene_t { char dummy; } spectre_scene_t;
    typedef struct spectre_state_t { char dummy; } spectre_state_t;
    typedef struct spectre_window_component_t {
        uint32_t width; uint32_t height;
        int32_t position_x; int32_t position_y;
        uint32_t min_width; uint32_t min_height;
        uint32_t max_width; uint32_t max_height;
        const char* title;
        bool vsync; bool fullscreen; bool borderless; bool resizable;
        bool always_on_top; bool minimized; bool maximized;
        bool visible; bool focused; bool hovered;
        bool cursor_visible; bool cursor_locked;
        void* native_handle;
    } spectre_window_component_t;
    typedef enum spectre_script_argument_type_t { DUMMY_ARG = 0 } spectre_script_argument_type_t;
    typedef void (*spectre_recursive_callback_t)(ecs_world_t*, ecs_entity_t, void*);
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
    void spectre_resources_register_resource_loader(ecs_world_t* world, const char* type, spectre_resource_loader_component_t loader);
    bool spectre_serializer_has_serializer(ecs_world_t* world, const char* type);
    bool spectre_serializer_is_serializer(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_serializer_find_serializer(ecs_world_t* world, const char* type);
    void spectre_serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_component* serializer);
    sandbox_properties_handle_t spectre_serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);
    void spectre_serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity, sandbox_properties_handle_t props);
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
    void spectre_window_set_size_limits(ecs_world_t* world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);
    ecs_entity_t spectre_components_find_component(ecs_world_t* world, const char* name);
    bool spectre_components_has_component(ecs_world_t* world, const char* name);
    bool spectre_components_is_component(ecs_world_t* world, ecs_entity_t entity);
    void spectre_components_register_component(ecs_world_t* world, const char* name, spectre_component_registration_fn_t registration_fn, spectre_serializer_component serializer);
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
    void spectre_scripts_execute_script(ecs_world_t* world, const char* function_name, spectre_script_argument_t* args, size_t arg_count);
    sandbox_properties_handle_t spectre_prefabs_serialize_entity(ecs_world_t* world, ecs_entity_t entity);
    void spectre_prefabs_deserialize_entity(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    void spectre_prefabs_register_prefab(ecs_world_t* world, const char* name, sandbox_properties_handle_t props);
    bool spectre_prefabs_has_prefab(ecs_world_t* world, const char* name);
    bool spectre_prefabs_is_prefab(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t spectre_prefabs_find_prefab(ecs_world_t* world, const char* name);
    ecs_entity_t spectre_prefabs_create_entity_from_props(ecs_world_t* world, sandbox_properties_handle_t props);
    ecs_entity_t spectre_prefabs_create_entity_from_prefab(ecs_world_t* world, ecs_entity_t prefab);
    ecs_entity_t spectre_prefabs_create_entity_from_name(ecs_world_t* world, const char* name);
]] end)

local spectre = {}

-- ========================================
-- Resources API
-- ========================================
spectre.resources = {}

function spectre.resources.deserialize_resource(world, target, props)
    return ffi.C.spectre_resources_deserialize_resource((type(world) == "table" and world.ptr) and world.ptr or world, target, props)
end

function spectre.resources.serialize_resource(world, resourceEntity)
    return ffi.C.spectre_resources_serialize_resource((type(world) == "table" and world.ptr) and world.ptr or world, resourceEntity)
end

function spectre.resources.register_resource(world, props)
    return ffi.C.spectre_resources_register_resource((type(world) == "table" and world.ptr) and world.ptr or world, props)
end

function spectre.resources.has_resource_loader(world, type)
    return ffi.C.spectre_resources_has_resource_loader((type(world) == "table" and world.ptr) and world.ptr or world, type)
end

function spectre.resources.has_resource(world, name)
    return ffi.C.spectre_resources_has_resource((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.resources.is_resource(world, entity)
    return ffi.C.spectre_resources_is_resource((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.resources.find_resource_loader(world, type)
    return ffi.C.spectre_resources_find_resource_loader((type(world) == "table" and world.ptr) and world.ptr or world, type)
end

function spectre.resources.find_resource(world, name)
    return ffi.C.spectre_resources_find_resource((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.resources.is_resource_loaded(world, resource)
    return ffi.C.spectre_resources_is_resource_loaded((type(world) == "table" and world.ptr) and world.ptr or world, resource)
end

function spectre.resources.load_resource(world, resourceEntity)
    return ffi.C.spectre_resources_load_resource((type(world) == "table" and world.ptr) and world.ptr or world, resourceEntity)
end

function spectre.resources.free_resource(world, resourceEntity)
    return ffi.C.spectre_resources_free_resource((type(world) == "table" and world.ptr) and world.ptr or world, resourceEntity)
end

function spectre.resources.get_resource(world, resourceEntity)
    return ffi.C.spectre_resources_get_resource((type(world) == "table" and world.ptr) and world.ptr or world, resourceEntity)
end

function spectre.resources.register_resource_loader(world, type, loader)
    return ffi.C.spectre_resources_register_resource_loader((type(world) == "table" and world.ptr) and world.ptr or world, type, loader)
end

-- ========================================
-- Serializer API
-- ========================================
spectre.serializer = {}

function spectre.serializer.has_serializer(world, type)
    return ffi.C.spectre_serializer_has_serializer((type(world) == "table" and world.ptr) and world.ptr or world, type)
end

function spectre.serializer.is_serializer(world, entity)
    return ffi.C.spectre_serializer_is_serializer((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.serializer.find_serializer(world, type)
    return ffi.C.spectre_serializer_find_serializer((type(world) == "table" and world.ptr) and world.ptr or world, type)
end

function spectre.serializer.register_serializer(world, type, serializer)
    return ffi.C.spectre_serializer_register_serializer((type(world) == "table" and world.ptr) and world.ptr or world, type, serializer)
end

function spectre.serializer.serialize_entity(world, serializer, entity)
    return ffi.C.spectre_serializer_serialize_entity((type(world) == "table" and world.ptr) and world.ptr or world, serializer, entity)
end

function spectre.serializer.deserialize_entity(world, serializer, entity, props)
    return ffi.C.spectre_serializer_deserialize_entity((type(world) == "table" and world.ptr) and world.ptr or world, serializer, entity, props)
end

-- ========================================
-- Renderer API
-- ========================================
spectre.renderer = {}

function spectre.renderer.deserialize_renderer(world, target, props)
    return ffi.C.spectre_renderer_deserialize_renderer((type(world) == "table" and world.ptr) and world.ptr or world, target, props)
end

function spectre.renderer.serialize_renderer(world, renderer)
    return ffi.C.spectre_renderer_serialize_renderer((type(world) == "table" and world.ptr) and world.ptr or world, renderer)
end

function spectre.renderer.register_renderer(world, props)
    return ffi.C.spectre_renderer_register_renderer((type(world) == "table" and world.ptr) and world.ptr or world, props)
end

function spectre.renderer.is_renderer(world)
    return ffi.C.spectre_renderer_is_renderer((type(world) == "table" and world.ptr) and world.ptr or world)
end

-- ========================================
-- Scenes API
-- ========================================
spectre.scenes = {}

function spectre.scenes.serialize_state(world, state)
    return ffi.C.spectre_scenes_serialize_state((type(world) == "table" and world.ptr) and world.ptr or world, state)
end

function spectre.scenes.deserialize_state(world, target, props)
    return ffi.C.spectre_scenes_deserialize_state((type(world) == "table" and world.ptr) and world.ptr or world, target, props)
end

function spectre.scenes.serialize_scene(world, scene)
    return ffi.C.spectre_scenes_serialize_scene((type(world) == "table" and world.ptr) and world.ptr or world, scene)
end

function spectre.scenes.deserialize_scene(world, target, props)
    return ffi.C.spectre_scenes_deserialize_scene((type(world) == "table" and world.ptr) and world.ptr or world, target, props)
end

function spectre.scenes.register_state(world, props)
    return ffi.C.spectre_scenes_register_state((type(world) == "table" and world.ptr) and world.ptr or world, props)
end

function spectre.scenes.register_scene(world, props)
    return ffi.C.spectre_scenes_register_scene((type(world) == "table" and world.ptr) and world.ptr or world, props)
end

function spectre.scenes.find_state(world, name)
    return ffi.C.spectre_scenes_find_state((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.scenes.find_scene(world, name)
    return ffi.C.spectre_scenes_find_scene((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.scenes.has_state(world, name)
    return ffi.C.spectre_scenes_has_state((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.scenes.has_scene(world, name)
    return ffi.C.spectre_scenes_has_scene((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.scenes.is_state(world, entity)
    return ffi.C.spectre_scenes_is_state((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.scenes.is_scene(world, entity)
    return ffi.C.spectre_scenes_is_scene((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.scenes.find_current_state(world)
    return ffi.C.spectre_scenes_find_current_state((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.scenes.find_current_scenes(world)
    return ffi.C.spectre_scenes_find_current_scenes((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.scenes.push_state(world, state)
    return ffi.C.spectre_scenes_push_state((type(world) == "table" and world.ptr) and world.ptr or world, state)
end

function spectre.scenes.pop_state(world)
    return ffi.C.spectre_scenes_pop_state((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.scenes.execute_recursive(world, entity, callback, payload)
    return ffi.C.spectre_scenes_execute_recursive((type(world) == "table" and world.ptr) and world.ptr or world, entity, callback, payload)
end

-- ========================================
-- Window API
-- ========================================
spectre.window = {}

function spectre.window.deserialize_window(world, target, props)
    return ffi.C.spectre_window_deserialize_window((type(world) == "table" and world.ptr) and world.ptr or world, target, props)
end

function spectre.window.serialize_window(world, window)
    return ffi.C.spectre_window_serialize_window((type(world) == "table" and world.ptr) and world.ptr or world, window)
end

function spectre.window.register_window(world, props)
    return ffi.C.spectre_window_register_window((type(world) == "table" and world.ptr) and world.ptr or world, props)
end

function spectre.window.should_close(world)
    return ffi.C.spectre_window_should_close((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.set_should_close(world, close)
    return ffi.C.spectre_window_set_should_close((type(world) == "table" and world.ptr) and world.ptr or world, close)
end

function spectre.window.set_size(world, width, height)
    return ffi.C.spectre_window_set_size((type(world) == "table" and world.ptr) and world.ptr or world, width, height)
end

function spectre.window.set_position(world, x, y)
    return ffi.C.spectre_window_set_position((type(world) == "table" and world.ptr) and world.ptr or world, x, y)
end

function spectre.window.get_width(world)
    return ffi.C.spectre_window_get_width((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.get_height(world)
    return ffi.C.spectre_window_get_height((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.get_aspect_ratio(world)
    return ffi.C.spectre_window_get_aspect_ratio((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.get_position_x(world)
    return ffi.C.spectre_window_get_position_x((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.get_position_y(world)
    return ffi.C.spectre_window_get_position_y((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.set_title(world, title)
    return ffi.C.spectre_window_set_title((type(world) == "table" and world.ptr) and world.ptr or world, title)
end

function spectre.window.set_vsync(world, enabled)
    return ffi.C.spectre_window_set_vsync((type(world) == "table" and world.ptr) and world.ptr or world, enabled)
end

function spectre.window.set_fullscreen(world, fullscreen)
    return ffi.C.spectre_window_set_fullscreen((type(world) == "table" and world.ptr) and world.ptr or world, fullscreen)
end

function spectre.window.set_borderless(world, borderless)
    return ffi.C.spectre_window_set_borderless((type(world) == "table" and world.ptr) and world.ptr or world, borderless)
end

function spectre.window.set_resizable(world, resizable)
    return ffi.C.spectre_window_set_resizable((type(world) == "table" and world.ptr) and world.ptr or world, resizable)
end

function spectre.window.set_always_on_top(world, always_on_top)
    return ffi.C.spectre_window_set_always_on_top((type(world) == "table" and world.ptr) and world.ptr or world, always_on_top)
end

function spectre.window.get_title(world)
    return ffi.C.spectre_window_get_title((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_vsync(world)
    return ffi.C.spectre_window_is_vsync((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_fullscreen(world)
    return ffi.C.spectre_window_is_fullscreen((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_borderless(world)
    return ffi.C.spectre_window_is_borderless((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_resizable(world)
    return ffi.C.spectre_window_is_resizable((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.minimize(world)
    return ffi.C.spectre_window_minimize((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.maximize(world)
    return ffi.C.spectre_window_maximize((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.restore(world)
    return ffi.C.spectre_window_restore((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.show(world)
    return ffi.C.spectre_window_show((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.hide(world)
    return ffi.C.spectre_window_hide((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.request_attention(world)
    return ffi.C.spectre_window_request_attention((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_minimized(world)
    return ffi.C.spectre_window_is_minimized((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_maximized(world)
    return ffi.C.spectre_window_is_maximized((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_visible(world)
    return ffi.C.spectre_window_is_visible((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_focused(world)
    return ffi.C.spectre_window_is_focused((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_hovered(world)
    return ffi.C.spectre_window_is_hovered((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.set_cursor_visible(world, visible)
    return ffi.C.spectre_window_set_cursor_visible((type(world) == "table" and world.ptr) and world.ptr or world, visible)
end

function spectre.window.set_cursor_locked(world, locked)
    return ffi.C.spectre_window_set_cursor_locked((type(world) == "table" and world.ptr) and world.ptr or world, locked)
end

function spectre.window.is_cursor_visible(world)
    return ffi.C.spectre_window_is_cursor_visible((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_cursor_locked(world)
    return ffi.C.spectre_window_is_cursor_locked((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.get_native_handle(world)
    return ffi.C.spectre_window_get_native_handle((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.is_key_down(world, keycode)
    return ffi.C.spectre_window_is_key_down((type(world) == "table" and world.ptr) and world.ptr or world, keycode)
end

function spectre.window.is_key_pressed(world, keycode)
    return ffi.C.spectre_window_is_key_pressed((type(world) == "table" and world.ptr) and world.ptr or world, keycode)
end

function spectre.window.is_key_released(world, keycode)
    return ffi.C.spectre_window_is_key_released((type(world) == "table" and world.ptr) and world.ptr or world, keycode)
end

function spectre.window.get_mouse_x(world)
    return ffi.C.spectre_window_get_mouse_x((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.get_mouse_y(world)
    return ffi.C.spectre_window_get_mouse_y((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.get_mouse_delta_x(world)
    return ffi.C.spectre_window_get_mouse_delta_x((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.get_mouse_delta_y(world)
    return ffi.C.spectre_window_get_mouse_delta_y((type(world) == "table" and world.ptr) and world.ptr or world)
end

function spectre.window.set_size_limits(world, min_width, min_height, max_width, max_height)
    return ffi.C.spectre_window_set_size_limits((type(world) == "table" and world.ptr) and world.ptr or world, min_width, min_height, max_width, max_height)
end

-- ========================================
-- Components API
-- ========================================
spectre.components = {}

function spectre.components.find_component(world, name)
    return ffi.C.spectre_components_find_component((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.components.has_component(world, name)
    return ffi.C.spectre_components_has_component((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.components.is_component(world, entity)
    return ffi.C.spectre_components_is_component((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.components.register_component(world, name, registration_fn, serializer)
    return ffi.C.spectre_components_register_component((type(world) == "table" and world.ptr) and world.ptr or world, name, registration_fn, serializer)
end

-- ========================================
-- Scripts API
-- ========================================
spectre.scripts = {}

function spectre.scripts.is_script(world, entity)
    return ffi.C.spectre_scripts_is_script((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.scripts.find_script(world, function_name)
    return ffi.C.spectre_scripts_find_script((type(world) == "table" and world.ptr) and world.ptr or world, function_name)
end

function spectre.scripts.include_code(world, path)
    return ffi.C.spectre_scripts_include_code((type(world) == "table" and world.ptr) and world.ptr or world, path)
end

function spectre.scripts.serialize_scripts(world, entity)
    return ffi.C.spectre_scripts_serialize_scripts((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.scripts.deserialize_scripts(world, target, props)
    return ffi.C.spectre_scripts_deserialize_scripts((type(world) == "table" and world.ptr) and world.ptr or world, target, props)
end

function spectre.scripts.execute_on_create(world, entity)
    return ffi.C.spectre_scripts_execute_on_create((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.scripts.execute_on_destroy(world, entity)
    return ffi.C.spectre_scripts_execute_on_destroy((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.scripts.execute_on_update(world, entity)
    return ffi.C.spectre_scripts_execute_on_update((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.scripts.execute_on_enter(world, entity)
    return ffi.C.spectre_scripts_execute_on_enter((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.scripts.execute_on_exit(world, entity)
    return ffi.C.spectre_scripts_execute_on_exit((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.scripts.has_script(world, function_name, arg_types, arg_count)
    return ffi.C.spectre_scripts_has_script((type(world) == "table" and world.ptr) and world.ptr or world, function_name, arg_types, arg_count)
end

function spectre.scripts.execute_script(world, function_name, args, arg_count)
    return ffi.C.spectre_scripts_execute_script((type(world) == "table" and world.ptr) and world.ptr or world, function_name, args, arg_count)
end

-- ========================================
-- Prefabs API
-- ========================================
spectre.prefabs = {}

function spectre.prefabs.serialize_entity(world, entity)
    return ffi.C.spectre_prefabs_serialize_entity((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.prefabs.deserialize_entity(world, target, props)
    return ffi.C.spectre_prefabs_deserialize_entity((type(world) == "table" and world.ptr) and world.ptr or world, target, props)
end

function spectre.prefabs.register_prefab(world, name, props)
    return ffi.C.spectre_prefabs_register_prefab((type(world) == "table" and world.ptr) and world.ptr or world, name, props)
end

function spectre.prefabs.has_prefab(world, name)
    return ffi.C.spectre_prefabs_has_prefab((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.prefabs.is_prefab(world, entity)
    return ffi.C.spectre_prefabs_is_prefab((type(world) == "table" and world.ptr) and world.ptr or world, entity)
end

function spectre.prefabs.find_prefab(world, name)
    return ffi.C.spectre_prefabs_find_prefab((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.prefabs.create_entity_from_props(world, props)
    return ffi.C.spectre_prefabs_create_entity_from_props((type(world) == "table" and world.ptr) and world.ptr or world, props)
end

function spectre.prefabs.create_entity_from_prefab(world, prefab)
    return ffi.C.spectre_prefabs_create_entity_from_prefab((type(world) == "table" and world.ptr) and world.ptr or world, prefab)
end

function spectre.prefabs.create_entity_from_name(world, name)
    return ffi.C.spectre_prefabs_create_entity_from_name((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.init(world)
    spectre.Transform2D = spectre.components.find_component(world, "Transform2D")
    if spectre.Transform2D ~= 0 then world:bind_struct(spectre.Transform2D, "spectre_2D_transform_component_t") end
    spectre.Renderable = spectre.components.find_component(world, "Renderable")
    if spectre.Renderable ~= 0 then world:bind_struct(spectre.Renderable, "spectre_renderable_t") end
    spectre.RectangleRenderable = spectre.components.find_component(world, "RectangeRenderable")
    if spectre.RectangleRenderable ~= 0 then world:bind_struct(spectre.RectangleRenderable, "spectre_rectange_renderable_t") end
    spectre.Scene = spectre.components.find_component(world, "Scene")
    if spectre.Scene ~= 0 then world:bind_struct(spectre.Scene, "spectre_scene_t") end
    spectre.State = spectre.components.find_component(world, "State")
    if spectre.State ~= 0 then world:bind_struct(spectre.State, "spectre_state_t") end
    spectre.Window = spectre.components.find_component(world, "Window")
    if spectre.Window ~= 0 then world:bind_struct(spectre.Window, "spectre_window_component_t") end
end

return spectre
