#pragma once

#include <sandbox/abi/bootstrapper.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pure-C virtual table bridging the Window module across the ABI boundary.
 *
 * Callers obtain a pointer to this struct via the spectre_window_service_t service.
 * The `instance` field is an opaque pointer to the internal C++ module object and
 * must be passed back to every function that requires it.
 *
 * All setters are fire-and-forget: they return void and silently fail if `instance`
 * is null or the underlying platform call is unsupported.
 */
typedef struct spectre_window_api_t {
    /** Opaque pointer to the WindowModule instance. Do not modify. */
    void* instance;

    // ── Application Loop ─────────────────────────────────────────────────────
    /** Returns true when the OS or user has requested the window to close. */
    bool (*should_close)(const void* instance);
    /** Reserved — not all backends support programmatic close-request. */
    void (*set_should_close)(void* instance, bool should_close);

    // ── Dimensions & Position ────────────────────────────────────────────────
    void     (*set_size)(void* instance, uint32_t width, uint32_t height);
    void     (*set_position)(void* instance, int32_t x, int32_t y);
    void     (*set_size_limits)(void* instance, uint32_t min_width, uint32_t min_height,
                                                uint32_t max_width, uint32_t max_height);
    uint32_t (*get_width)(const void* instance);
    uint32_t (*get_height)(const void* instance);
    float    (*get_aspect_ratio)(const void* instance);
    int32_t  (*get_position_x)(const void* instance);
    int32_t  (*get_position_y)(const void* instance);

    // ── Window Attributes ────────────────────────────────────────────────────
    void        (*set_title)(void* instance, const char* title);
    void        (*set_vsync)(void* instance, bool enabled);
    void        (*set_fullscreen)(void* instance, bool fullscreen);
    void        (*set_borderless)(void* instance, bool borderless);
    void        (*set_resizable)(void* instance, bool resizable);
    void        (*set_always_on_top)(void* instance, bool always_on_top);
    const char* (*get_title)(const void* instance);
    bool        (*is_vsync)(const void* instance);
    bool        (*is_fullscreen)(const void* instance);
    bool        (*is_borderless)(const void* instance);
    bool        (*is_resizable)(const void* instance);

    // ── Window Actions ───────────────────────────────────────────────────────
    void (*minimize)(void* instance);
    void (*maximize)(void* instance);
    void (*restore)(void* instance);
    void (*show)(void* instance);
    void (*hide)(void* instance);
    void (*request_attention)(void* instance);

    // ── Window State ─────────────────────────────────────────────────────────
    bool (*is_minimized)(const void* instance);
    bool (*is_maximized)(const void* instance);
    bool (*is_visible)(const void* instance);
    bool (*is_focused)(const void* instance);
    bool (*is_hovered)(const void* instance);

    // ── Cursor ───────────────────────────────────────────────────────────────
    void (*set_cursor_visible)(void* instance, bool visible);
    void (*set_cursor_locked)(void* instance, bool locked);
    bool (*is_cursor_visible)(const void* instance);
    bool (*is_cursor_locked)(const void* instance);

    // ── Native / Platform ────────────────────────────────────────────────────
    /** Returns the platform-specific window handle (e.g. HWND on Windows, NSWindow on macOS). */
    void* (*get_native_handle)(const void* instance);

} spectre_window_api_t;

/**
 * @brief Service descriptor for the Window module.
 *        Retrieve via SANDBOX_GET_SERVICE(world, spectre_window_service_t).
 */
SANDBOX_DECLARE_SERVICE(spectre_window_service_t, spectre_window_api_t, {
    .name        = "window",
    .description = "Manages the OS window, framerate, and cursor state.",
    .architecture  = "spectre",
    .version_major = 1,
    .version_minor = 0
})

#ifdef __cplusplus
}
#endif