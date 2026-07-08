#pragma once
#include <cstdint>

#include <flecs.h>
#include "sandbox/sdk/properties.hpp"

namespace spectre::modules {
    class window_module_t {
    public:
        window_module_t(flecs::world& world);
        ~window_module_t() = default;

        window_module_t(const window_module_t&) = delete;
        window_module_t& operator=(const window_module_t&) = delete;
        window_module_t(window_module_t&&) = delete;
        window_module_t& operator=(window_module_t&&) = delete;

        flecs::entity deserialize_window(sandbox::properties props);
        sandbox::properties serialize_window(flecs::entity window);

        void register_window(sandbox::properties props);

        bool should_close() const;
        void set_should_close(bool close);

        void set_size(uint32_t width, uint32_t height);
        void set_position(int32_t x, int32_t y);
        void set_size_limits(uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);

        uint32_t get_width() const;
        uint32_t get_height() const;
        float get_aspect_ratio() const;
        int32_t get_position_x() const;
        int32_t get_position_y() const;

        void set_title(const char* title);
        void set_vsync(bool enabled);
        void set_fullscreen(bool fullscreen);
        void set_borderless(bool borderless);
        void set_resizable(bool resizable);
        void set_always_on_top(bool always_on_top);

        const char* get_title() const;
        bool is_vsync() const;
        bool is_fullscreen() const;
        bool is_borderless() const;
        bool is_resizable() const;

        void minimize();
        void maximize();
        void restore();
        void show();
        void hide();
        void request_attention();

        bool is_minimized() const;
        bool is_maximized() const;
        bool is_visible() const;
        bool is_focused() const;
        bool is_hovered() const;

        void set_cursor_visible(bool visible);
        void set_cursor_locked(bool locked);
        bool is_cursor_visible() const;
        bool is_cursor_locked() const;

        void* get_native_handle() const;

        // --- Input Queries (For gameplay systems) ---
        bool is_key_down(int keycode) const;
        bool is_key_pressed(int keycode) const;
        bool is_key_released(int keycode) const;

        float get_mouse_x() const;
        float get_mouse_y() const;
        float get_mouse_delta_x() const;
        float get_mouse_delta_y() const;

    private:
        void begin_input_frame();
        void set_key_down(int keycode);
        void set_key_up(int keycode);
        void set_mouse_position(float x, float y);
        void set_mouse_delta(float dx, float dy);

    private:
        flecs::entity m_window_module;
        flecs::world m_world;
    };
}
