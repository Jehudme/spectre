#pragma once
#include <spectre/services/window_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class window {
    public:
        static ecs_entity_t deserialize_window(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_window_deserialize_window(entity_world.c_ptr(), props);
        }

        static sandbox_properties_handle_t serialize_window(const flecs::world& entity_world, ecs_entity_t window) {
            return spectre_window_serialize_window(entity_world.c_ptr(), window);
        }

        static void register_window(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_window_register_window(entity_world.c_ptr(), props);}

        static bool should_close(const flecs::world& entity_world) {
            return spectre_window_should_close(entity_world.c_ptr());
        }

        static void set_should_close(const flecs::world& entity_world, bool close) {
            spectre_window_set_should_close(entity_world.c_ptr(), close);}

        static void set_size(const flecs::world& entity_world, uint32_t width, uint32_t height) {
            spectre_window_set_size(entity_world.c_ptr(), width, height);}

        static void set_position(const flecs::world& entity_world, int32_t x, int32_t y) {
            spectre_window_set_position(entity_world.c_ptr(), x, y);}

        static void set_size_limits(const flecs::world& entity_world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
            spectre_window_set_size_limits(entity_world.c_ptr(), min_width, min_height, max_width, max_height);}

        static uint32_t get_width(const flecs::world& entity_world) {
            return spectre_window_get_width(entity_world.c_ptr());
        }

        static uint32_t get_height(const flecs::world& entity_world) {
            return spectre_window_get_height(entity_world.c_ptr());
        }

        static float get_aspect_ratio(const flecs::world& entity_world) {
            return spectre_window_get_aspect_ratio(entity_world.c_ptr());
        }

        static int32_t get_position_x(const flecs::world& entity_world) {
            return spectre_window_get_position_x(entity_world.c_ptr());
        }

        static int32_t get_position_y(const flecs::world& entity_world) {
            return spectre_window_get_position_y(entity_world.c_ptr());
        }

        static void set_title(const flecs::world& entity_world, const char* title) {
            spectre_window_set_title(entity_world.c_ptr(), title);}

        static void set_vsync(const flecs::world& entity_world, bool enabled) {
            spectre_window_set_vsync(entity_world.c_ptr(), enabled);}

        static void set_fullscreen(const flecs::world& entity_world, bool fullscreen) {
            spectre_window_set_fullscreen(entity_world.c_ptr(), fullscreen);}

        static void set_borderless(const flecs::world& entity_world, bool borderless) {
            spectre_window_set_borderless(entity_world.c_ptr(), borderless);}

        static void set_resizable(const flecs::world& entity_world, bool resizable) {
            spectre_window_set_resizable(entity_world.c_ptr(), resizable);}

        static void set_always_on_top(const flecs::world& entity_world, bool always_on_top) {
            spectre_window_set_always_on_top(entity_world.c_ptr(), always_on_top);}

        static const char* get_title(const flecs::world& entity_world) {
            return spectre_window_get_title(entity_world.c_ptr());
        }

        static bool is_vsync(const flecs::world& entity_world) {
            return spectre_window_is_vsync(entity_world.c_ptr());
        }

        static bool is_fullscreen(const flecs::world& entity_world) {
            return spectre_window_is_fullscreen(entity_world.c_ptr());
        }

        static bool is_borderless(const flecs::world& entity_world) {
            return spectre_window_is_borderless(entity_world.c_ptr());
        }

        static bool is_resizable(const flecs::world& entity_world) {
            return spectre_window_is_resizable(entity_world.c_ptr());
        }

        static void minimize(const flecs::world& entity_world) {
            spectre_window_minimize(entity_world.c_ptr());}

        static void maximize(const flecs::world& entity_world) {
            spectre_window_maximize(entity_world.c_ptr());}

        static void restore(const flecs::world& entity_world) {
            spectre_window_restore(entity_world.c_ptr());}

        static void show(const flecs::world& entity_world) {
            spectre_window_show(entity_world.c_ptr());}

        static void hide(const flecs::world& entity_world) {
            spectre_window_hide(entity_world.c_ptr());}

        static void request_attention(const flecs::world& entity_world) {
            spectre_window_request_attention(entity_world.c_ptr());}

        static bool is_minimized(const flecs::world& entity_world) {
            return spectre_window_is_minimized(entity_world.c_ptr());
        }

        static bool is_maximized(const flecs::world& entity_world) {
            return spectre_window_is_maximized(entity_world.c_ptr());
        }

        static bool is_visible(const flecs::world& entity_world) {
            return spectre_window_is_visible(entity_world.c_ptr());
        }

        static bool is_focused(const flecs::world& entity_world) {
            return spectre_window_is_focused(entity_world.c_ptr());
        }

        static bool is_hovered(const flecs::world& entity_world) {
            return spectre_window_is_hovered(entity_world.c_ptr());
        }

        static void set_cursor_visible(const flecs::world& entity_world, bool visible) {
            spectre_window_set_cursor_visible(entity_world.c_ptr(), visible);}

        static void set_cursor_locked(const flecs::world& entity_world, bool locked) {
            spectre_window_set_cursor_locked(entity_world.c_ptr(), locked);}

        static bool is_cursor_visible(const flecs::world& entity_world) {
            return spectre_window_is_cursor_visible(entity_world.c_ptr());
        }

        static bool is_cursor_locked(const flecs::world& entity_world) {
            return spectre_window_is_cursor_locked(entity_world.c_ptr());
        }

        static void* get_native_handle(const flecs::world& entity_world) {
            return spectre_window_get_native_handle(entity_world.c_ptr());
        }

        static bool is_key_down(const flecs::world& entity_world, int keycode) {
            return spectre_window_is_key_down(entity_world.c_ptr(), keycode);
        }

        static bool is_key_pressed(const flecs::world& entity_world, int keycode) {
            return spectre_window_is_key_pressed(entity_world.c_ptr(), keycode);
        }

        static bool is_key_released(const flecs::world& entity_world, int keycode) {
            return spectre_window_is_key_released(entity_world.c_ptr(), keycode);
        }

        static float get_mouse_x(const flecs::world& entity_world) {
            return spectre_window_get_mouse_x(entity_world.c_ptr());
        }

        static float get_mouse_y(const flecs::world& entity_world) {
            return spectre_window_get_mouse_y(entity_world.c_ptr());
        }

        static float get_mouse_delta_x(const flecs::world& entity_world) {
            return spectre_window_get_mouse_delta_x(entity_world.c_ptr());
        }

        static float get_mouse_delta_y(const flecs::world& entity_world) {
            return spectre_window_get_mouse_delta_y(entity_world.c_ptr());
        }
    };
}
#endif
