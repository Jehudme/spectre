#include "spectre/services/window_service.h"
#include "window_module.h"

static ecs_entity_t   window_deserialize_window(ecs_world_t* world, sandbox_properties_handle_t props);
static sandbox_properties_handle_t window_serialize_window(ecs_world_t* world, ecs_entity_t window);
static void           window_register_window(ecs_world_t* world, sandbox_properties_handle_t props);
static bool           window_should_close(ecs_world_t* world);
static void           window_set_should_close(ecs_world_t* world, bool close);
static void           window_set_size(ecs_world_t* world, uint32_t width, uint32_t height);
static void           window_set_position(ecs_world_t* world, int32_t x, int32_t y);
static void           window_set_size_limits(ecs_world_t* world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);
static uint32_t       window_get_width(ecs_world_t* world);
static uint32_t       window_get_height(ecs_world_t* world);
static float          window_get_aspect_ratio(ecs_world_t* world);
static int32_t        window_get_position_x(ecs_world_t* world);
static int32_t        window_get_position_y(ecs_world_t* world);
static void           window_set_title(ecs_world_t* world, const char* title);
static void           window_set_vsync(ecs_world_t* world, bool enabled);
static void           window_set_fullscreen(ecs_world_t* world, bool fullscreen);
static void           window_set_borderless(ecs_world_t* world, bool borderless);
static void           window_set_resizable(ecs_world_t* world, bool resizable);
static void           window_set_always_on_top(ecs_world_t* world, bool always_on_top);
static const char*    window_get_title(ecs_world_t* world);
static bool           window_is_vsync(ecs_world_t* world);
static bool           window_is_fullscreen(ecs_world_t* world);
static bool           window_is_borderless(ecs_world_t* world);
static bool           window_is_resizable(ecs_world_t* world);
static void           window_minimize(ecs_world_t* world);
static void           window_maximize(ecs_world_t* world);
static void           window_restore(ecs_world_t* world);
static void           window_show(ecs_world_t* world);
static void           window_hide(ecs_world_t* world);
static void           window_request_attention(ecs_world_t* world);
static bool           window_is_minimized(ecs_world_t* world);
static bool           window_is_maximized(ecs_world_t* world);
static bool           window_is_visible(ecs_world_t* world);
static bool           window_is_focused(ecs_world_t* world);
static bool           window_is_hovered(ecs_world_t* world);
static void           window_set_cursor_visible(ecs_world_t* world, bool visible);
static void           window_set_cursor_locked(ecs_world_t* world, bool locked);
static bool           window_is_cursor_visible(ecs_world_t* world);
static bool           window_is_cursor_locked(ecs_world_t* world);
static void*          window_get_native_handle(ecs_world_t* world);
static bool           window_is_key_down(ecs_world_t* world, int keycode);
static bool           window_is_key_pressed(ecs_world_t* world, int keycode);
static bool           window_is_key_released(ecs_world_t* world, int keycode);
static float          window_get_mouse_x(ecs_world_t* world);
static float          window_get_mouse_y(ecs_world_t* world);
static float          window_get_mouse_delta_x(ecs_world_t* world);
static float          window_get_mouse_delta_y(ecs_world_t* world);

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

static ecs_entity_t window_deserialize_window(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->deserialize_window(sandbox::properties(props)).id();
    return 0;
}

static sandbox_properties_handle_t window_serialize_window(ecs_world_t* world, ecs_entity_t window) {
    if (!world) return {0};
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->serialize_window(w.entity(window)).get_raw();
    return {0};
}

static void window_register_window(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->register_window(sandbox::properties(props));
}

static bool window_should_close(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->should_close();
    return false;
}

static void window_set_should_close(ecs_world_t* world, bool close) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_should_close(close);
}

static void window_set_size(ecs_world_t* world, uint32_t width, uint32_t height) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_size(width, height);
}

static void window_set_position(ecs_world_t* world, int32_t x, int32_t y) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_position(x, y);
}

