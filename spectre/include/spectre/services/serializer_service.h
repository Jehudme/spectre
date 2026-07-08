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

#ifdef __cplusplus
}
#endif
