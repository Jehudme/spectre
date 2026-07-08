#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif



typedef struct spectre_serializer_api_t {
    void (*register_serializer)(ecs_world_t* world, const char* type, const spectre_serializer_t* serializer);
    bool (*has_serializer)(ecs_world_t* world, const char* type);
    ecs_entity_t (*find_serializer)(ecs_world_t* world, const char* type);
    sandbox_properties_handle_t (*serialize_entity)(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);
    ecs_entity_t (*deserialize_entity)(ecs_world_t* world, ecs_entity_t serializer, sandbox_properties_handle_t props);
} spectre_serializer_api_t;

SANDBOX_DECLARE_SERVICE(spectre_serializer_service_t, spectre_serializer_api_t, {
    .struct_size = 0,
    .name = "serializer",
    .description = "Serializer Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

// --- Public C API ---
static inline void spectre_serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_t* serializer) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->register_serializer) {
        service->api->register_serializer(world, type, serializer);
        return;
    }
    
}
static inline bool spectre_serializer_has_serializer(ecs_world_t* world, const char* type) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->has_serializer) {
        return service->api->has_serializer(world, type);
        
    }
    return false;
}
static inline ecs_entity_t spectre_serializer_find_serializer(ecs_world_t* world, const char* type) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->find_serializer) {
        return service->api->find_serializer(world, type);
        
    }
    return (ecs_entity_t){0};
}
static inline sandbox_properties_handle_t spectre_serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_entity) {
        return service->api->serialize_entity(world, serializer, entity);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}
static inline ecs_entity_t spectre_serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_entity) {
        return service->api->deserialize_entity(world, serializer, props);
        
    }
    return (ecs_entity_t){0};
}

#ifdef __cplusplus
}
#endif
