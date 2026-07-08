#include <spectre/sdk/window.hpp>
#include "spectre/services/window_service.h"
#include "window_module.h"

static ecs_entity_t   window_deserialize_window(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static sandbox_properties_handle_t window_serialize_window(ecs_world_t* entity_world, ecs_entity_t window);
static void           window_register_window(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static bool           window_should_close(ecs_world_t* entity_world);
static void           window_set_should_close(ecs_world_t* entity_world, bool close);
static void           window_set_size(ecs_world_t* entity_world, uint32_t width, uint32_t height);
static void           window_set_position(ecs_world_t* entity_world, int32_t x, int32_t y);
static void           window_set_size_limits(ecs_world_t* entity_world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);
static uint32_t       window_get_width(ecs_world_t* entity_world);
static uint32_t       window_get_height(ecs_world_t* entity_world);
static float          window_get_aspect_ratio(ecs_world_t* entity_world);
static int32_t        window_get_position_x(ecs_world_t* entity_world);
static int32_t        window_get_position_y(ecs_world_t* entity_world);
static void           window_set_title(ecs_world_t* entity_world, const char* title);
static void           window_set_vsync(ecs_world_t* entity_world, bool enabled);
static void           window_set_fullscreen(ecs_world_t* entity_world, bool fullscreen);
static void           window_set_borderless(ecs_world_t* entity_world, bool borderless);
static void           window_set_resizable(ecs_world_t* entity_world, bool resizable);
static void           window_set_always_on_top(ecs_world_t* entity_world, bool always_on_top);
static const char*    window_get_title(ecs_world_t* entity_world);
static bool           window_is_vsync(ecs_world_t* entity_world);
static bool           window_is_fullscreen(ecs_world_t* entity_world);
static bool           window_is_borderless(ecs_world_t* entity_world);
static bool           window_is_resizable(ecs_world_t* entity_world);
static void           window_minimize(ecs_world_t* entity_world);
static void           window_maximize(ecs_world_t* entity_world);
static void           window_restore(ecs_world_t* entity_world);
static void           window_show(ecs_world_t* entity_world);
static void           window_hide(ecs_world_t* entity_world);
static void           window_request_attention(ecs_world_t* entity_world);
static bool           window_is_minimized(ecs_world_t* entity_world);
static bool           window_is_maximized(ecs_world_t* entity_world);
static bool           window_is_visible(ecs_world_t* entity_world);
static bool           window_is_focused(ecs_world_t* entity_world);
static bool           window_is_hovered(ecs_world_t* entity_world);
static void           window_set_cursor_visible(ecs_world_t* entity_world, bool visible);
static void           window_set_cursor_locked(ecs_world_t* entity_world, bool locked);
static bool           window_is_cursor_visible(ecs_world_t* entity_world);
static bool           window_is_cursor_locked(ecs_world_t* entity_world);
static void*          window_get_native_handle(ecs_world_t* entity_world);
static bool           window_is_key_down(ecs_world_t* entity_world, int keycode);
static bool           window_is_key_pressed(ecs_world_t* entity_world, int keycode);
static bool           window_is_key_released(ecs_world_t* entity_world, int keycode);
static float          window_get_mouse_x(ecs_world_t* entity_world);
static float          window_get_mouse_y(ecs_world_t* entity_world);
static float          window_get_mouse_delta_x(ecs_world_t* entity_world);
static float          window_get_mouse_delta_y(ecs_world_t* entity_world);

spectre_window_api_t g_window_api = {
    .deserialize_window  = window_deserialize_window,
    .serialize_window    = window_serialize_window,
    .register_window     = window_register_window,
    .should_close        = window_should_close,
    .set_should_close    = window_set_should_close,
    .set_size            = window_set_size,
    .set_position        = window_set_position,
    .set_size_limits     = window_set_size_limits,
    .get_width           = window_get_width,
    .get_height          = window_get_height,
    .get_aspect_ratio    = window_get_aspect_ratio,
    .get_position_x      = window_get_position_x,
    .get_position_y      = window_get_position_y,
    .set_title           = window_set_title,
    .set_vsync           = window_set_vsync,
    .set_fullscreen      = window_set_fullscreen,
    .set_borderless      = window_set_borderless,
    .set_resizable       = window_set_resizable,
    .set_always_on_top   = window_set_always_on_top,
    .get_title           = window_get_title,
    .is_vsync            = window_is_vsync,
    .is_fullscreen       = window_is_fullscreen,
    .is_borderless       = window_is_borderless,
    .is_resizable        = window_is_resizable,
    .minimize            = window_minimize,
    .maximize            = window_maximize,
    .restore             = window_restore,
    .show                = window_show,
    .hide                = window_hide,
    .request_attention   = window_request_attention,
    .is_minimized        = window_is_minimized,
    .is_maximized        = window_is_maximized,
    .is_visible          = window_is_visible,
    .is_focused          = window_is_focused,
    .is_hovered          = window_is_hovered,
    .set_cursor_visible  = window_set_cursor_visible,
    .set_cursor_locked   = window_set_cursor_locked,
    .is_cursor_visible   = window_is_cursor_visible,
    .is_cursor_locked    = window_is_cursor_locked,
    .get_native_handle   = window_get_native_handle,
    .is_key_down         = window_is_key_down,
    .is_key_pressed      = window_is_key_pressed,
    .is_key_released     = window_is_key_released,
    .get_mouse_x         = window_get_mouse_x,
    .get_mouse_y         = window_get_mouse_y,
    .get_mouse_delta_x   = window_get_mouse_delta_x,
    .get_mouse_delta_y   = window_get_mouse_delta_y,
};

SANDBOX_DEFINE_SERVICE(spectre_window_service_t, spectre_window_api_t, &g_window_api)

static ecs_entity_t window_deserialize_window(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->deserialize_window(sandbox::properties(props)).id();
    return 0;
}

static sandbox_properties_handle_t window_serialize_window(ecs_world_t* entity_world, ecs_entity_t window) {
    if (!entity_world) return {0};
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->serialize_window(flecs_world.entity(window)).get_raw();
    return {0};
}

static void window_register_window(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->register_window(sandbox::properties(props));
}

static bool window_should_close(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->should_close();
    return false;
}

static void window_set_should_close(ecs_world_t* entity_world, bool close) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_should_close(close);
}

static void window_set_size(ecs_world_t* entity_world, uint32_t width, uint32_t height) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_size(width, height);
}

