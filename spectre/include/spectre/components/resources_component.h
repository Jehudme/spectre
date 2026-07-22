#pragma once

#include <flecs.h>
#include <sandbox/abi/properties.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_use_loader_relation_t {
    char dummy;
} spectre_resource_loader_relation;
typedef struct spectre_use_resource_relation_t {
    char dummy;
} spectre_use_resource_relation;
typedef struct spectre_resource_loaded_flag_t {
    char dummy;
} spectre_resource_flag_t;

typedef struct spectre_resource_component_t {
    const char* path;
    void* instance;
    sandbox_properties_handle_t properties_handle;
} spectre_resource_component_t;

typedef struct spectre_resource_loader_component_t {
    void (*load_fn)(ecs_world_t* world, spectre_resource_component_t* resource);
    void (*free_fn)(ecs_world_t* world, spectre_resource_component_t* resource);
} spectre_resource_loader_component_t;

#ifdef __cplusplus
}
#endif
