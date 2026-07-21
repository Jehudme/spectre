#pragma once

#include <flecs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_disable_rendering_t {
    char dummy;
} spectre_disable_rendering_t;

typedef struct spectre_state_t {
    char dummy;
} spectre_state_t;
typedef struct spectre_scene_t {
    char dummy;
} spectre_scene_t;
typedef struct spectre_state_use_scene_relation_t {
    int layer_index;
} spectre_state_use_scene_relation_t;

typedef void (*spectre_recursive_callback_t)(ecs_world_t* world, ecs_entity_t entity, void* payload);

#ifdef __cplusplus
} // extern "C"

struct spectre_scene_context_t {
    flecs::entity scene_entity;
};

struct spectre_state_context_t {
    flecs::entity state_entity;
};
#endif