static void window_set_position(ecs_world_t* entity_world, int32_t x, int32_t y) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_position(x, y);
}

static void window_set_size_limits(ecs_world_t* entity_world, uint32_t min_w, uint32_t min_h, uint32_t max_w, uint32_t max_h) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_size_limits(min_w, min_h, max_w, max_h);
}

static uint32_t window_get_width(ecs_world_t* entity_world) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_width();
    return 0;
}

static uint32_t window_get_height(ecs_world_t* entity_world) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_height();
    return 0;
}

static float window_get_aspect_ratio(ecs_world_t* entity_world) {
    if (!entity_world) return 0.0f;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_aspect_ratio();
    return 0.0f;
}

static int32_t window_get_position_x(ecs_world_t* entity_world) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_position_x();
    return 0;
}

static int32_t window_get_position_y(ecs_world_t* entity_world) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_position_y();
    return 0;
}

static void window_set_title(ecs_world_t* entity_world, const char* title) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_title(title);
}

static void window_set_vsync(ecs_world_t* entity_world, bool enabled) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_vsync(enabled);
}

static void window_set_fullscreen(ecs_world_t* entity_world, bool fullscreen) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_fullscreen(fullscreen);
}

static void window_set_borderless(ecs_world_t* entity_world, bool borderless) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_borderless(borderless);
}

static void window_set_resizable(ecs_world_t* entity_world, bool resizable) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_resizable(resizable);
}

static void window_set_always_on_top(ecs_world_t* entity_world, bool always_on_top) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_always_on_top(always_on_top);
}

