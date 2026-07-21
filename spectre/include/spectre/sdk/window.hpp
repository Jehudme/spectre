#pragma once
#include <spectre/services/window_service.h>

#ifdef __cplusplus
namespace spectre::modules {
class window {
  public:
    static void deserialize_window(const flecs::world& entity_world, ecs_entity_t target,
                                   sandbox_properties_handle_t props);

    static sandbox_properties_handle_t serialize_window(const flecs::world& entity_world, ecs_entity_t window);

    static void register_window(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static bool should_close(const flecs::world& entity_world);

    static void set_should_close(const flecs::world& entity_world, bool close);

    static void set_size(const flecs::world& entity_world, uint32_t width, uint32_t height);

    static void set_position(const flecs::world& entity_world, int32_t x, int32_t y);

    static void set_size_limits(const flecs::world& entity_world, uint32_t min_width, uint32_t min_height,
                                uint32_t max_width, uint32_t max_height);

    static uint32_t get_width(const flecs::world& entity_world);

    static uint32_t get_height(const flecs::world& entity_world);

    static float get_aspect_ratio(const flecs::world& entity_world);

    static int32_t get_position_x(const flecs::world& entity_world);

    static int32_t get_position_y(const flecs::world& entity_world);

    static void set_title(const flecs::world& entity_world, const char* title);

    static void set_vsync(const flecs::world& entity_world, bool enabled);

    static void set_fullscreen(const flecs::world& entity_world, bool fullscreen);

    static void set_borderless(const flecs::world& entity_world, bool borderless);

    static void set_resizable(const flecs::world& entity_world, bool resizable);

    static void set_always_on_top(const flecs::world& entity_world, bool always_on_top);

    static const char* get_title(const flecs::world& entity_world);

    static bool is_vsync(const flecs::world& entity_world);

    static bool is_fullscreen(const flecs::world& entity_world);

    static bool is_borderless(const flecs::world& entity_world);

    static bool is_resizable(const flecs::world& entity_world);

    static void minimize(const flecs::world& entity_world);

    static void maximize(const flecs::world& entity_world);

    static void restore(const flecs::world& entity_world);

    static void show(const flecs::world& entity_world);

    static void hide(const flecs::world& entity_world);

    static void request_attention(const flecs::world& entity_world);

    static bool is_minimized(const flecs::world& entity_world);

    static bool is_maximized(const flecs::world& entity_world);

    static bool is_visible(const flecs::world& entity_world);

    static bool is_focused(const flecs::world& entity_world);

    static bool is_hovered(const flecs::world& entity_world);

    static void set_cursor_visible(const flecs::world& entity_world, bool visible);

    static void set_cursor_locked(const flecs::world& entity_world, bool locked);

    static bool is_cursor_visible(const flecs::world& entity_world);

    static bool is_cursor_locked(const flecs::world& entity_world);

    static void* get_native_handle(const flecs::world& entity_world);

    static bool is_key_down(const flecs::world& entity_world, int keycode);

    static bool is_key_pressed(const flecs::world& entity_world, int keycode);

    static bool is_key_released(const flecs::world& entity_world, int keycode);

    static float get_mouse_x(const flecs::world& entity_world);

    static float get_mouse_y(const flecs::world& entity_world);

    static float get_mouse_delta_x(const flecs::world& entity_world);

    static float get_mouse_delta_y(const flecs::world& entity_world);
};
} // namespace spectre::modules
#endif
