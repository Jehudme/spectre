local ffi = require('ffi')

local spectre = {}

spectre.resources = {}
function spectre.resources.deserialize_resource(world, target, props)
    return ffi.C.spectre_resources_deserialize_resource(world, target, props)
end
function spectre.resources.serialize_resource(world, resourceEntity)
    return ffi.C.spectre_resources_serialize_resource(world, resourceEntity)
end
function spectre.resources.register_resource(world, props)
    return ffi.C.spectre_resources_register_resource(world, props)
end
function spectre.resources.has_resource_loader(world, type)
    return ffi.C.spectre_resources_has_resource_loader(world, type)
end
function spectre.resources.has_resource(world, name)
    return ffi.C.spectre_resources_has_resource(world, name)
end
function spectre.resources.is_resource(world, entity)
    return ffi.C.spectre_resources_is_resource(world, entity)
end
function spectre.resources.find_resource_loader(world, type)
    return ffi.C.spectre_resources_find_resource_loader(world, type)
end
function spectre.resources.find_resource(world, name)
    return ffi.C.spectre_resources_find_resource(world, name)
end
function spectre.resources.is_resource_loaded(world, resource)
    return ffi.C.spectre_resources_is_resource_loaded(world, resource)
end
function spectre.resources.load_resource(world, resourceEntity)
    return ffi.C.spectre_resources_load_resource(world, resourceEntity)
end
function spectre.resources.free_resource(world, resourceEntity)
    return ffi.C.spectre_resources_free_resource(world, resourceEntity)
end
function spectre.resources.get_resource(world, resourceEntity)
    return ffi.C.spectre_resources_get_resource(world, resourceEntity)
end
function spectre.resources.deserialize_resource(world, target, props)
    return ffi.C.spectre_resources_deserialize_resource(world, target, props)
end
function spectre.resources.serialize_resource(world, resourceEntity)
    return ffi.C.spectre_resources_serialize_resource(world, resourceEntity)
end
function spectre.resources.register_resource_loader(world, type, loader)
    return ffi.C.spectre_resources_register_resource_loader(world, type, loader)
end
function spectre.resources.register_resource(world, props)
    return ffi.C.spectre_resources_register_resource(world, props)
end
function spectre.resources.has_resource_loader(world, type)
    return ffi.C.spectre_resources_has_resource_loader(world, type)
end
function spectre.resources.has_resource(world, name)
    return ffi.C.spectre_resources_has_resource(world, name)
end
function spectre.resources.is_resource(world, entity)
    return ffi.C.spectre_resources_is_resource(world, entity)
end
function spectre.resources.find_resource_loader(world, type)
    return ffi.C.spectre_resources_find_resource_loader(world, type)
end
function spectre.resources.find_resource(world, name)
    return ffi.C.spectre_resources_find_resource(world, name)
end
function spectre.resources.is_resource_loaded(world, resource)
    return ffi.C.spectre_resources_is_resource_loaded(world, resource)
end
function spectre.resources.load_resource(world, resourceEntity)
    return ffi.C.spectre_resources_load_resource(world, resourceEntity)
end
function spectre.resources.free_resource(world, resourceEntity)
    return ffi.C.spectre_resources_free_resource(world, resourceEntity)
end
function spectre.resources.get_resource(world, resourceEntity)
    return ffi.C.spectre_resources_get_resource(world, resourceEntity)
end

spectre.serializer = {}
function spectre.serializer.has_serializer(world, type)
    return ffi.C.spectre_serializer_has_serializer(world, type)
end
function spectre.serializer.is_serializer(world, entity)
    return ffi.C.spectre_serializer_is_serializer(world, entity)
end
function spectre.serializer.find_serializer(world, type)
    return ffi.C.spectre_serializer_find_serializer(world, type)
end
function spectre.serializer.register_serializer(world, type, serializer)
    return ffi.C.spectre_serializer_register_serializer(world, type, serializer)
end
function spectre.serializer.has_serializer(world, type)
    return ffi.C.spectre_serializer_has_serializer(world, type)
end
function spectre.serializer.is_serializer(world, entity)
    return ffi.C.spectre_serializer_is_serializer(world, entity)
end
function spectre.serializer.find_serializer(world, type)
    return ffi.C.spectre_serializer_find_serializer(world, type)