static const char* window_get_title(ecs_world_t* entity_world) {
    if (!entity_world) return nullptr;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_title();
    return nullptr;
}

static bool window_is_vsync(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_vsync();
    return false;
}

static bool window_is_fullscreen(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_fullscreen();
    return false;
}

static bool window_is_borderless(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_borderless();
    return false;
}

static bool window_is_resizable(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_resizable();
    return false;
}

static void window_minimize(ecs_world_t* entity_world) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->minimize();
}

static void window_maximize(ecs_world_t* entity_world) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->maximize();
}

static void window_restore(ecs_world_t* entity_world) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->restore();
}

static void window_show(ecs_world_t* entity_world) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->show();
}

static void window_hide(ecs_world_t* entity_world) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->hide();
}

static void window_request_attention(ecs_world_t* entity_world) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->request_attention();
}

static bool window_is_minimized(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_minimized();
    return false;
}

static bool window_is_maximized(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_maximized();
    return false;
}

static bool window_is_visible(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_visible();
    return false;
}

static bool window_is_focused(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_focused();
    return false;
}

static bool window_is_hovered(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_hovered();
    return false;
}

static void window_set_cursor_visible(ecs_world_t* entity_world, bool visible) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_cursor_visible(visible);
}

static void window_set_cursor_locked(ecs_world_t* entity_world, bool locked) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) module->set_cursor_locked(locked);
}

static bool window_is_cursor_visible(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_cursor_visible();
    return false;
}

static bool window_is_cursor_locked(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_cursor_locked();
    return false;
}

static void* window_get_native_handle(ecs_world_t* entity_world) {
    if (!entity_world) return nullptr;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_native_handle();
    return nullptr;
}

static bool window_is_key_down(ecs_world_t* entity_world, int keycode) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_key_down(keycode);
    return false;
}

static bool window_is_key_pressed(ecs_world_t* entity_world, int keycode) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_key_pressed(keycode);
    return false;
}

static bool window_is_key_released(ecs_world_t* entity_world, int keycode) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->is_key_released(keycode);
    return false;
}

static float window_get_mouse_x(ecs_world_t* entity_world) {
    if (!entity_world) return 0.0f;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_mouse_x();
    return 0.0f;
}

static float window_get_mouse_y(ecs_world_t* entity_world) {
    if (!entity_world) return 0.0f;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_mouse_y();
    return 0.0f;
}

static float window_get_mouse_delta_x(ecs_world_t* entity_world) {
    if (!entity_world) return 0.0f;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_mouse_delta_x();
    return 0.0f;
}

static float window_get_mouse_delta_y(ecs_world_t* entity_world) {
    if (!entity_world) return 0.0f;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::window_module_t>();
    if (module) return module->get_mouse_delta_y();
    return 0.0f;
}

// --- Public C API Implementations ---
ecs_entity_t spectre_window_deserialize_window(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_window) {
        return service->api->deserialize_window(world, props);
        
    }
    return (ecs_entity_t){0};
}

sandbox_properties_handle_t spectre_window_serialize_window(ecs_world_t* world, ecs_entity_t window) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_window) {
        return service->api->serialize_window(world, window);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}

void spectre_window_register_window(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->register_window) {
        service->api->register_window(world, props);
        return;
    }
    
}

bool spectre_window_should_close(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->should_close) {
        return service->api->should_close(world);
        
    }
    return false;
}

void spectre_window_set_should_close(ecs_world_t* world, bool close) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_should_close) {
        service->api->set_should_close(world, close);
        return;
    }
    
}

void spectre_window_set_size(ecs_world_t* world, uint32_t width, uint32_t height) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_size) {
        service->api->set_size(world, width, height);
        return;
    }
    
}

void spectre_window_set_position(ecs_world_t* world, int32_t x, int32_t y) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_position) {
        service->api->set_position(world, x, y);
        return;
    }
    
}

void spectre_window_set_size_limits(ecs_world_t* world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_size_limits) {
        service->api->set_size_limits(world, min_width, min_height, max_width, max_height);
        return;
    }
    
}

