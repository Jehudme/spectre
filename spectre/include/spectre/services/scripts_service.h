#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif


// --- Public C API ---
SANDBOX_API sandbox_properties_handle_t spectre_scripts_serialize_script(ecs_world_t* entity_world, ecs_entity_t script);
SANDBOX_API ecs_entity_t spectre_scripts_deserialize_script(ecs_world_t* entity_world, sandbox_properties_handle_t props);
SANDBOX_API void spectre_scripts_register_script(ecs_world_t* entity_world, sandbox_properties_handle_t props);
SANDBOX_API bool spectre_scripts_has_script(ecs_world_t* entity_world, const char* name);
SANDBOX_API bool spectre_scripts_is_script(ecs_world_t* entity_world, ecs_entity_t entity);
SANDBOX_API ecs_entity_t spectre_scripts_find_script(ecs_world_t* entity_world, const char* name);
SANDBOX_API void spectre_scripts_include_code(ecs_world_t* entity_world, const char* path);
SANDBOX_API void spectre_scripts_execute_script(ecs_world_t* entity_world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count);

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