end
function spectre.serializer.serialize_entity(world, serializer, entity)
    return ffi.C.spectre_serializer_serialize_entity(world, serializer, entity)
end
function spectre.serializer.deserialize_entity(world, serializer, entity, props)
    return ffi.C.spectre_serializer_deserialize_entity(world, serializer, entity, props)
end

spectre.renderer = {}
function spectre.renderer.deserialize_renderer(world, target, props)
    return ffi.C.spectre_renderer_deserialize_renderer(world, target, props)
end
function spectre.renderer.serialize_renderer(world, renderer)
    return ffi.C.spectre_renderer_serialize_renderer(world, renderer)
end
function spectre.renderer.register_renderer(world, props)
    return ffi.C.spectre_renderer_register_renderer(world, props)
end
function spectre.renderer.is_renderer(world)
    return ffi.C.spectre_renderer_is_renderer(world)
end
function spectre.renderer.deserialize_renderer(world, target, props)
    return ffi.C.spectre_renderer_deserialize_renderer(world, target, props)
end
function spectre.renderer.serialize_renderer(world, renderer)
    return ffi.C.spectre_renderer_serialize_renderer(world, renderer)
end
function spectre.renderer.register_renderer(world, props)
    return ffi.C.spectre_renderer_register_renderer(world, props)
end
function spectre.renderer.is_renderer(world)
    return ffi.C.spectre_renderer_is_renderer(world)
end

spectre.scenes = {}
function spectre.scenes.serialize_state(world, state)
    return ffi.C.spectre_scenes_serialize_state(world, state)
end
function spectre.scenes.deserialize_state(world, target, props)
    return ffi.C.spectre_scenes_deserialize_state(world, target, props)
end
function spectre.scenes.serialize_scene(world, scene)
    return ffi.C.spectre_scenes_serialize_scene(world, scene)
end
function spectre.scenes.deserialize_scene(world, target, props)
    return ffi.C.spectre_scenes_deserialize_scene(world, target, props)
end
function spectre.scenes.register_state(world, props)
    return ffi.C.spectre_scenes_register_state(world, props)
end
function spectre.scenes.register_scene(world, props)
    return ffi.C.spectre_scenes_register_scene(world, props)
end
function spectre.scenes.find_state(world, name)
    return ffi.C.spectre_scenes_find_state(world, name)
end
function spectre.scenes.find_scene(world, name)
    return ffi.C.spectre_scenes_find_scene(world, name)
end
function spectre.scenes.has_state(world, name)
    return ffi.C.spectre_scenes_has_state(world, name)
end
function spectre.scenes.has_scene(world, name)
    return ffi.C.spectre_scenes_has_scene(world, name)
end
function spectre.scenes.is_state(world, entity)
    return ffi.C.spectre_scenes_is_state(world, entity)
end
function spectre.scenes.is_scene(world, entity)
    return ffi.C.spectre_scenes_is_scene(world, entity)
end
function spectre.scenes.find_current_state(world)
    return ffi.C.spectre_scenes_find_current_state(world)
end
function spectre.scenes.find_current_scenes(world)
    return ffi.C.spectre_scenes_find_current_scenes(world)
end
function spectre.scenes.push_state(world, state)
    return ffi.C.spectre_scenes_push_state(world, state)
end
function spectre.scenes.pop_state(world)
    return ffi.C.spectre_scenes_pop_state(world)
end
function spectre.scenes.serialize_state(world, state)
    return ffi.C.spectre_scenes_serialize_state(world, state)
end
function spectre.scenes.deserialize_state(world, target, props)
    return ffi.C.spectre_scenes_deserialize_state(world, target, props)
end
function spectre.scenes.serialize_scene(world, scene)
    return ffi.C.spectre_scenes_serialize_scene(world, scene)
end
function spectre.scenes.deserialize_scene(world, target, props)
    return ffi.C.spectre_scenes_deserialize_scene(world, target, props)
end
function spectre.scenes.register_state(world, props)
    return ffi.C.spectre_scenes_register_state(world, props)
end
function spectre.scenes.register_scene(world, props)
    return ffi.C.spectre_scenes_register_scene(world, props)