uint32_t spectre_window_get_width(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_width) {
        return service->api->get_width(world);
        
    }
    return 0;
}

uint32_t spectre_window_get_height(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_height) {
        return service->api->get_height(world);
        
    }
    return 0;
}

float spectre_window_get_aspect_ratio(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_aspect_ratio) {
        return service->api->get_aspect_ratio(world);
        
    }
    return 0;
}

int32_t spectre_window_get_position_x(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_position_x) {
        return service->api->get_position_x(world);
        
    }
    return 0;
}

int32_t spectre_window_get_position_y(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_position_y) {
        return service->api->get_position_y(world);
        
    }
    return 0;
}

void spectre_window_set_title(ecs_world_t* world, const char* title) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_title) {
        service->api->set_title(world, title);
        return;
    }
    
}

void spectre_window_set_vsync(ecs_world_t* world, bool enabled) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_vsync) {
        service->api->set_vsync(world, enabled);
        return;
    }
    
}

void spectre_window_set_fullscreen(ecs_world_t* world, bool fullscreen) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_fullscreen) {
        service->api->set_fullscreen(world, fullscreen);
        return;
    }
    
}

void spectre_window_set_borderless(ecs_world_t* world, bool borderless) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_borderless) {
        service->api->set_borderless(world, borderless);
        return;
    }
    
}

void spectre_window_set_resizable(ecs_world_t* world, bool resizable) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_resizable) {
        service->api->set_resizable(world, resizable);
        return;
    }
    
}

void spectre_window_set_always_on_top(ecs_world_t* world, bool always_on_top) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_always_on_top) {
        service->api->set_always_on_top(world, always_on_top);
        return;
    }
    
}

const char* spectre_window_get_title(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_title) {
        return service->api->get_title(world);
        
    }
    return NULL;
}

bool spectre_window_is_vsync(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_vsync) {
        return service->api->is_vsync(world);
        
    }
    return false;
}

bool spectre_window_is_fullscreen(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_fullscreen) {
        return service->api->is_fullscreen(world);
        
    }
    return false;
}

bool spectre_window_is_borderless(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_borderless) {
        return service->api->is_borderless(world);
        
    }
    return false;
}

bool spectre_window_is_resizable(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_resizable) {
        return service->api->is_resizable(world);
        
    }
    return false;
}

void spectre_window_minimize(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->minimize) {
        service->api->minimize(world);
        return;
    }
    
}

void spectre_window_maximize(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->maximize) {
        service->api->maximize(world);
        return;
    }
    
}

void spectre_window_restore(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->restore) {
        service->api->restore(world);
        return;
    }
    
}

void spectre_window_show(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->show) {
        service->api->show(world);
        return;
    }
    
}

void spectre_window_hide(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->hide) {
        service->api->hide(world);
        return;
    }
    
}

void spectre_window_request_attention(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->request_attention) {
        service->api->request_attention(world);
        return;
    }
    
}

bool spectre_window_is_minimized(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_minimized) {
        return service->api->is_minimized(world);
        
    }
    return false;
}

bool spectre_window_is_maximized(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_maximized) {
        return service->api->is_maximized(world);
        
    }
    return false;
}

bool spectre_window_is_visible(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_visible) {
        return service->api->is_visible(world);
        
    }
    return false;
}

bool spectre_window_is_focused(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_focused) {
        return service->api->is_focused(world);
        
    }
    return false;
}

bool spectre_window_is_hovered(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_hovered) {
        return service->api->is_hovered(world);
        
    }
    return false;
}

void spectre_window_set_cursor_visible(ecs_world_t* world, bool visible) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_cursor_visible) {
        service->api->set_cursor_visible(world, visible);
        return;
    }
    
}

void spectre_window_set_cursor_locked(ecs_world_t* world, bool locked) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_cursor_locked) {
        service->api->set_cursor_locked(world, locked);
        return;
    }
    
}

bool spectre_window_is_cursor_visible(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_cursor_visible) {
        return service->api->is_cursor_visible(world);
        
    }
    return false;
}

