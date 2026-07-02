#pragma once

#include <spectre/abi/spectre_window_input_abi.h>

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

    inline bool should_close() const {
        return m_api->should_window_close();
    }

private:
    const spectre_window_input_api_t* m_api;
    const spectre_input_state_t* m_state;
};

} // namespace sandbox::sdk