end
function spectre.scenes.find_state(world, name)
    return ffi.C.spectre_scenes_find_state(world, name)
end
function spectre.scenes.find_scene(world, name)
    return ffi.C.spectre_scenes_find_scene(world, name)
end
function spectre.scenes.has_state(world, name)
    return ffi.C.spectre_scenes_has_state(world, name)
end
function spectre.scenes.has_scene(world, name)
    return ffi.C.spectre_scenes_has_scene(world, name)
end
function spectre.scenes.is_state(world, entity)
    return ffi.C.spectre_scenes_is_state(world, entity)
end
function spectre.scenes.is_scene(world, entity)
    return ffi.C.spectre_scenes_is_scene(world, entity)
end
function spectre.scenes.find_current_state(world)
    return ffi.C.spectre_scenes_find_current_state(world)
end
function spectre.scenes.push_state(world, state)
    return ffi.C.spectre_scenes_push_state(world, state)
end
function spectre.scenes.pop_state(world)
    return ffi.C.spectre_scenes_pop_state(world)
end
function spectre.scenes.execute_recursive(world, entity, callback, payload)
    return ffi.C.spectre_scenes_execute_recursive(world, entity, callback, payload)
end

spectre.window = {}
function spectre.window.deserialize_window(world, target, props)
    return ffi.C.spectre_window_deserialize_window(world, target, props)
end
function spectre.window.serialize_window(world, window)
    return ffi.C.spectre_window_serialize_window(world, window)
end
function spectre.window.register_window(world, props)
    return ffi.C.spectre_window_register_window(world, props)
end
function spectre.window.should_close(world)
    return ffi.C.spectre_window_should_close(world)
end
function spectre.window.set_should_close(world, close)
    return ffi.C.spectre_window_set_should_close(world, close)
end
function spectre.window.set_size(world, width, height)
    return ffi.C.spectre_window_set_size(world, width, height)
end
function spectre.window.set_position(world, x, y)
    return ffi.C.spectre_window_set_position(world, x, y)
end
function spectre.window.get_width(world)
    return ffi.C.spectre_window_get_width(world)
end
function spectre.window.get_height(world)
    return ffi.C.spectre_window_get_height(world)
end
function spectre.window.get_aspect_ratio(world)
    return ffi.C.spectre_window_get_aspect_ratio(world)
end
function spectre.window.get_position_x(world)
    return ffi.C.spectre_window_get_position_x(world)
end
function spectre.window.get_position_y(world)
    return ffi.C.spectre_window_get_position_y(world)
end
function spectre.window.set_title(world, title)
    return ffi.C.spectre_window_set_title(world, title)
end
function spectre.window.set_vsync(world, enabled)
    return ffi.C.spectre_window_set_vsync(world, enabled)
end
function spectre.window.set_fullscreen(world, fullscreen)
    return ffi.C.spectre_window_set_fullscreen(world, fullscreen)
end
function spectre.window.set_borderless(world, borderless)
    return ffi.C.spectre_window_set_borderless(world, borderless)
end
function spectre.window.set_resizable(world, resizable)
    return ffi.C.spectre_window_set_resizable(world, resizable)
end
function spectre.window.set_always_on_top(world, always_on_top)
    return ffi.C.spectre_window_set_always_on_top(world, always_on_top)
end
function spectre.window.get_title(world)
    return ffi.C.spectre_window_get_title(world)
end
function spectre.window.is_vsync(world)
    return ffi.C.spectre_window_is_vsync(world)
end
function spectre.window.is_fullscreen(world)
    return ffi.C.spectre_window_is_fullscreen(world)
end
function spectre.window.is_borderless(world)
    return ffi.C.spectre_window_is_borderless(world)
end
function spectre.window.is_resizable(world)
    return ffi.C.spectre_window_is_resizable(world)
end
function spectre.window.minimize(world)
    return ffi.C.spectre_window_minimize(world)
end
function spectre.window.maximize(world)
    return ffi.C.spectre_window_maximize(world)
end
function spectre.window.restore(world)
    return ffi.C.spectre_window_restore(world)
end
function spectre.window.show(world)
    return ffi.C.spectre_window_show(world)
end
function spectre.window.hide(world)
    return ffi.C.spectre_window_hide(world)