bool spectre_window_is_cursor_locked(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_cursor_locked) {
        return service->api->is_cursor_locked(world);
        
    }
    return false;
}

void* spectre_window_get_native_handle(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_native_handle) {
        return service->api->get_native_handle(world);
        
    }
    return NULL;
}

bool spectre_window_is_key_down(ecs_world_t* world, int keycode) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_key_down) {
        return service->api->is_key_down(world, keycode);
        
    }
    return false;
}

bool spectre_window_is_key_pressed(ecs_world_t* world, int keycode) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_key_pressed) {
        return service->api->is_key_pressed(world, keycode);
        
    }
    return false;
}

bool spectre_window_is_key_released(ecs_world_t* world, int keycode) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_key_released) {
        return service->api->is_key_released(world, keycode);
        
    }
    return false;
}

float spectre_window_get_mouse_x(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_mouse_x) {
        return service->api->get_mouse_x(world);
        
    }
    return 0;
}

float spectre_window_get_mouse_y(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_mouse_y) {
        return service->api->get_mouse_y(world);
        
    }
    return 0;
}

float spectre_window_get_mouse_delta_x(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_mouse_delta_x) {
        return service->api->get_mouse_delta_x(world);
        
    }
    return 0;
}

float spectre_window_get_mouse_delta_y(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_mouse_delta_y) {
        return service->api->get_mouse_delta_y(world);
        
    }
    return 0;
}

