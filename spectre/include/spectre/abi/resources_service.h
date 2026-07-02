#pragma once

#include <stdint.h>
#include <stddef.h>
#include <sandbox/abi/bootstrapper.h>

#ifdef __cplusplus
extern "C" {
#endif

// Represents the binary content of a resource file
typedef struct spectre_resource_content_t {
    uint8_t* data;
    size_t size;
} spectre_resource_content_t;

// Function pointers for resource loading and freeing
typedef void* (*spectre_load_resource_fn)(spectre_resource_content_t* content);
typedef void (*spectre_free_resource_fn)(void* instance);

typedef struct spectre_resources_api_t {
    void (*create_resource)(ecs_world_t* ecs, const char* type, spectre_load_resource_fn load_fn, spectre_free_resource_fn free_fn);
    void (*create_resource_instance)(ecs_world_t* ecs, const char* type, const char* path);

    uint64_t (*handle)(ecs_world_t* ecs, const char* path);

    void* (*load)(ecs_world_t* ecs, uint64_t handle);
    void (*free_instance)(ecs_world_t* ecs, uint64_t handle); // named free_instance because free is a keyword
} spectre_resources_api_t;

SANDBOX_DECLARE_SERVICE(spectre_resources_service_t, spectre_resources_api_t, {
    .name = "resources",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0
})

#ifdef __cplusplus
}
#endif
