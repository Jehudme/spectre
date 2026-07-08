#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif



typedef struct spectre_scripts_api_t {
    sandbox_properties_handle_t (*serialize_script)(ecs_world_t* world, ecs_entity_t script);
    ecs_entity_t (*deserialize_script)(ecs_world_t* world, sandbox_properties_handle_t props);
    void (*register_script)(ecs_world_t* world, sandbox_properties_handle_t props);
    bool (*has_script)(ecs_world_t* world, const char* name);
    bool (*is_script)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*find_script)(ecs_world_t* world, const char* name);
    void (*include_code)(ecs_world_t* world, const char* path);
    void (*execute_script)(ecs_world_t* world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count);
} spectre_scripts_api_t;

SANDBOX_DECLARE_SERVICE(spectre_scripts_service_t, spectre_scripts_api_t, {
    .struct_size = 0,
    .name = "scripts",
    .description = "Scripts Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

// --- Public C API ---
static inline sandbox_properties_handle_t spectre_scripts_serialize_script(ecs_world_t* world, ecs_entity_t script) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
    const spectre_scripts_service_t* service = (const spectre_scripts_service_t*)ecs_singleton_get(world, spectre_scripts_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_script) {
        return service->api->serialize_script(world, script);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}
static inline ecs_entity_t spectre_scripts_deserialize_script(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
    const spectre_scripts_service_t* service = (const spectre_scripts_service_t*)ecs_singleton_get(world, spectre_scripts_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_script) {
        return service->api->deserialize_script(world, props);
        
    }
    return (ecs_entity_t){0};
}
static inline void spectre_scripts_register_script(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
    const spectre_scripts_service_t* service = (const spectre_scripts_service_t*)ecs_singleton_get(world, spectre_scripts_service_t);
#endif
#endif
    if (service && service->api && service->api->register_script) {
        service->api->register_script(world, props);
        return;
    }
    
}
static inline bool spectre_scripts_has_script(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
    const spectre_scripts_service_t* service = (const spectre_scripts_service_t*)ecs_singleton_get(world, spectre_scripts_service_t);
#endif
#endif
    if (service && service->api && service->api->has_script) {
        return service->api->has_script(world, name);
        
    }
    return false;
}
static inline bool spectre_scripts_is_script(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
    const spectre_scripts_service_t* service = (const spectre_scripts_service_t*)ecs_singleton_get(world, spectre_scripts_service_t);
#endif
#endif
    if (service && service->api && service->api->is_script) {
        return service->api->is_script(world, entity);
        
    }
    return false;
}
static inline ecs_entity_t spectre_scripts_find_script(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
    const spectre_scripts_service_t* service = (const spectre_scripts_service_t*)ecs_singleton_get(world, spectre_scripts_service_t);
#endif
#endif
    if (service && service->api && service->api->find_script) {
        return service->api->find_script(world, name);
        
    }
    return (ecs_entity_t){0};
}
static inline void spectre_scripts_include_code(ecs_world_t* world, const char* path) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
    const spectre_scripts_service_t* service = (const spectre_scripts_service_t*)ecs_singleton_get(world, spectre_scripts_service_t);
#endif
#endif
    if (service && service->api && service->api->include_code) {
        service->api->include_code(world, path);
        return;
    }
    
}
static inline void spectre_scripts_execute_script(ecs_world_t* world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();
#else
    const spectre_scripts_service_t* service = (const spectre_scripts_service_t*)ecs_singleton_get(world, spectre_scripts_service_t);
#endif
#endif
    if (service && service->api && service->api->execute_script) {
        service->api->execute_script(world, script_entity, args, arg_count);
        return;
    }
    
}

#ifdef __cplusplus
}
#endif
