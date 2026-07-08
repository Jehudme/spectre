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

// --- Public C API ---
static inline ecs_entity_t spectre_window_deserialize_window(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_window) {
        return service->api->deserialize_window(world, props);
        
    }
    return (ecs_entity_t){0};
}
static inline sandbox_properties_handle_t spectre_window_serialize_window(ecs_world_t* world, ecs_entity_t window) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_window) {
        return service->api->serialize_window(world, window);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}
static inline void spectre_window_register_window(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->register_window) {
        service->api->register_window(world, props);
        return;
    }
    
}
static inline bool spectre_window_should_close(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->should_close) {
        return service->api->should_close(world);
        
    }
    return false;
}
static inline void spectre_window_set_should_close(ecs_world_t* world, bool close) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_should_close) {
        service->api->set_should_close(world, close);
        return;
    }
    
}
static inline void spectre_window_set_size(ecs_world_t* world, uint32_t width, uint32_t height) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_size) {
        service->api->set_size(world, width, height);
        return;
    }
    
}
static inline void spectre_window_set_position(ecs_world_t* world, int32_t x, int32_t y) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_position) {
        service->api->set_position(world, x, y);
        return;
    }
    
}
static inline void spectre_window_set_size_limits(ecs_world_t* world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_size_limits) {
        service->api->set_size_limits(world, min_width, min_height, max_width, max_height);
        return;
    }
    
}
static inline uint32_t spectre_window_get_width(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_width) {
        return service->api->get_width(world);
        
    }
    return 0;
}
static inline uint32_t spectre_window_get_height(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_height) {
        return service->api->get_height(world);
        
    }
    return 0;
}
static inline float spectre_window_get_aspect_ratio(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_aspect_ratio) {
        return service->api->get_aspect_ratio(world);
        
    }
    return 0;
}
static inline int32_t spectre_window_get_position_x(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_position_x) {
        return service->api->get_position_x(world);
        
    }
    return 0;
}
static inline int32_t spectre_window_get_position_y(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_position_y) {
        return service->api->get_position_y(world);
        
    }
    return 0;
}
static inline void spectre_window_set_title(ecs_world_t* world, const char* title) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_title) {
        service->api->set_title(world, title);
        return;
    }
    
}
static inline void spectre_window_set_vsync(ecs_world_t* world, bool enabled) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_vsync) {
        service->api->set_vsync(world, enabled);
        return;
    }
    
}
static inline void spectre_window_set_fullscreen(ecs_world_t* world, bool fullscreen) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_fullscreen) {
        service->api->set_fullscreen(world, fullscreen);
        return;
    }
    
}
static inline void spectre_window_set_borderless(ecs_world_t* world, bool borderless) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_borderless) {
        service->api->set_borderless(world, borderless);
        return;
    }
    
}
static inline void spectre_window_set_resizable(ecs_world_t* world, bool resizable) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_resizable) {
        service->api->set_resizable(world, resizable);
        return;
    }
    
}
static inline void spectre_window_set_always_on_top(ecs_world_t* world, bool always_on_top) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_always_on_top) {
        service->api->set_always_on_top(world, always_on_top);
        return;
    }
    
}
static inline const char* spectre_window_get_title(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_title) {
        return service->api->get_title(world);
        
    }
    return NULL;
}
static inline bool spectre_window_is_vsync(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_vsync) {
        return service->api->is_vsync(world);
        
    }
    return false;
}
static inline bool spectre_window_is_fullscreen(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_fullscreen) {
        return service->api->is_fullscreen(world);
        
    }
    return false;
}
static inline bool spectre_window_is_borderless(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_borderless) {
        return service->api->is_borderless(world);
        
    }
    return false;
}
static inline bool spectre_window_is_resizable(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_resizable) {
        return service->api->is_resizable(world);
        
    }
    return false;
}
static inline void spectre_window_minimize(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->minimize) {
        service->api->minimize(world);
        return;
    }
    
}
static inline void spectre_window_maximize(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->maximize) {
        service->api->maximize(world);
        return;
    }
    
}
static inline void spectre_window_restore(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->restore) {
        service->api->restore(world);
        return;
    }
    
}
static inline void spectre_window_show(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->show) {
        service->api->show(world);
        return;
    }
    
}
static inline void spectre_window_hide(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->hide) {
        service->api->hide(world);
        return;
    }
    
}
static inline void spectre_window_request_attention(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->request_attention) {
        service->api->request_attention(world);
        return;
    }
    
}
static inline bool spectre_window_is_minimized(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_minimized) {
        return service->api->is_minimized(world);
        
    }
    return false;
}
static inline bool spectre_window_is_maximized(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_maximized) {
        return service->api->is_maximized(world);
        
    }
    return false;
}
static inline bool spectre_window_is_visible(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_visible) {
        return service->api->is_visible(world);
        
    }
    return false;
}
static inline bool spectre_window_is_focused(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_focused) {
        return service->api->is_focused(world);
        
    }
    return false;
}
static inline bool spectre_window_is_hovered(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_hovered) {
        return service->api->is_hovered(world);
        
    }
    return false;
}
static inline void spectre_window_set_cursor_visible(ecs_world_t* world, bool visible) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_cursor_visible) {
        service->api->set_cursor_visible(world, visible);
        return;
    }
    
}
static inline void spectre_window_set_cursor_locked(ecs_world_t* world, bool locked) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->set_cursor_locked) {
        service->api->set_cursor_locked(world, locked);
        return;
    }
    
}
static inline bool spectre_window_is_cursor_visible(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_cursor_visible) {
        return service->api->is_cursor_visible(world);
        
    }
    return false;
}
static inline bool spectre_window_is_cursor_locked(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_cursor_locked) {
        return service->api->is_cursor_locked(world);
        
    }
    return false;
}
static inline void* spectre_window_get_native_handle(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_native_handle) {
        return service->api->get_native_handle(world);
        
    }
    return NULL;
}
static inline bool spectre_window_is_key_down(ecs_world_t* world, int keycode) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_key_down) {
        return service->api->is_key_down(world, keycode);
        
    }
    return false;
}
static inline bool spectre_window_is_key_pressed(ecs_world_t* world, int keycode) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_key_pressed) {
        return service->api->is_key_pressed(world, keycode);
        
    }
    return false;
}
static inline bool spectre_window_is_key_released(ecs_world_t* world, int keycode) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->is_key_released) {
        return service->api->is_key_released(world, keycode);
        
    }
    return false;
}
static inline float spectre_window_get_mouse_x(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_mouse_x) {
        return service->api->get_mouse_x(world);
        
    }
    return 0;
}
static inline float spectre_window_get_mouse_y(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_mouse_y) {
        return service->api->get_mouse_y(world);
        
    }
    return 0;
}
static inline float spectre_window_get_mouse_delta_x(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_mouse_delta_x) {
        return service->api->get_mouse_delta_x(world);
        
    }
    return 0;
}
static inline float spectre_window_get_mouse_delta_y(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();
#else
    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_singleton_get(world, spectre_window_service_t);
#endif
#endif
    if (service && service->api && service->api->get_mouse_delta_y) {
        return service->api->get_mouse_delta_y(world);
        
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