static void window_set_size_limits(ecs_world_t* world, uint32_t min_w, uint32_t min_h, uint32_t max_w, uint32_t max_h) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_size_limits(min_w, min_h, max_w, max_h);
}

static uint32_t window_get_width(ecs_world_t* world) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_width();
    return 0;
}

static uint32_t window_get_height(ecs_world_t* world) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_height();
    return 0;
}

static float window_get_aspect_ratio(ecs_world_t* world) {
    if (!world) return 0.0f;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_aspect_ratio();
    return 0.0f;
}

static int32_t window_get_position_x(ecs_world_t* world) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_position_x();
    return 0;
}

static int32_t window_get_position_y(ecs_world_t* world) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_position_y();
    return 0;
}

static void window_set_title(ecs_world_t* world, const char* title) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_title(title);
}

static void window_set_vsync(ecs_world_t* world, bool enabled) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_vsync(enabled);
}

static void window_set_fullscreen(ecs_world_t* world, bool fullscreen) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_fullscreen(fullscreen);
}

static void window_set_borderless(ecs_world_t* world, bool borderless) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_borderless(borderless);
}

static void window_set_resizable(ecs_world_t* world, bool resizable) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_resizable(resizable);
}

static void window_set_always_on_top(ecs_world_t* world, bool always_on_top) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_always_on_top(always_on_top);
}

static const char* window_get_title(ecs_world_t* world) {
    if (!world) return nullptr;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_title();
    return nullptr;
}

static bool window_is_vsync(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_vsync();
    return false;
}

static bool window_is_fullscreen(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_fullscreen();
    return false;
}

static bool window_is_borderless(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_borderless();
    return false;
}

static bool window_is_resizable(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_resizable();
    return false;
}

static void window_minimize(ecs_world_t* world) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->minimize();
}

static void window_maximize(ecs_world_t* world) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->maximize();
}

static void window_restore(ecs_world_t* world) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->restore();
}

static void window_show(ecs_world_t* world) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->show();
}

static void window_hide(ecs_world_t* world) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->hide();
}

static void window_request_attention(ecs_world_t* world) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->request_attention();
}

static bool window_is_minimized(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_minimized();
    return false;
}

static bool window_is_maximized(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_maximized();
    return false;
}

static bool window_is_visible(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_visible();
    return false;
}

static bool window_is_focused(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_focused();
    return false;
}

static bool window_is_hovered(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_hovered();
    return false;
}

static void window_set_cursor_visible(ecs_world_t* world, bool visible) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_cursor_visible(visible);
}

static void window_set_cursor_locked(ecs_world_t* world, bool locked) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) m->set_cursor_locked(locked);
}

static bool window_is_cursor_visible(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_cursor_visible();
    return false;
}

static bool window_is_cursor_locked(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_cursor_locked();
    return false;
}

static void* window_get_native_handle(ecs_world_t* world) {
    if (!world) return nullptr;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_native_handle();
    return nullptr;
}

static bool window_is_key_down(ecs_world_t* world, int keycode) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_key_down(keycode);
    return false;
}

static bool window_is_key_pressed(ecs_world_t* world, int keycode) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_key_pressed(keycode);
    return false;
}

static bool window_is_key_released(ecs_world_t* world, int keycode) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->is_key_released(keycode);
    return false;
}

static float window_get_mouse_x(ecs_world_t* world) {
    if (!world) return 0.0f;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_mouse_x();
    return 0.0f;
}

static float window_get_mouse_y(ecs_world_t* world) {
    if (!world) return 0.0f;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_mouse_y();
    return 0.0f;
}

static float window_get_mouse_delta_x(ecs_world_t* world) {
    if (!world) return 0.0f;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_mouse_delta_x();
    return 0.0f;
}

static float window_get_mouse_delta_y(ecs_world_t* world) {
    if (!world) return 0.0f;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::window_module_t>();
    if (m) return m->get_mouse_delta_y();
    return 0.0f;
}
