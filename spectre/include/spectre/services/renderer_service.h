#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif



typedef struct spectre_renderer_api_t {
    ecs_entity_t (*deserialize_renderer)(ecs_world_t* world, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize_renderer)(ecs_world_t* world, ecs_entity_t renderer);
    void (*register_renderer)(ecs_world_t* world, sandbox_properties_handle_t props);
    bool (*is_renderer)(ecs_world_t* world);
} spectre_renderer_api_t;

SANDBOX_DECLARE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t, {
    .struct_size = 0,
    .name = "renderer",
    .description = "Renderer Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

// --- Public C API ---
static inline ecs_entity_t spectre_renderer_deserialize_renderer(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
    const spectre_renderer_service_t* service = (const spectre_renderer_service_t*)ecs_singleton_get(world, spectre_renderer_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_renderer) {
        return service->api->deserialize_renderer(world, props);
        
    }
    return (ecs_entity_t){0};
}
static inline sandbox_properties_handle_t spectre_renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
    const spectre_renderer_service_t* service = (const spectre_renderer_service_t*)ecs_singleton_get(world, spectre_renderer_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_renderer) {
        return service->api->serialize_renderer(world, renderer);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}
static inline void spectre_renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
    const spectre_renderer_service_t* service = (const spectre_renderer_service_t*)ecs_singleton_get(world, spectre_renderer_service_t);
#endif
#endif
    if (service && service->api && service->api->register_renderer) {
        service->api->register_renderer(world, props);
        return;
    }
    
}
static inline bool spectre_renderer_is_renderer(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
    const spectre_renderer_service_t* service = (const spectre_renderer_service_t*)ecs_singleton_get(world, spectre_renderer_service_t);
#endif
#endif
    if (service && service->api && service->api->is_renderer) {
        return service->api->is_renderer(world);
        
    }
    return false;
}

#ifdef __cplusplus
}
#endif
