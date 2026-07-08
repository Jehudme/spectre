#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif


// --- Public C API ---
SANDBOX_API sandbox_properties_handle_t spectre_prefabs_serialize_entity(ecs_world_t* entity_world, ecs_entity_t entity);
SANDBOX_API ecs_entity_t spectre_prefabs_deserialize_entity(ecs_world_t* entity_world, sandbox_properties_handle_t props);
SANDBOX_API void spectre_prefabs_register_prefab(ecs_world_t* entity_world, sandbox_properties_handle_t props);
SANDBOX_API bool spectre_prefabs_has_prefab(ecs_world_t* entity_world, const char* name);
SANDBOX_API bool spectre_prefabs_is_prefab(ecs_world_t* entity_world, ecs_entity_t entity);
SANDBOX_API ecs_entity_t spectre_prefabs_find_prefab(ecs_world_t* entity_world, const char* name);
SANDBOX_API ecs_entity_t spectre_prefabs_create_entity_from_props(ecs_world_t* entity_world, sandbox_properties_handle_t props);
SANDBOX_API ecs_entity_t spectre_prefabs_create_entity_from_prefab(ecs_world_t* entity_world, ecs_entity_t prefab);
SANDBOX_API ecs_entity_t spectre_prefabs_create_entity_from_name(ecs_world_t* entity_world, const char* name);

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

#ifdef __cplusplus
}
#endif
