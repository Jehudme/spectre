#pragma once
#include "spectre/components/serializer_component.h"
#include "spectre/spectre.h" // for custom types
#include <sandbox/abi/bootstrapper.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef ecs_entity_t (*spectre_component_registration_fn_t)(ecs_world_t* world);

typedef struct spectre_components_api_t {
    void (*register_component)(ecs_world_t* world, const char* name, spectre_component_registration_fn_t registration_fn,
                               spectre_serializer_component serializer);
    ecs_entity_t (*find_component)(ecs_world_t* world, const char* name);
    bool (*has_component)(ecs_world_t* world, const char* name);
    bool (*is_component)(ecs_world_t* world, ecs_entity_t entity);
    void (*register_dynamic_component)(ecs_world_t* world, const char* name, sandbox_properties_handle_t properties);
    void (*import_configuration)(ecs_world_t* world, const char* directory_path);
    void (*export_configuration)(ecs_world_t* world, const char* directory_path);
} spectre_components_api_t;

SANDBOX_DECLARE_SERVICE(spectre_components_service_t, spectre_components_api_t,
                        {.struct_size = 0,
                         .name = "components",
                         .description = "Components Service",
                         .architecture = "spectre",
                         .version_major = 1,
                         .version_minor = 0,
                         .init_fn = NULL});

// --- Public C API ---
SANDBOX_API
void spectre_components_register_component(ecs_world_t* world, const char* name,
                                           spectre_component_registration_fn_t registration_fn,
                                           spectre_serializer_component serializer);

SANDBOX_API
ecs_entity_t spectre_components_find_component(ecs_world_t* world, const char* name);

SANDBOX_API
bool spectre_components_has_component(ecs_world_t* world, const char* name);

SANDBOX_API
bool spectre_components_is_component(ecs_world_t* world, ecs_entity_t entity);

SANDBOX_API
void spectre_components_register_dynamic_component(ecs_world_t* world, const char* name, sandbox_properties_handle_t properties);

SANDBOX_API
void spectre_components_import_configuration(ecs_world_t* world, const char* directory_path);

SANDBOX_API
void spectre_components_export_configuration(ecs_world_t* world, const char* directory_path);


#ifdef __cplusplus
}
#endif