end
function spectre.window.request_attention(world)
    return ffi.C.spectre_window_request_attention(world)
end
function spectre.window.is_minimized(world)
    return ffi.C.spectre_window_is_minimized(world)
end
function spectre.window.is_maximized(world)
    return ffi.C.spectre_window_is_maximized(world)
end
function spectre.window.is_visible(world)
    return ffi.C.spectre_window_is_visible(world)
end
function spectre.window.is_focused(world)
    return ffi.C.spectre_window_is_focused(world)
end
function spectre.window.is_hovered(world)
    return ffi.C.spectre_window_is_hovered(world)
end
function spectre.window.set_cursor_visible(world, visible)
    return ffi.C.spectre_window_set_cursor_visible(world, visible)
end
function spectre.window.set_cursor_locked(world, locked)
    return ffi.C.spectre_window_set_cursor_locked(world, locked)
end
function spectre.window.is_cursor_visible(world)
    return ffi.C.spectre_window_is_cursor_visible(world)
end
function spectre.window.is_cursor_locked(world)
    return ffi.C.spectre_window_is_cursor_locked(world)
end
function spectre.window.get_native_handle(world)
    return ffi.C.spectre_window_get_native_handle(world)
end
function spectre.window.is_key_down(world, keycode)
    return ffi.C.spectre_window_is_key_down(world, keycode)
end
function spectre.window.is_key_pressed(world, keycode)
    return ffi.C.spectre_window_is_key_pressed(world, keycode)
end
function spectre.window.is_key_released(world, keycode)
    return ffi.C.spectre_window_is_key_released(world, keycode)
end
function spectre.window.get_mouse_x(world)
    return ffi.C.spectre_window_get_mouse_x(world)
end
function spectre.window.get_mouse_y(world)
    return ffi.C.spectre_window_get_mouse_y(world)
end
function spectre.window.get_mouse_delta_x(world)
    return ffi.C.spectre_window_get_mouse_delta_x(world)
end
function spectre.window.get_mouse_delta_y(world)
    return ffi.C.spectre_window_get_mouse_delta_y(world)
end
function spectre.window.deserialize_window(world, target, props)
    return ffi.C.spectre_window_deserialize_window(world, target, props)
end
function spectre.window.serialize_window(world, window)
    return ffi.C.spectre_window_serialize_window(world, window)
end
function spectre.window.register_window(world, props)
    return ffi.C.spectre_window_register_window(world, props)
end
function spectre.window.should_close(world)
    return ffi.C.spectre_window_should_close(world)
end
function spectre.window.set_should_close(world, close)
    return ffi.C.spectre_window_set_should_close(world, close)
end
function spectre.window.set_size(world, width, height)
    return ffi.C.spectre_window_set_size(world, width, height)
end
function spectre.window.set_position(world, x, y)
    return ffi.C.spectre_window_set_position(world, x, y)
end
function spectre.window.set_size_limits(world, min_width, min_height, max_width, max_height)
    return ffi.C.spectre_window_set_size_limits(world, min_width, min_height, max_width, max_height)
end
function spectre.window.set_title(world, title)
    return ffi.C.spectre_window_set_title(world, title)
end
function spectre.window.set_vsync(world, enabled)
    return ffi.C.spectre_window_set_vsync(world, enabled)
end
function spectre.window.set_fullscreen(world, fullscreen)
    return ffi.C.spectre_window_set_fullscreen(world, fullscreen)
end
function spectre.window.set_borderless(world, borderless)
    return ffi.C.spectre_window_set_borderless(world, borderless)
end
function spectre.window.set_resizable(world, resizable)
    return ffi.C.spectre_window_set_resizable(world, resizable)
end
function spectre.window.set_always_on_top(world, always_on_top)
    return ffi.C.spectre_window_set_always_on_top(world, always_on_top)
end
function spectre.window.is_vsync(world)
    return ffi.C.spectre_window_is_vsync(world)
end
function spectre.window.is_fullscreen(world)
    return ffi.C.spectre_window_is_fullscreen(world)
end
function spectre.window.is_borderless(world)
    return ffi.C.spectre_window_is_borderless(world)
end
function spectre.window.is_resizable(world)
    return ffi.C.spectre_window_is_resizable(world)
