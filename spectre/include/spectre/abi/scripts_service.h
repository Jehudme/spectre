#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/handle.h>
#include <spectre/abi/scripts_components.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ecs_world_t ecs_world_t;
typedef uint64_t ecs_entity_t;

typedef struct spectre_scripts_api_t {
    ecs_entity_t (*create_script_entity)(ecs_world_t* ecs, const char* function_name, spectre_script_trigger_t trigger);
    void (*load_script)(ecs_world_t* ecs, const char* path);
    void (*execute_script)(ecs_world_t* ecs, const char* function_name, ecs_entity_t target_entity);
} spectre_scripts_api_t;

SANDBOX_DECLARE_SERVICE(spectre_scripts_service_t, spectre_scripts_api_t, {
    .name = "scripts",
    .description = "Scripts Service API",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0
})

#ifdef __cplusplus
}
#endif
