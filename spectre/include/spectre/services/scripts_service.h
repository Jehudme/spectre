#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif



typedef struct spectre_scripts_api_t {
    sandbox_properties_handle_t (*serialize_script)(ecs_world_t* world, ecs_entity_t script);
    ecs_entity_t (*deserialize_script)(ecs_world_t* world, sandbox_properties_handle_t props);
    void (*register_script)(ecs_world_t* world, sandbox_properties_handle_t props);
    bool (*has_script)(ecs_world_t* world, const char* name);
    bool (*is_script)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*find_script)(ecs_world_t* world, const char* name);
    void (*include_code)(ecs_world_t* world, const char* path);
    void (*execute_script)(ecs_world_t* world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count);
} spectre_scripts_api_t;

SANDBOX_DECLARE_SERVICE(spectre_scripts_service_t, spectre_scripts_api_t, {
    .struct_size = 0,
    .name = "scripts",
    .description = "Scripts Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

#ifdef __cplusplus
}
#endif