end
function spectre.window.minimize(world)
    return ffi.C.spectre_window_minimize(world)
end
function spectre.window.maximize(world)
    return ffi.C.spectre_window_maximize(world)
end
function spectre.window.restore(world)
    return ffi.C.spectre_window_restore(world)
end
function spectre.window.show(world)
    return ffi.C.spectre_window_show(world)
end
function spectre.window.hide(world)
    return ffi.C.spectre_window_hide(world)
end
function spectre.window.request_attention(world)
    return ffi.C.spectre_window_request_attention(world)
end
function spectre.window.is_minimized(world)
    return ffi.C.spectre_window_is_minimized(world)
end
function spectre.window.is_maximized(world)
    return ffi.C.spectre_window_is_maximized(world)
end
function spectre.window.is_visible(world)
    return ffi.C.spectre_window_is_visible(world)
end
function spectre.window.is_focused(world)
    return ffi.C.spectre_window_is_focused(world)
end
function spectre.window.is_hovered(world)
    return ffi.C.spectre_window_is_hovered(world)
end
function spectre.window.set_cursor_visible(world, visible)
    return ffi.C.spectre_window_set_cursor_visible(world, visible)
end
function spectre.window.set_cursor_locked(world, locked)
    return ffi.C.spectre_window_set_cursor_locked(world, locked)
end
function spectre.window.is_cursor_visible(world)
    return ffi.C.spectre_window_is_cursor_visible(world)
end
function spectre.window.is_cursor_locked(world)
    return ffi.C.spectre_window_is_cursor_locked(world)
end
function spectre.window.get_native_handle(world)
    return ffi.C.spectre_window_get_native_handle(world)
end
function spectre.window.is_key_down(world, keycode)
    return ffi.C.spectre_window_is_key_down(world, keycode)
end
function spectre.window.is_key_pressed(world, keycode)
    return ffi.C.spectre_window_is_key_pressed(world, keycode)
end
function spectre.window.is_key_released(world, keycode)
    return ffi.C.spectre_window_is_key_released(world, keycode)
end

spectre.components = {}
function spectre.components.find_component(world, name)
    return ffi.C.spectre_components_find_component(world, name)
end
function spectre.components.has_component(world, name)
    return ffi.C.spectre_components_has_component(world, name)
end
function spectre.components.is_component(world, entity)
    return ffi.C.spectre_components_is_component(world, entity)
end
function spectre.components.register_component(world, name, registration_fn, serializer)
    return ffi.C.spectre_components_register_component(world, name, registration_fn, serializer)
end
function spectre.components.find_component(world, name)
    return ffi.C.spectre_components_find_component(world, name)
end
function spectre.components.has_component(world, name)
    return ffi.C.spectre_components_has_component(world, name)
end
function spectre.components.is_component(world, entity)
    return ffi.C.spectre_components_is_component(world, entity)
end

spectre.scripts = {}
function spectre.scripts.is_script(world, entity)
    return ffi.C.spectre_scripts_is_script(world, entity)
end
function spectre.scripts.find_script(world, function_name)
    return ffi.C.spectre_scripts_find_script(world, function_name)
end
function spectre.scripts.include_code(world, path)
    return ffi.C.spectre_scripts_include_code(world, path)
end
function spectre.scripts.serialize_scripts(world, entity)
    return ffi.C.spectre_scripts_serialize_scripts(world, entity)
end
function spectre.scripts.deserialize_scripts(world, target, props)
    return ffi.C.spectre_scripts_deserialize_scripts(world, target, props)
end
function spectre.scripts.execute_on_create(world, entity)
    return ffi.C.spectre_scripts_execute_on_create(world, entity)
end
function spectre.scripts.execute_on_destroy(world, entity)
    return ffi.C.spectre_scripts_execute_on_destroy(world, entity)
end
function spectre.scripts.execute_on_update(world, entity)
    return ffi.C.spectre_scripts_execute_on_update(world, entity)
end
function spectre.scripts.execute_on_enter(world, entity)
    return ffi.C.spectre_scripts_execute_on_enter(world, entity)
end
function spectre.scripts.execute_on_exit(world, entity)
    return ffi.C.spectre_scripts_execute_on_exit(world, entity)
