#pragma once

#include <spectre/abi/window_service.h>

namespace sandbox::sdk {

/**
 * @brief Lightweight C++ wrapper around the unified Window/Input C-ABI.
 */
class WindowInputService {
public:
    inline WindowInputService(const spectre_window_input_api_t* api, const spectre_input_state_t* state)
        : m_api(api), m_state(state) {}

    inline bool is_key_down(int key) const {
        return m_api->is_key_down(m_state, key);
    }

    inline bool is_key_pressed(int key) const {
        return m_api->is_key_pressed(m_state, key);
    }

    inline bool is_key_released(int key) const {
        return m_api->is_key_released(m_state, key);
    }

    inline void get_mouse_position(float& x, float& y) const {
        m_api->get_mouse_position(m_state, &x, &y);
    }

    inline void get_mouse_delta(float& dx, float& dy) const {
        m_api->get_mouse_delta(m_state, &dx, &dy);
    }

    inline bool should_close() const { return m_api->should_window_close(); }
    inline void set_should_close(bool close) const { m_api->set_should_close(close); }
    inline void set_size(uint32_t width, uint32_t height) const { m_api->set_size(width, height); }
    inline void set_position(int32_t x, int32_t y) const { m_api->set_position(x, y); }
    inline void set_size_limits(uint32_t min_w, uint32_t min_h, uint32_t max_w, uint32_t max_h) const { m_api->set_size_limits(min_w, min_h, max_w, max_h); }
    inline uint32_t get_width() const { return m_api->get_width(); }
    inline uint32_t get_height() const { return m_api->get_height(); }
    inline float get_aspect_ratio() const { return m_api->get_aspect_ratio(); }
    inline int32_t get_position_x() const { return m_api->get_position_x(); }
    inline int32_t get_position_y() const { return m_api->get_position_y(); }
    inline void set_title(const char* title) const { m_api->set_title(title); }
    inline void set_vsync(bool enabled) const { m_api->set_vsync(enabled); }
    inline void set_fullscreen(bool fullscreen) const { m_api->set_fullscreen(fullscreen); }
    inline void set_borderless(bool borderless) const { m_api->set_borderless(borderless); }
    inline void set_resizable(bool resizable) const { m_api->set_resizable(resizable); }
    inline void set_always_on_top(bool on_top) const { m_api->set_always_on_top(on_top); }
    inline const char* get_title() const { return m_api->get_title(); }
    inline bool is_vsync() const { return m_api->is_vsync(); }
    inline bool is_fullscreen() const { return m_api->is_fullscreen(); }
    inline bool is_borderless() const { return m_api->is_borderless(); }
    inline bool is_resizable() const { return m_api->is_resizable(); }
    inline void minimize() const { m_api->minimize(); }
    inline void maximize() const { m_api->maximize(); }
    inline void restore() const { m_api->restore(); }
    inline void show() const { m_api->show(); }
    inline void hide() const { m_api->hide(); }
    inline void request_attention() const { m_api->request_attention(); }
    inline bool is_minimized() const { return m_api->is_minimized(); }
    inline bool is_maximized() const { return m_api->is_maximized(); }
    inline bool is_visible() const { return m_api->is_visible(); }
    inline bool is_focused() const { return m_api->is_focused(); }
    inline bool is_hovered() const { return m_api->is_hovered(); }
    inline void set_cursor_visible(bool visible) const { m_api->set_cursor_visible(visible); }
    inline void set_cursor_locked(bool locked) const { m_api->set_cursor_locked(locked); }
    inline bool is_cursor_visible() const { return m_api->is_cursor_visible(); }
    inline bool is_cursor_locked() const { return m_api->is_cursor_locked(); }
    inline void* get_native_handle() const { return m_api->get_native_handle(); }

private:
    const spectre_window_input_api_t* m_api;
    const spectre_input_state_t* m_state;
};

} // namespace sandbox::sdk
