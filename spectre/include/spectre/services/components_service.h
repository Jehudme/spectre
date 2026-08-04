#pragma once
#include "spectre/components/serializer_component.h"
#include "spectre/spectre.h" // for custom types
#include <sandbox/abi/bootstrapper.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef ecs_entity_t (*spectre_component_registration_fn_t)(ecs_world_t* world);

typedef enum spectre_dynamic_type_t {
    SPECTRE_DYNAMIC_TYPE_UNKNOWN,
    SPECTRE_DYNAMIC_TYPE_INT,
    SPECTRE_DYNAMIC_TYPE_FLOAT,
    SPECTRE_DYNAMIC_TYPE_DOUBLE,
    SPECTRE_DYNAMIC_TYPE_BOOL,
    SPECTRE_DYNAMIC_TYPE_STRING,
    SPECTRE_DYNAMIC_TYPE_ARRAY_INT,
    SPECTRE_DYNAMIC_TYPE_ARRAY_FLOAT,
    SPECTRE_DYNAMIC_TYPE_ARRAY_DOUBLE,
    SPECTRE_DYNAMIC_TYPE_ARRAY_STRING
} spectre_dynamic_type_t;

typedef struct spectre_components_api_t {
    void (*register_component)(ecs_world_t* world, const char* name, spectre_component_registration_fn_t registration_fn,
                               spectre_serializer_component serializer);
    bool (*is_static)(ecs_world_t* world, const char* name);
    sandbox_properties_handle_t (*find_schema)(ecs_world_t* world, const char* name);
    ecs_entity_t (*find_component)(ecs_world_t* world, const char* name);
    bool (*has_component)(ecs_world_t* world, const char* name);
    bool (*is_component)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t* (*list_components)(ecs_world_t* world, size_t* count);
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

#ifndef SPECTRE_API
#ifdef _WIN32
#define SPECTRE_API __declspec(dllexport)
#else
#define SPECTRE_API __attribute__((visibility("default")))
#endif
#endif

// --- Public C API ---
SPECTRE_API
SPECTRE_API void spectre_components_register_component(ecs_world_t* world, const char* name,
                                           spectre_component_registration_fn_t registration_fn,
                                           spectre_serializer_component serializer);

SPECTRE_API
SPECTRE_API bool spectre_components_is_static(ecs_world_t* world, const char* name);

SPECTRE_API
SPECTRE_API sandbox_properties_handle_t spectre_components_find_schema(ecs_world_t* world, const char* name);

SPECTRE_API
SPECTRE_API ecs_entity_t spectre_components_find_component(ecs_world_t* world, const char* name);

SPECTRE_API
SPECTRE_API bool spectre_components_has_component(ecs_world_t* world, const char* name);

SPECTRE_API
SPECTRE_API bool spectre_components_is_component(ecs_world_t* world, ecs_entity_t entity);

SPECTRE_API
SPECTRE_API ecs_entity_t* spectre_components_list_components(ecs_world_t* world, size_t* count);

SPECTRE_API
SPECTRE_API void spectre_components_register_dynamic_component(ecs_world_t* world, const char* name, sandbox_properties_handle_t properties);

SPECTRE_API
SPECTRE_API void spectre_components_import_configuration(ecs_world_t* world, const char* directory_path);

SPECTRE_API
SPECTRE_API void spectre_components_export_configuration(ecs_world_t* world, const char* directory_path);


#ifdef __cplusplus
}
#endif
