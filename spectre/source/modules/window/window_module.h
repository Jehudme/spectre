#pragma once

#include <flecs.h>
#include <cstdint>

namespace spectre::module {
    class WindowModule {
    public:
        WindowModule(flecs::world& world);
        ~WindowModule();

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

    private:
        flecs::world m_entity_world;
        flecs::system m_window_system;
        const char* m_title = "Spectre Engine";
    };
}
