#pragma once
#include <spectre/services/window_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class window {
    public:
        static ecs_entity_t deserialize_window(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->deserialize_window) {
                    return service->api->deserialize_window(entity_world.c_ptr(), props);
                }
            } return 0;
        }

        static sandbox_properties_handle_t serialize_window(const flecs::world& entity_world, ecs_entity_t window) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->serialize_window) {
                    return service->api->serialize_window(entity_world.c_ptr(), window);
                }
            } return {0};
        }

        static void register_window(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->register_window) {
                    service->api->register_window(entity_world.c_ptr(), props);
                }
            }
        }

        static bool should_close(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->should_close) {
                    return service->api->should_close(entity_world.c_ptr());
                }
            } return false;
        }

        static void set_should_close(const flecs::world& entity_world, bool close) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_should_close) {
                    service->api->set_should_close(entity_world.c_ptr(), close);
                }
            }
        }

        static void set_size(const flecs::world& entity_world, uint32_t width, uint32_t height) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_size) {
                    service->api->set_size(entity_world.c_ptr(), width, height);
                }
            }
        }

        static void set_position(const flecs::world& entity_world, int32_t x, int32_t y) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_position) {
                    service->api->set_position(entity_world.c_ptr(), x, y);
                }
            }
        }

        static void set_size_limits(const flecs::world& entity_world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_size_limits) {
                    service->api->set_size_limits(entity_world.c_ptr(), min_width, min_height, max_width, max_height);
                }
            }
        }

        static uint32_t get_width(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_width) {
                    return service->api->get_width(entity_world.c_ptr());
                }
            } return 0;
        }

        static uint32_t get_height(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_height) {
                    return service->api->get_height(entity_world.c_ptr());
                }
            } return 0;
        }

        static float get_aspect_ratio(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_aspect_ratio) {
                    return service->api->get_aspect_ratio(entity_world.c_ptr());
                }
            } return 0;
        }

        static int32_t get_position_x(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_position_x) {
                    return service->api->get_position_x(entity_world.c_ptr());
                }
            } return 0;
        }

        static int32_t get_position_y(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_position_y) {
                    return service->api->get_position_y(entity_world.c_ptr());
                }
            } return 0;
        }

        static void set_title(const flecs::world& entity_world, const char* title) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_title) {
                    service->api->set_title(entity_world.c_ptr(), title);
                }
            }
        }

        static void set_vsync(const flecs::world& entity_world, bool enabled) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_vsync) {
                    service->api->set_vsync(entity_world.c_ptr(), enabled);
                }
            }
        }

        static void set_fullscreen(const flecs::world& entity_world, bool fullscreen) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_fullscreen) {
                    service->api->set_fullscreen(entity_world.c_ptr(), fullscreen);
                }
            }
        }

        static void set_borderless(const flecs::world& entity_world, bool borderless) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_borderless) {
                    service->api->set_borderless(entity_world.c_ptr(), borderless);
                }
            }
        }

        static void set_resizable(const flecs::world& entity_world, bool resizable) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_resizable) {
                    service->api->set_resizable(entity_world.c_ptr(), resizable);
                }
            }
        }

        static void set_always_on_top(const flecs::world& entity_world, bool always_on_top) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_always_on_top) {
                    service->api->set_always_on_top(entity_world.c_ptr(), always_on_top);
                }
            }
        }

        static const char* get_title(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_title) {
                    return service->api->get_title(entity_world.c_ptr());
                }
            } return nullptr;
        }

        static bool is_vsync(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_vsync) {
                    return service->api->is_vsync(entity_world.c_ptr());
                }
            } return false;
        }

        static bool is_fullscreen(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_fullscreen) {
                    return service->api->is_fullscreen(entity_world.c_ptr());
                }
            } return false;
        }

        static bool is_borderless(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_borderless) {
                    return service->api->is_borderless(entity_world.c_ptr());
                }
            } return false;
        }

        static bool is_resizable(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_resizable) {
                    return service->api->is_resizable(entity_world.c_ptr());
                }
            } return false;
        }

        static void minimize(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->minimize) {
                    service->api->minimize(entity_world.c_ptr());
                }
            }
        }

        static void maximize(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->maximize) {
                    service->api->maximize(entity_world.c_ptr());
                }
            }
        }

        static void restore(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->restore) {
                    service->api->restore(entity_world.c_ptr());
                }
            }
        }

        static void show(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->show) {
                    service->api->show(entity_world.c_ptr());
                }
            }
        }

        static void hide(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->hide) {
                    service->api->hide(entity_world.c_ptr());
                }
            }
        }

        static void request_attention(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->request_attention) {
                    service->api->request_attention(entity_world.c_ptr());
                }
            }
        }

        static bool is_minimized(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_minimized) {
                    return service->api->is_minimized(entity_world.c_ptr());
                }
            } return false;
        }

        static bool is_maximized(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_maximized) {
                    return service->api->is_maximized(entity_world.c_ptr());
                }
            } return false;
        }

        static bool is_visible(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_visible) {
                    return service->api->is_visible(entity_world.c_ptr());
                }
            } return false;
        }

        static bool is_focused(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_focused) {
                    return service->api->is_focused(entity_world.c_ptr());
                }
            } return false;
        }

        static bool is_hovered(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_hovered) {
                    return service->api->is_hovered(entity_world.c_ptr());
                }
            } return false;
        }

        static void set_cursor_visible(const flecs::world& entity_world, bool visible) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_cursor_visible) {
                    service->api->set_cursor_visible(entity_world.c_ptr(), visible);
                }
            }
        }

        static void set_cursor_locked(const flecs::world& entity_world, bool locked) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_cursor_locked) {
                    service->api->set_cursor_locked(entity_world.c_ptr(), locked);
                }
            }
        }

        static bool is_cursor_visible(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_cursor_visible) {
                    return service->api->is_cursor_visible(entity_world.c_ptr());
                }
            } return false;
        }

        static bool is_cursor_locked(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_cursor_locked) {
                    return service->api->is_cursor_locked(entity_world.c_ptr());
                }
            } return false;
        }

        static void* get_native_handle(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_native_handle) {
                    return service->api->get_native_handle(entity_world.c_ptr());
                }
            } return nullptr;
        }

        static bool is_key_down(const flecs::world& entity_world, int keycode) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_key_down) {
                    return service->api->is_key_down(entity_world.c_ptr(), keycode);
                }
            } return false;
        }

        static bool is_key_pressed(const flecs::world& entity_world, int keycode) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_key_pressed) {
                    return service->api->is_key_pressed(entity_world.c_ptr(), keycode);
                }
            } return false;
        }

        static bool is_key_released(const flecs::world& entity_world, int keycode) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_key_released) {
                    return service->api->is_key_released(entity_world.c_ptr(), keycode);
                }
            } return false;
        }

        static float get_mouse_x(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_mouse_x) {
                    return service->api->get_mouse_x(entity_world.c_ptr());
                }
            } return 0;
        }

        static float get_mouse_y(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_mouse_y) {
                    return service->api->get_mouse_y(entity_world.c_ptr());
                }
            } return 0;
        }

        static float get_mouse_delta_x(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_mouse_delta_x) {
                    return service->api->get_mouse_delta_x(entity_world.c_ptr());
                }
            } return 0;
        }

        static float get_mouse_delta_y(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_mouse_delta_y) {
                    return service->api->get_mouse_delta_y(entity_world.c_ptr());
                }
            } return 0;
        }
    };
}
#endif