// --- SDK Implementations ---
namespace spectre::modules {
ecs_entity_t window::deserialize_window(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_window_deserialize_window(entity_world.c_ptr(), props);
        }

sandbox_properties_handle_t window::serialize_window(const flecs::world& entity_world, ecs_entity_t window) {
            return spectre_window_serialize_window(entity_world.c_ptr(), window);
        }

void window::register_window(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_window_register_window(entity_world.c_ptr(), props);}

bool window::should_close(const flecs::world& entity_world) {
            return spectre_window_should_close(entity_world.c_ptr());
        }

void window::set_should_close(const flecs::world& entity_world, bool close) {
            spectre_window_set_should_close(entity_world.c_ptr(), close);}

void window::set_size(const flecs::world& entity_world, uint32_t width, uint32_t height) {
            spectre_window_set_size(entity_world.c_ptr(), width, height);}

void window::set_position(const flecs::world& entity_world, int32_t x, int32_t y) {
            spectre_window_set_position(entity_world.c_ptr(), x, y);}

void window::set_size_limits(const flecs::world& entity_world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
            spectre_window_set_size_limits(entity_world.c_ptr(), min_width, min_height, max_width, max_height);}

uint32_t window::get_width(const flecs::world& entity_world) {
            return spectre_window_get_width(entity_world.c_ptr());
        }

uint32_t window::get_height(const flecs::world& entity_world) {
            return spectre_window_get_height(entity_world.c_ptr());
        }

float window::get_aspect_ratio(const flecs::world& entity_world) {
            return spectre_window_get_aspect_ratio(entity_world.c_ptr());
        }

int32_t window::get_position_x(const flecs::world& entity_world) {
            return spectre_window_get_position_x(entity_world.c_ptr());
        }

int32_t window::get_position_y(const flecs::world& entity_world) {
            return spectre_window_get_position_y(entity_world.c_ptr());
        }

void window::set_title(const flecs::world& entity_world, const char* title) {
            spectre_window_set_title(entity_world.c_ptr(), title);}

void window::set_vsync(const flecs::world& entity_world, bool enabled) {
            spectre_window_set_vsync(entity_world.c_ptr(), enabled);}

void window::set_fullscreen(const flecs::world& entity_world, bool fullscreen) {
            spectre_window_set_fullscreen(entity_world.c_ptr(), fullscreen);}

void window::set_borderless(const flecs::world& entity_world, bool borderless) {
            spectre_window_set_borderless(entity_world.c_ptr(), borderless);}

void window::set_resizable(const flecs::world& entity_world, bool resizable) {
            spectre_window_set_resizable(entity_world.c_ptr(), resizable);}

void window::set_always_on_top(const flecs::world& entity_world, bool always_on_top) {
            spectre_window_set_always_on_top(entity_world.c_ptr(), always_on_top);}

const char* window::get_title(const flecs::world& entity_world) {
            return spectre_window_get_title(entity_world.c_ptr());
        }

bool window::is_vsync(const flecs::world& entity_world) {
            return spectre_window_is_vsync(entity_world.c_ptr());
        }

bool window::is_fullscreen(const flecs::world& entity_world) {
            return spectre_window_is_fullscreen(entity_world.c_ptr());
        }

bool window::is_borderless(const flecs::world& entity_world) {
            return spectre_window_is_borderless(entity_world.c_ptr());
        }

bool window::is_resizable(const flecs::world& entity_world) {
            return spectre_window_is_resizable(entity_world.c_ptr());
        }

void window::minimize(const flecs::world& entity_world) {
            spectre_window_minimize(entity_world.c_ptr());}

void window::maximize(const flecs::world& entity_world) {
            spectre_window_maximize(entity_world.c_ptr());}

void window::restore(const flecs::world& entity_world) {
            spectre_window_restore(entity_world.c_ptr());}

void window::show(const flecs::world& entity_world) {
            spectre_window_show(entity_world.c_ptr());}

void window::hide(const flecs::world& entity_world) {
            spectre_window_hide(entity_world.c_ptr());}

void window::request_attention(const flecs::world& entity_world) {
            spectre_window_request_attention(entity_world.c_ptr());}

bool window::is_minimized(const flecs::world& entity_world) {
            return spectre_window_is_minimized(entity_world.c_ptr());
        }

bool window::is_maximized(const flecs::world& entity_world) {
            return spectre_window_is_maximized(entity_world.c_ptr());
        }

bool window::is_visible(const flecs::world& entity_world) {
            return spectre_window_is_visible(entity_world.c_ptr());
        }

bool window::is_focused(const flecs::world& entity_world) {
            return spectre_window_is_focused(entity_world.c_ptr());
        }

bool window::is_hovered(const flecs::world& entity_world) {
            return spectre_window_is_hovered(entity_world.c_ptr());
        }

void window::set_cursor_visible(const flecs::world& entity_world, bool visible) {
            spectre_window_set_cursor_visible(entity_world.c_ptr(), visible);}

void window::set_cursor_locked(const flecs::world& entity_world, bool locked) {
            spectre_window_set_cursor_locked(entity_world.c_ptr(), locked);}

bool window::is_cursor_visible(const flecs::world& entity_world) {
            return spectre_window_is_cursor_visible(entity_world.c_ptr());
        }

bool window::is_cursor_locked(const flecs::world& entity_world) {
            return spectre_window_is_cursor_locked(entity_world.c_ptr());
        }

void* window::get_native_handle(const flecs::world& entity_world) {
            return spectre_window_get_native_handle(entity_world.c_ptr());
        }

bool window::is_key_down(const flecs::world& entity_world, int keycode) {
            return spectre_window_is_key_down(entity_world.c_ptr(), keycode);
        }

bool window::is_key_pressed(const flecs::world& entity_world, int keycode) {
            return spectre_window_is_key_pressed(entity_world.c_ptr(), keycode);
        }

bool window::is_key_released(const flecs::world& entity_world, int keycode) {
            return spectre_window_is_key_released(entity_world.c_ptr(), keycode);
        }

float window::get_mouse_x(const flecs::world& entity_world) {
            return spectre_window_get_mouse_x(entity_world.c_ptr());
        }

float window::get_mouse_y(const flecs::world& entity_world) {
            return spectre_window_get_mouse_y(entity_world.c_ptr());
        }

float window::get_mouse_delta_x(const flecs::world& entity_world) {
            return spectre_window_get_mouse_delta_x(entity_world.c_ptr());
        }

float window::get_mouse_delta_y(const flecs::world& entity_world) {
            return spectre_window_get_mouse_delta_y(entity_world.c_ptr());
        }
}
