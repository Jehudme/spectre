#pragma once

#ifndef SPECTRE_API
#if defined(_WIN32)
#define SPECTRE_API __declspec(dllexport)
#else
#define SPECTRE_API __attribute__((visibility("default")))
#endif
#endif

#include "spectre/spectre.h" // for custom types
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_resources_api_t {
    void (*deserialize_resource)(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
    void (*register_resource_loader)(ecs_world_t* world, const char* type, spectre_resource_loader_component_t loader);
    void (*register_resource)(ecs_world_t* world, sandbox_properties_handle_t props);
    bool (*has_resource_loader)(ecs_world_t* world, const char* type);
    bool (*has_resource)(ecs_world_t* world, const char* name);
    bool (*is_resource)(ecs_world_t* world, ecs_entity_t entity);
    ecs_entity_t (*find_resource_loader)(ecs_world_t* world, const char* type);
    ecs_entity_t* (*list_resources)(ecs_world_t* world, size_t* count);
    ecs_entity_t* (*list_resource_loaders)(ecs_world_t* world, size_t* count);
    ecs_entity_t (*find_resource)(ecs_world_t* world, const char* name);
    bool (*is_resource_loaded)(ecs_world_t* world, ecs_entity_t resource);
    void (*load_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
    void (*free_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
    void* (*get_resource)(ecs_world_t* world, ecs_entity_t resourceEntity);
    void (*import_configuration)(ecs_world_t* world, const char* path);
    void (*export_configuration)(ecs_world_t* world, const char* path);
} spectre_resources_api_t;

SANDBOX_DECLARE_SERVICE(spectre_resources_service_t, spectre_resources_api_t,
                        {.struct_size = 0,
                         .name = "resources",
                         .description = "Resources Service",
                         .architecture = "spectre",
                         .version_major = 1,
                         .version_minor = 0,
                         .init_fn = NULL});

// --- Public C API ---
SANDBOX_API
SPECTRE_API void spectre_resources_deserialize_resource(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
SANDBOX_API
SPECTRE_API sandbox_properties_handle_t spectre_resources_serialize_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
SANDBOX_API
SPECTRE_API void spectre_resources_register_resource_loader(ecs_world_t* world, const char* type,
                                                spectre_resource_loader_component_t loader);
SANDBOX_API
SPECTRE_API void spectre_resources_register_resource(ecs_world_t* world, sandbox_properties_handle_t props);
SANDBOX_API
SPECTRE_API bool spectre_resources_has_resource_loader(ecs_world_t* world, const char* type);
SANDBOX_API
SPECTRE_API bool spectre_resources_has_resource(ecs_world_t* world, const char* name);
SANDBOX_API
SPECTRE_API bool spectre_resources_is_resource(ecs_world_t* world, ecs_entity_t entity);
SANDBOX_API
SANDBOX_API
SPECTRE_API ecs_entity_t* spectre_resources_list_resources(ecs_world_t* world, size_t* count);
SANDBOX_API
SPECTRE_API ecs_entity_t* spectre_resources_list_resource_loaders(ecs_world_t* world, size_t* count);

SPECTRE_API ecs_entity_t spectre_resources_find_resource_loader(ecs_world_t* world, const char* type);
SANDBOX_API
SANDBOX_API
SPECTRE_API ecs_entity_t* spectre_resources_list_resources(ecs_world_t* world, size_t* count);
SANDBOX_API
SPECTRE_API ecs_entity_t* spectre_resources_list_resource_loaders(ecs_world_t* world, size_t* count);

SPECTRE_API ecs_entity_t spectre_resources_find_resource(ecs_world_t* world, const char* name);
SANDBOX_API
SPECTRE_API bool spectre_resources_is_resource_loaded(ecs_world_t* world, ecs_entity_t resource);
SANDBOX_API
void* spectre_resources_get_resource(ecs_world_t* world, ecs_entity_t resourceEntity);
SANDBOX_API
SPECTRE_API void spectre_resources_import_configuration(ecs_world_t* world, const char* path);
SANDBOX_API
SPECTRE_API void spectre_resources_export_configuration(ecs_world_t* world, const char* path);

#ifdef __cplusplus
}
#endif
