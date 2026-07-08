#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_scenes_api_t {
    sandbox_properties_handle_t (*serialize_state)(ecs_world_t* world, ecs_entity_t state);
    ecs_entity_t (*deserialize_state)(ecs_world_t* world, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize_scene)(ecs_world_t* world, ecs_entity_t scene);
    ecs_entity_t (*deserialize_scene)(ecs_world_t* world, sandbox_properties_handle_t props);
    void (*register_state)(ecs_world_t* world, sandbox_properties_handle_t props);
    void (*register_scene)(ecs_world_t* world, sandbox_properties_handle_t props);
    ecs_entity_t (*find_state)(ecs_world_t* world, const char* name);
    ecs_entity_t (*find_scene)(ecs_world_t* world, const char* name);
    bool (*has_state)(ecs_world_t* world, const char* name);
    bool (*has_scene)(ecs_world_t* world, const char* name);
    bool (*is_state)(ecs_world_t* world, ecs_entity_t entity);
    bool (*is_scene)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*find_current_state)(ecs_world_t* world);
    ecs_query_t* (*find_current_scenes)(ecs_world_t* world);
    void (*push_state)(ecs_world_t* world, ecs_entity_t state);
    void (*pop_state)(ecs_world_t* world);
} spectre_scenes_api_t;

SANDBOX_DECLARE_SERVICE(spectre_scenes_service_t, spectre_scenes_api_t, {
    .struct_size = 0,
    .name = "scenes",
    .description = "Scenes Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

#ifdef __cplusplus
}
#endif
