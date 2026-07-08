#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif



typedef struct spectre_scenes_api_t {
    sandbox_properties_handle_t (*serialize_state)(ecs_world_t* world, ecs_entity_t state);
    ecs_entity_t (*deserialize_state)(ecs_world_t* world, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize_scene)(ecs_world_t* world, ecs_entity_t scene);
    ecs_entity_t (*deserialize_scene)(ecs_world_t* world, sandbox_properties_handle_t props);
    void (*register_state)(ecs_world_t* world, sandbox_properties_handle_t props);
    void (*register_scene)(ecs_world_t* world, sandbox_properties_handle_t props);
    ecs_entity_t (*find_state)(ecs_world_t* world, const char* name);
    ecs_entity_t (*find_scene)(ecs_world_t* world, const char* name);
    bool (*has_state)(ecs_world_t* world, const char* name);
    bool (*has_scene)(ecs_world_t* world, const char* name);
    bool (*is_state)(ecs_world_t* world, ecs_entity_t entity);
    bool (*is_scene)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*find_current_state)(ecs_world_t* world);
    ecs_query_t* (*find_current_scenes)(ecs_world_t* world);
    void (*push_state)(ecs_world_t* world, ecs_entity_t state);
    void (*pop_state)(ecs_world_t* world);
} spectre_scenes_api_t;

SANDBOX_DECLARE_SERVICE(spectre_scenes_service_t, spectre_scenes_api_t, {
    .struct_size = 0,
    .name = "scenes",
    .description = "Scenes Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

// --- Public C API ---
static inline sandbox_properties_handle_t spectre_scenes_serialize_state(ecs_world_t* world, ecs_entity_t state) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_state) {
        return service->api->serialize_state(world, state);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}
static inline ecs_entity_t spectre_scenes_deserialize_state(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_state) {
        return service->api->deserialize_state(world, props);
        
    }
    return (ecs_entity_t){0};
}
static inline sandbox_properties_handle_t spectre_scenes_serialize_scene(ecs_world_t* world, ecs_entity_t scene) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_scene) {
        return service->api->serialize_scene(world, scene);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}
static inline ecs_entity_t spectre_scenes_deserialize_scene(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_scene) {
        return service->api->deserialize_scene(world, props);
        
    }
    return (ecs_entity_t){0};
}
static inline void spectre_scenes_register_state(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->register_state) {
        service->api->register_state(world, props);
        return;
    }
    
}
static inline void spectre_scenes_register_scene(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->register_scene) {
        service->api->register_scene(world, props);
        return;
    }
    
}
static inline ecs_entity_t spectre_scenes_find_state(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->find_state) {
        return service->api->find_state(world, name);
        
    }
    return (ecs_entity_t){0};
}
static inline ecs_entity_t spectre_scenes_find_scene(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->find_scene) {
        return service->api->find_scene(world, name);
        
    }
    return (ecs_entity_t){0};
}
static inline bool spectre_scenes_has_state(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->has_state) {
        return service->api->has_state(world, name);
        
    }
    return false;
}
static inline bool spectre_scenes_has_scene(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->has_scene) {
        return service->api->has_scene(world, name);
        
    }
    return false;
}
static inline bool spectre_scenes_is_state(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->is_state) {
        return service->api->is_state(world, entity);
        
    }
    return false;
}
static inline bool spectre_scenes_is_scene(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->is_scene) {
        return service->api->is_scene(world, entity);
        
    }
    return false;
}
static inline ecs_entity_t spectre_scenes_find_current_state(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->find_current_state) {
        return service->api->find_current_state(world);
        
    }
    return (ecs_entity_t){0};
}
static inline ecs_query_t* spectre_scenes_find_current_scenes(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->find_current_scenes) {
        return service->api->find_current_scenes(world);
        
    }
    return NULL;
}
static inline void spectre_scenes_push_state(ecs_world_t* world, ecs_entity_t state) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->push_state) {
        service->api->push_state(world, state);
        return;
    }
    
}
static inline void spectre_scenes_pop_state(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->pop_state) {
        service->api->pop_state(world);
        return;
    }
    
}

#ifdef __cplusplus
}
#endif
