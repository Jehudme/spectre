#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <sandbox/abi/bootstrapper.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Compact input state structure packing 512 keyboard keys 
 *        into 64-bit integers for cache-friendly ABI transport.
 */
typedef struct spectre_input_state_t {
    uint64_t keys_down[8];
    uint64_t keys_pressed[8];
    uint64_t keys_released[8];
    float mouse_position[2];
    float mouse_delta[2];
} spectre_input_state_t;

/**
 * @brief Opaque function pointer table to query window and input states.
 */
typedef struct spectre_window_input_api_t {
    bool (*is_key_down)(const spectre_input_state_t* state, int key);
    bool (*is_key_pressed)(const spectre_input_state_t* state, int key);
    bool (*is_key_released)(const spectre_input_state_t* state, int key);
    void (*get_mouse_position)(const spectre_input_state_t* state, float* x, float* y);
    void (*get_mouse_delta)(const spectre_input_state_t* state, float* dx, float* dy);

    bool (*should_window_close)(void);
    void (*set_should_close)(bool close);
    void (*set_size)(uint32_t width, uint32_t height);
    void (*set_position)(int32_t x, int32_t y);
    void (*set_size_limits)(uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);
    uint32_t (*get_width)(void);
    uint32_t (*get_height)(void);
    float (*get_aspect_ratio)(void);
    int32_t (*get_position_x)(void);
    int32_t (*get_position_y)(void);
    void (*set_title)(const char* title);
    void (*set_vsync)(bool enabled);
    void (*set_fullscreen)(bool fullscreen);
    void (*set_borderless)(bool borderless);
    void (*set_resizable)(bool resizable);
    void (*set_always_on_top)(bool always_on_top);
    const char* (*get_title)(void);
    bool (*is_vsync)(void);
    bool (*is_fullscreen)(void);
    bool (*is_borderless)(void);
    bool (*is_resizable)(void);
    void (*minimize)(void);
    void (*maximize)(void);
    void (*restore)(void);
    void (*show)(void);
    void (*hide)(void);
    void (*request_attention)(void);
    bool (*is_minimized)(void);
    bool (*is_maximized)(void);
    bool (*is_visible)(void);
    bool (*is_focused)(void);
    bool (*is_hovered)(void);
    void (*set_cursor_visible)(bool visible);
    void (*set_cursor_locked)(bool locked);
    bool (*is_cursor_visible)(void);
    bool (*is_cursor_locked)(void);
    void* (*get_native_handle)(void);
} spectre_window_input_api_t;



SANDBOX_DECLARE_SERVICE(spectre_window_input_service_t, spectre_window_input_api_t, {
    .name = "window",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0
})

#ifdef __cplusplus
}
#endif
