#pragma once

#include <spectre/abi/window_service.h>
#include <flecs.h>
#include <cstdint>

namespace spectre::module {

    /**
     * @brief High-level C++ SDK for interacting with the window module.
     */
    class window {
    public:
        // --- Core Application Loop ---
        static bool should_close(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->should_close) return service->api->should_close(service->api->instance);
            }
            return false;
        }
        static void set_should_close(const flecs::world& entity_world, bool close) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_should_close) service->api->set_should_close(service->api->instance, close);
            }
        }

        // --- Dimensions & Position ---
        static void set_size(const flecs::world& entity_world, uint32_t width, uint32_t height) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_size) service->api->set_size(service->api->instance, width, height);
            }
        }
        static void set_position(const flecs::world& entity_world, int32_t x, int32_t y) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_position) service->api->set_position(service->api->instance, x, y);
            }
        }
        static void set_size_limits(const flecs::world& entity_world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_size_limits) service->api->set_size_limits(service->api->instance, min_width, min_height, max_width, max_height);
            }
        }

        static uint32_t get_width(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_width) return service->api->get_width(service->api->instance);
            }
            return 0;
        }
        static uint32_t get_height(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_height) return service->api->get_height(service->api->instance);
            }
            return 0;
        }
        static float get_aspect_ratio(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_aspect_ratio) return service->api->get_aspect_ratio(service->api->instance);
            }
            return 0.0f;
        }
        static int32_t get_position_x(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_position_x) return service->api->get_position_x(service->api->instance);
            }
            return 0;
        }
        static int32_t get_position_y(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_position_y) return service->api->get_position_y(service->api->instance);
            }
            return 0;
        }

        // --- Window Attributes ---
        static void set_title(const flecs::world& entity_world, const char* title) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_title) service->api->set_title(service->api->instance, title);
            }
        }
        static void set_vsync(const flecs::world& entity_world, bool enabled) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_vsync) service->api->set_vsync(service->api->instance, enabled);
            }
        }
        static void set_fullscreen(const flecs::world& entity_world, bool fullscreen) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_fullscreen) service->api->set_fullscreen(service->api->instance, fullscreen);
            }
        }
        static void set_borderless(const flecs::world& entity_world, bool borderless) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_borderless) service->api->set_borderless(service->api->instance, borderless);
            }
        }
        static void set_resizable(const flecs::world& entity_world, bool resizable) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_resizable) service->api->set_resizable(service->api->instance, resizable);
            }
        }
        static void set_always_on_top(const flecs::world& entity_world, bool always_on_top) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_always_on_top) service->api->set_always_on_top(service->api->instance, always_on_top);
            }
        }

        static const char* get_title(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_title) return service->api->get_title(service->api->instance);
            }
            return "";
        }
        static bool is_vsync(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_vsync) return service->api->is_vsync(service->api->instance);
            }
            return false;
        }
        static bool is_fullscreen(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_fullscreen) return service->api->is_fullscreen(service->api->instance);
            }
            return false;
        }
        static bool is_borderless(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_borderless) return service->api->is_borderless(service->api->instance);
            }
            return false;
        }
        static bool is_resizable(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_resizable) return service->api->is_resizable(service->api->instance);
            }
            return false;
        }

        // --- Window Actions ---
        static void minimize(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->minimize) service->api->minimize(service->api->instance);
            }
        }
        static void maximize(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->maximize) service->api->maximize(service->api->instance);
            }
        }
        static void restore(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->restore) service->api->restore(service->api->instance);
            }
        }
        static void show(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->show) service->api->show(service->api->instance);
            }
        }
        static void hide(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->hide) service->api->hide(service->api->instance);
            }
        }
        static void request_attention(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->request_attention) service->api->request_attention(service->api->instance);
            }
        }

        // --- Window State Inquiries ---
        static bool is_minimized(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_minimized) return service->api->is_minimized(service->api->instance);
            }
            return false;
        }
        static bool is_maximized(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_maximized) return service->api->is_maximized(service->api->instance);
            }
            return false;
        }
        static bool is_visible(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_visible) return service->api->is_visible(service->api->instance);
            }
            return false;
        }
        static bool is_focused(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_focused) return service->api->is_focused(service->api->instance);
            }
            return false;
        }
        static bool is_hovered(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_hovered) return service->api->is_hovered(service->api->instance);
            }
            return false;
        }

        // --- Input & Cursor Management ---
        static void set_cursor_visible(const flecs::world& entity_world, bool visible) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_cursor_visible) service->api->set_cursor_visible(service->api->instance, visible);
            }
        }
        static void set_cursor_locked(const flecs::world& entity_world, bool locked) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->set_cursor_locked) service->api->set_cursor_locked(service->api->instance, locked);
            }
        }
        static bool is_cursor_visible(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_cursor_visible) return service->api->is_cursor_visible(service->api->instance);
            }
            return false;
        }
        static bool is_cursor_locked(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->is_cursor_locked) return service->api->is_cursor_locked(service->api->instance);
            }
            return false;
        }

        // --- Native/Platform Interop ---
        static void* get_native_handle(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_window_service_t)) {
                if (service->api && service->api->get_native_handle) return service->api->get_native_handle(service->api->instance);
            }
            return nullptr;
        }
    };

}