end
function spectre.scripts.has_script(world, function_name, arg_types, arg_count)
    return ffi.C.spectre_scripts_has_script(world, function_name, arg_types, arg_count)
end
function spectre.scripts.is_script(world, entity)
    return ffi.C.spectre_scripts_is_script(world, entity)
end
function spectre.scripts.find_script(world, function_name)
    return ffi.C.spectre_scripts_find_script(world, function_name)
end
function spectre.scripts.include_code(world, path)
    return ffi.C.spectre_scripts_include_code(world, path)
end
function spectre.scripts.execute_script(world, function_name, args, arg_count)
    return ffi.C.spectre_scripts_execute_script(world, function_name, args, arg_count)
end
function spectre.scripts.serialize_scripts(world, entity)
    return ffi.C.spectre_scripts_serialize_scripts(world, entity)
end
function spectre.scripts.deserialize_scripts(world, target, props)
    return ffi.C.spectre_scripts_deserialize_scripts(world, target, props)
end
function spectre.scripts.execute_on_create(world, entity)
    return ffi.C.spectre_scripts_execute_on_create(world, entity)
end
function spectre.scripts.execute_on_destroy(world, entity)
    return ffi.C.spectre_scripts_execute_on_destroy(world, entity)
end
function spectre.scripts.execute_on_update(world, entity)
    return ffi.C.spectre_scripts_execute_on_update(world, entity)
end
function spectre.scripts.execute_on_enter(world, entity)
    return ffi.C.spectre_scripts_execute_on_enter(world, entity)
end
function spectre.scripts.execute_on_exit(world, entity)
    return ffi.C.spectre_scripts_execute_on_exit(world, entity)
end

spectre.prefabs = {}
function spectre.prefabs.serialize_entity(world, entity)
    return ffi.C.spectre_prefabs_serialize_entity(world, entity)
end
function spectre.prefabs.deserialize_entity(world, target, props)
    return ffi.C.spectre_prefabs_deserialize_entity(world, target, props)
end
function spectre.prefabs.register_prefab(world, name, props)
    return ffi.C.spectre_prefabs_register_prefab(world, name, props)
end
function spectre.prefabs.has_prefab(world, name)
    return ffi.C.spectre_prefabs_has_prefab(world, name)
end
function spectre.prefabs.is_prefab(world, entity)
    return ffi.C.spectre_prefabs_is_prefab(world, entity)
end
function spectre.prefabs.find_prefab(world, name)
    return ffi.C.spectre_prefabs_find_prefab(world, name)
end
function spectre.prefabs.create_entity_from_props(world, props)
    return ffi.C.spectre_prefabs_create_entity_from_props(world, props)
end
function spectre.prefabs.create_entity_from_prefab(world, prefab)
    return ffi.C.spectre_prefabs_create_entity_from_prefab(world, prefab)
end
function spectre.prefabs.create_entity_from_name(world, name)
    return ffi.C.spectre_prefabs_create_entity_from_name(world, name)
end
function spectre.prefabs.serialize_entity(world, entity)
    return ffi.C.spectre_prefabs_serialize_entity(world, entity)
end
function spectre.prefabs.deserialize_entity(world, target, props)
    return ffi.C.spectre_prefabs_deserialize_entity(world, target, props)
end
function spectre.prefabs.register_prefab(world, name, props)
    return ffi.C.spectre_prefabs_register_prefab(world, name, props)
end
function spectre.prefabs.has_prefab(world, name)
    return ffi.C.spectre_prefabs_has_prefab(world, name)
end
function spectre.prefabs.is_prefab(world, entity)
    return ffi.C.spectre_prefabs_is_prefab(world, entity)
end
function spectre.prefabs.find_prefab(world, name)
    return ffi.C.spectre_prefabs_find_prefab(world, name)
end
function spectre.prefabs.create_entity_from_props(world, props)
    return ffi.C.spectre_prefabs_create_entity_from_props(world, props)
end
function spectre.prefabs.create_entity_from_prefab(world, prefab)
    return ffi.C.spectre_prefabs_create_entity_from_prefab(world, prefab)
end
function spectre.prefabs.create_entity_from_name(world, name)
    return ffi.C.spectre_prefabs_create_entity_from_name(world, name)
end

return spectre
