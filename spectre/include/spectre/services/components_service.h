#pragma once

#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ecs_entity_t (*spectre_component_registration_fn_t)(ecs_world_t* world);

typedef struct spectre_components_api_t {
    sandbox_properties_handle_t (*serialize_component)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*deserialize_component)(ecs_world_t* world, const char* type, sandbox_properties_handle_t props);
    void (*register_component)(ecs_world_t* world, const char* type, spectre_component_registration_fn_t registration, spectre_serializer_component serializer);
    bool (*has_component)(ecs_world_t* world, const char* type);
    bool (*is_component)(ecs_world_t* world, ecs_entity_t entity);
} spectre_components_api_t;

SANDBOX_DECLARE_SERVICE(spectre_components_service_t, spectre_components_api_t, {
    .struct_size = 0,
    .name = "components",
    .description = "Components Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

// --- Public C API ---
SANDBOX_API sandbox_properties_handle_t spectre_components_serialize_component(ecs_world_t* world, ecs_entity_t entity);
SANDBOX_API ecs_entity_t spectre_components_deserialize_component(ecs_world_t* world, const char* type, sandbox_properties_handle_t props);
SANDBOX_API void spectre_components_register_component(ecs_world_t* world, const char* type, spectre_component_registration_fn_t registration, spectre_serializer_component serializer);
SANDBOX_API bool spectre_components_has_component(ecs_world_t* world, const char* type);
SANDBOX_API bool spectre_components_is_component(ecs_world_t* world, ecs_entity_t entity);

#ifdef __cplusplus
}
#endif
