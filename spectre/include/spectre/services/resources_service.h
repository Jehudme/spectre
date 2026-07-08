#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif



typedef struct spectre_resources_api_t {
    ecs_entity_t (*deserialize_resource)(ecs_world_t* world, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
    void (*register_resource_loader)(ecs_world_t* world, const char* type, spectre_resource_loader_t loader);
    void (*register_resource)(ecs_world_t* world, sandbox_properties_handle_t props);
    bool (*has_resource_loader)(ecs_world_t* world, const char* type);
    bool (*has_resource)(ecs_world_t* world, const char* name);
    bool (*is_resource)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*find_resource_loader)(ecs_world_t* world, const char* type);
    ecs_entity_t (*find_resource)(ecs_world_t* world, const char* name);
    bool (*is_resource_loaded)(ecs_world_t* world, ecs_entity_t resource);
    void (*load_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
    void (*free_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
} spectre_resources_api_t;

SANDBOX_DECLARE_SERVICE(spectre_resources_service_t, spectre_resources_api_t, {
    .struct_size = 0,
    .name = "resources",
    .description = "Resources Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

// --- Public C API ---
static inline ecs_entity_t spectre_resources_deserialize_resource(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_resource) {
        return service->api->deserialize_resource(world, props);
        
    }
    return (ecs_entity_t){0};
}
static inline sandbox_properties_handle_t spectre_resources_serialize_resource(ecs_world_t* world, ecs_entity_t resourceEntity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_resource) {
        return service->api->serialize_resource(world, resourceEntity);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}
static inline void spectre_resources_register_resource_loader(ecs_world_t* world, const char* type, spectre_resource_loader_t loader) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->register_resource_loader) {
        service->api->register_resource_loader(world, type, loader);
        return;
    }
    
}
static inline void spectre_resources_register_resource(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->register_resource) {
        service->api->register_resource(world, props);
        return;
    }
    
}
static inline bool spectre_resources_has_resource_loader(ecs_world_t* world, const char* type) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->has_resource_loader) {
        return service->api->has_resource_loader(world, type);
        
    }
    return false;
}
static inline bool spectre_resources_has_resource(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->has_resource) {
        return service->api->has_resource(world, name);
        
    }
    return false;
}
static inline bool spectre_resources_is_resource(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->is_resource) {
        return service->api->is_resource(world, entity);
        
    }
    return false;
}
static inline ecs_entity_t spectre_resources_find_resource_loader(ecs_world_t* world, const char* type) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->find_resource_loader) {
        return service->api->find_resource_loader(world, type);
        
    }
    return (ecs_entity_t){0};
}
static inline ecs_entity_t spectre_resources_find_resource(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->find_resource) {
        return service->api->find_resource(world, name);
        
    }
    return (ecs_entity_t){0};
}
static inline bool spectre_resources_is_resource_loaded(ecs_world_t* world, ecs_entity_t resource) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->is_resource_loaded) {
        return service->api->is_resource_loaded(world, resource);
        
    }
    return false;
}
static inline void spectre_resources_load_resource(ecs_world_t* world, ecs_entity_t resourceEntity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->load_resource) {
        service->api->load_resource(world, resourceEntity);
        return;
    }
    
}
static inline void spectre_resources_free_resource(ecs_world_t* world, ecs_entity_t resourceEntity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->free_resource) {
        service->api->free_resource(world, resourceEntity);
        return;
    }
    
}

#ifdef __cplusplus
}
#endif
