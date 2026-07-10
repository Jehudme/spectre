#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_input_state_t {
    uint64_t keys_down[8];
    uint64_t keys_pressed[8];
    uint64_t keys_released[8];
    float mouse_position[2];
    float mouse_delta[2];
} spectre_input_state_t;

typedef struct spectre_window_component_t {
    uint32_t width;
    uint32_t height;
    int32_t position_x;
    int32_t position_y;
    uint32_t min_width;
    uint32_t min_height;
    uint32_t max_width;
    uint32_t max_height;
    const char* title;
    bool vsync;
    bool fullscreen;
    bool borderless;
    bool resizable;
    bool always_on_top;
    bool minimized;
    bool maximized;
    bool visible;
    bool focused;
    bool hovered;
    bool cursor_visible;
    bool cursor_locked;
    void* native_handle;
} spectre_window_component_t;

#ifdef __cplusplus
}
#endif
