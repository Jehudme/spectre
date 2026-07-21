#pragma once

#include "sandbox/abi/properties.h"
#include <flecs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_serializer_component {
    // We update this struct to take the target entity to allow direct injection
    // on existing entities
    void (*deserialize)(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize)(ecs_world_t* world, ecs_entity_t entity);
} spectre_serializer_component;

typedef struct spectre_serializer_relation_t {
    char dummy;
} spectre_serializer_relation_t;

#ifdef __cplusplus
}
#endif
