#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif



typedef struct spectre_prefabs_api_t {
    sandbox_properties_handle_t (*serialize_entity)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*deserialize_entity)(ecs_world_t* world, sandbox_properties_handle_t props);
    void (*register_prefab)(ecs_world_t* world, sandbox_properties_handle_t props);
    bool (*has_prefab)(ecs_world_t* world, const char* name);
    bool (*is_prefab)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*find_prefab)(ecs_world_t* world, const char* name);
    ecs_entity_t (*create_entity_from_props)(ecs_world_t* world, sandbox_properties_handle_t props);
    ecs_entity_t (*create_entity_from_prefab)(ecs_world_t* world, ecs_entity_t prefab);
    ecs_entity_t (*create_entity_from_name)(ecs_world_t* world, const char* name);
} spectre_prefabs_api_t;

SANDBOX_DECLARE_SERVICE(spectre_prefabs_service_t, spectre_prefabs_api_t, {
    .struct_size = 0,
    .name = "prefabs",
    .description = "Prefabs Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

// --- Public C API ---
static inline sandbox_properties_handle_t spectre_prefabs_serialize_entity(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_entity) {
        return service->api->serialize_entity(world, entity);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}
static inline ecs_entity_t spectre_prefabs_deserialize_entity(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_entity) {
        return service->api->deserialize_entity(world, props);
        
    }
    return (ecs_entity_t){0};
}
static inline void spectre_prefabs_register_prefab(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->register_prefab) {
        service->api->register_prefab(world, props);
        return;
    }
    
}
static inline bool spectre_prefabs_has_prefab(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->has_prefab) {
        return service->api->has_prefab(world, name);
        
    }
    return false;
}
static inline bool spectre_prefabs_is_prefab(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->is_prefab) {
        return service->api->is_prefab(world, entity);
        
    }
    return false;
}
static inline ecs_entity_t spectre_prefabs_find_prefab(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->find_prefab) {
        return service->api->find_prefab(world, name);
        
    }
    return (ecs_entity_t){0};
}
static inline ecs_entity_t spectre_prefabs_create_entity_from_props(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->create_entity_from_props) {
        return service->api->create_entity_from_props(world, props);
        
    }
    return (ecs_entity_t){0};
}
static inline ecs_entity_t spectre_prefabs_create_entity_from_prefab(ecs_world_t* world, ecs_entity_t prefab) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->create_entity_from_prefab) {
        return service->api->create_entity_from_prefab(world, prefab);
        
    }
    return (ecs_entity_t){0};
}
static inline ecs_entity_t spectre_prefabs_create_entity_from_name(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->create_entity_from_name) {
        return service->api->create_entity_from_name(world, name);
        
    }
    return (ecs_entity_t){0};
}

#ifdef __cplusplus
}
#endif
