#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/spectre.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif



typedef struct spectre_serializer_api_t {
    void (*register_serializer)(ecs_world_t* world, const char* type, const spectre_serializer_component* serializer);
    bool (*has_serializer)(ecs_world_t* world, const char* type);
    bool (*is_serializer)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*find_serializer)(ecs_world_t* world, const char* type);
    sandbox_properties_handle_t (*serialize_entity)(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);
    void (*deserialize_entity)(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity, sandbox_properties_handle_t props);
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
SANDBOX_API void spectre_serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_component* serializer);
SANDBOX_API bool spectre_serializer_has_serializer(ecs_world_t* world, const char* type);
SANDBOX_API bool spectre_serializer_is_serializer(ecs_world_t* world, ecs_entity_t entity);
SANDBOX_API ecs_entity_t spectre_serializer_find_serializer(ecs_world_t* world, const char* type);
SANDBOX_API sandbox_properties_handle_t spectre_serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);
SANDBOX_API void spectre_serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity, sandbox_properties_handle_t props);

#ifdef __cplusplus
}
#endif
