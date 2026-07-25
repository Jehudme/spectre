#pragma once
#include "spectre/spectre.h" // for custom types
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_renderer_api_t {
    void (*deserialize_renderer)(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize_renderer)(ecs_world_t* world, ecs_entity_t renderer);
    void (*register_renderer)(ecs_world_t* world, sandbox_properties_handle_t props);
    bool (*is_renderer)(ecs_world_t* world);
    void (*import_configuration)(ecs_world_t* world, const char* path);
    void (*export_configuration)(ecs_world_t* world, const char* path);
} spectre_renderer_api_t;

SANDBOX_DECLARE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t,
                        {.struct_size = 0,
                         .name = "renderer",
                         .description = "Renderer Service",
                         .architecture = "spectre",
                         .version_major = 1,
                         .version_minor = 0,
                         .init_fn = NULL});

// --- Public C API ---
SANDBOX_API
void spectre_renderer_deserialize_renderer(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props);
SANDBOX_API
sandbox_properties_handle_t spectre_renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer);
SANDBOX_API
void spectre_renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
SANDBOX_API
bool spectre_renderer_is_renderer(ecs_world_t* world);
SANDBOX_API
void spectre_renderer_import_configuration(ecs_world_t* world, const char* path);
SANDBOX_API
void spectre_renderer_export_configuration(ecs_world_t* world, const char* path);

#ifdef __cplusplus
}
#endif
