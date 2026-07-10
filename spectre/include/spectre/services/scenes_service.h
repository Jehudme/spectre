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
    void (*execute_recursive)(ecs_world_t* world, ecs_entity_t entity, spectre_recursive_callback_t callback, void* payload);
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

// --- Public C API ---
SANDBOX_API sandbox_properties_handle_t spectre_scenes_serialize_state(ecs_world_t* world, ecs_entity_t state);
SANDBOX_API ecs_entity_t spectre_scenes_deserialize_state(ecs_world_t* world, sandbox_properties_handle_t props);
SANDBOX_API sandbox_properties_handle_t spectre_scenes_serialize_scene(ecs_world_t* world, ecs_entity_t scene);
SANDBOX_API ecs_entity_t spectre_scenes_deserialize_scene(ecs_world_t* world, sandbox_properties_handle_t props);
SANDBOX_API void spectre_scenes_register_state(ecs_world_t* world, sandbox_properties_handle_t props);
SANDBOX_API void spectre_scenes_register_scene(ecs_world_t* world, sandbox_properties_handle_t props);
SANDBOX_API ecs_entity_t spectre_scenes_find_state(ecs_world_t* world, const char* name);
SANDBOX_API ecs_entity_t spectre_scenes_find_scene(ecs_world_t* world, const char* name);
SANDBOX_API bool spectre_scenes_has_state(ecs_world_t* world, const char* name);
SANDBOX_API bool spectre_scenes_has_scene(ecs_world_t* world, const char* name);
SANDBOX_API bool spectre_scenes_is_state(ecs_world_t* world, ecs_entity_t entity);
SANDBOX_API bool spectre_scenes_is_scene(ecs_world_t* world, ecs_entity_t entity);
SANDBOX_API ecs_entity_t spectre_scenes_find_current_state(ecs_world_t* world);
SANDBOX_API ecs_query_t* spectre_scenes_find_current_scenes(ecs_world_t* world);
SANDBOX_API void spectre_scenes_push_state(ecs_world_t* world, ecs_entity_t state);
SANDBOX_API void spectre_scenes_pop_state(ecs_world_t* world);
SANDBOX_API void spectre_scenes_execute_recursive(ecs_world_t* world, ecs_entity_t entity, spectre_recursive_callback_t callback, void* payload);

#ifdef __cplusplus
}
#endif
