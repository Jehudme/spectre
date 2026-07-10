#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/spectre.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif



typedef struct spectre_renderer_api_t {
    ecs_entity_t (*deserialize_renderer)(ecs_world_t* world, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize_renderer)(ecs_world_t* world, ecs_entity_t renderer);
    void (*register_renderer)(ecs_world_t* world, sandbox_properties_handle_t props);
    bool (*is_renderer)(ecs_world_t* world);
} spectre_renderer_api_t;

SANDBOX_DECLARE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t, {
    .struct_size = 0,
    .name = "renderer",
    .description = "Renderer Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

// --- Public C API ---
SANDBOX_API ecs_entity_t spectre_renderer_deserialize_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
SANDBOX_API sandbox_properties_handle_t spectre_renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer);
SANDBOX_API void spectre_renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
SANDBOX_API bool spectre_renderer_is_renderer(ecs_world_t* world);

#ifdef __cplusplus
}
#endif
