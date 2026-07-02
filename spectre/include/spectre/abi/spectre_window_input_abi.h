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
