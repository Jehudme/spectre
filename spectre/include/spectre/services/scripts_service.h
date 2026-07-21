#pragma once
#include "spectre/spectre.h" // for custom types
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_scripts_api_t {
    bool (*has_script)(
        ecs_world_t *world, const char *function_name, const spectre_script_argument_type_t *arg_types, size_t arg_count);
    bool (*is_script)(ecs_world_t *world, ecs_entity_t entity);
    ecs_entity_t (*find_script)(ecs_world_t *world, const char *function_name);
    void (*include_code)(ecs_world_t *world, const char *path);
    void (*execute_script)(ecs_world_t *world, const char *function_name, spectre_script_argument_t *args, size_t arg_count);
    sandbox_properties_handle_t (*serialize_scripts)(ecs_world_t *world, ecs_entity_t entity);
    void (*deserialize_scripts)(ecs_world_t *world, ecs_entity_t target, sandbox_properties_handle_t props);
    void (*execute_on_create)(ecs_world_t *world, ecs_entity_t entity);
    void (*execute_on_destroy)(ecs_world_t *world, ecs_entity_t entity);
    void (*execute_on_update)(ecs_world_t *world, ecs_entity_t entity);
    void (*execute_on_enter)(ecs_world_t *world, ecs_entity_t entity);
    void (*execute_on_exit)(ecs_world_t *world, ecs_entity_t entity);
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

// --- Public C API ---
SANDBOX_API
bool spectre_scripts_has_script(
    ecs_world_t *world, const char *function_name, const spectre_script_argument_type_t *arg_types, size_t arg_count);
SANDBOX_API
bool spectre_scripts_is_script(ecs_world_t *world, ecs_entity_t entity);
SANDBOX_API
ecs_entity_t spectre_scripts_find_script(ecs_world_t *world, const char *function_name);
SANDBOX_API
void spectre_scripts_include_code(ecs_world_t *world, const char *path);
SANDBOX_API
void spectre_scripts_execute_script(
    ecs_world_t *world, const char *function_name, spectre_script_argument_t *args, size_t arg_count);
SANDBOX_API
sandbox_properties_handle_t spectre_scripts_serialize_scripts(ecs_world_t *world, ecs_entity_t entity);
SANDBOX_API
void spectre_scripts_deserialize_scripts(ecs_world_t *world, ecs_entity_t target, sandbox_properties_handle_t props);
SANDBOX_API
void spectre_scripts_execute_on_create(ecs_world_t *world, ecs_entity_t entity);
SANDBOX_API
void spectre_scripts_execute_on_destroy(ecs_world_t *world, ecs_entity_t entity);
SANDBOX_API
void spectre_scripts_execute_on_update(ecs_world_t *world, ecs_entity_t entity);
SANDBOX_API
void spectre_scripts_execute_on_enter(ecs_world_t *world, ecs_entity_t entity);
SANDBOX_API
void spectre_scripts_execute_on_exit(ecs_world_t *world, ecs_entity_t entity);

#ifdef __cplusplus
}
#endif
