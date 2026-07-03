#pragma once
#include <stdint.h>
#include <stddef.h>
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*spectre_component_factory_fn)(ecs_world_t* ecs, ecs_entity_t entity, sandbox_properties_handle_t properties);

typedef struct spectre_prefabs_api_t {
    void (*register_component_factory)(ecs_world_t* ecs, const char* name, spectre_component_factory_fn create_fn);
    bool (*has_component_factory)(ecs_world_t* ecs, const char* name);
    ecs_entity_t (*create_prefab)(ecs_world_t* ecs, const char* name, sandbox_properties_handle_t entity_props);
    ecs_entity_t (*create_entity)(ecs_world_t* ecs, sandbox_properties_handle_t entity_props);
} spectre_prefabs_api_t;

SANDBOX_DECLARE_SERVICE(spectre_prefabs_service_t, spectre_prefabs_api_t, {
    .name = "prefabs",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0
})

#ifdef __cplusplus
}
#endif
