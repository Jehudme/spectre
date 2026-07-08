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

#ifdef __cplusplus
}
#endif
