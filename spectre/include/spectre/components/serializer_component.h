#pragma once

#include <flecs.h>
#include "sandbox/abi/properties.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_serializer_component {
    ecs_entity_t (*deserialize)(ecs_world_t* world, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize)(ecs_world_t* world, ecs_entity_t entity);
} spectre_serializer_component;

typedef struct spectre_serializer_relation_t {
    char dummy;
} spectre_serializer_relation_t;

#ifdef __cplusplus
}
#endif
