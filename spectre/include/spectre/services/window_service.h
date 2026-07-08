#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_window_api_t {
    ecs_entity_t (*deserialize_window)(ecs_world_t* world, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize_window)(ecs_world_t* world, ecs_entity_t window);
    void (*register_window)(ecs_world_t* world, sandbox_properties_handle_t props);
    bool (*should_close)(ecs_world_t* world);
    void (*set_should_close)(ecs_world_t* world, bool close);
    void (*set_size)(ecs_world_t* world, uint32_t width, uint32_t height);
    void (*set_position)(ecs_world_t* world, int32_t x, int32_t y);
    void (*set_size_limits)(ecs_world_t* world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height);
    uint32_t (*get_width)(ecs_world_t* world);
    uint32_t (*get_height)(ecs_world_t* world);
    float (*get_aspect_ratio)(ecs_world_t* world);
    int32_t (*get_position_x)(ecs_world_t* world);
    int32_t (*get_position_y)(ecs_world_t* world);
    void (*set_title)(ecs_world_t* world, const char* title);
    void (*set_vsync)(ecs_world_t* world, bool enabled);
    void (*set_fullscreen)(ecs_world_t* world, bool fullscreen);
    void (*set_borderless)(ecs_world_t* world, bool borderless);
    void (*set_resizable)(ecs_world_t* world, bool resizable);
    void (*set_always_on_top)(ecs_world_t* world, bool always_on_top);
    const char* (*get_title)(ecs_world_t* world);
    bool (*is_vsync)(ecs_world_t* world);
    bool (*is_fullscreen)(ecs_world_t* world);
    bool (*is_borderless)(ecs_world_t* world);
    bool (*is_resizable)(ecs_world_t* world);
    void (*minimize)(ecs_world_t* world);
    void (*maximize)(ecs_world_t* world);
    void (*restore)(ecs_world_t* world);
    void (*show)(ecs_world_t* world);
    void (*hide)(ecs_world_t* world);
    void (*request_attention)(ecs_world_t* world);
    bool (*is_minimized)(ecs_world_t* world);
    bool (*is_maximized)(ecs_world_t* world);
    bool (*is_visible)(ecs_world_t* world);
    bool (*is_focused)(ecs_world_t* world);
    bool (*is_hovered)(ecs_world_t* world);
    void (*set_cursor_visible)(ecs_world_t* world, bool visible);
    void (*set_cursor_locked)(ecs_world_t* world, bool locked);
    bool (*is_cursor_visible)(ecs_world_t* world);
    bool (*is_cursor_locked)(ecs_world_t* world);
    void* (*get_native_handle)(ecs_world_t* world);
    bool (*is_key_down)(ecs_world_t* world, int keycode);
    bool (*is_key_pressed)(ecs_world_t* world, int keycode);
    bool (*is_key_released)(ecs_world_t* world, int keycode);
    float (*get_mouse_x)(ecs_world_t* world);
    float (*get_mouse_y)(ecs_world_t* world);
    float (*get_mouse_delta_x)(ecs_world_t* world);
    float (*get_mouse_delta_y)(ecs_world_t* world);
} spectre_window_api_t;

SANDBOX_DECLARE_SERVICE(spectre_window_service_t, spectre_window_api_t, {
    .struct_size = 0,
    .name = "window",
    .description = "Window Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

#ifdef __cplusplus
}
#endif
