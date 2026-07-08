#include "spectre/services/renderer_service.h"
#include "renderer_module.h"

static ecs_entity_t                renderer_deserialize_renderer(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static sandbox_properties_handle_t renderer_serialize_renderer(ecs_world_t* entity_world, ecs_entity_t renderer);
static void                         renderer_register_renderer(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static bool                         renderer_is_renderer(ecs_world_t* entity_world);

spectre_renderer_api_t g_renderer_api = {
    .deserialize_renderer = renderer_deserialize_renderer,
    .serialize_renderer   = renderer_serialize_renderer,
    .register_renderer    = renderer_register_renderer,
    .is_renderer          = renderer_is_renderer,
};

SANDBOX_DEFINE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t, &g_renderer_api)

static ecs_entity_t renderer_deserialize_renderer(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::renderer_module_t>();
    if (module) return module->deserialize_renderer(sandbox::properties(props)).id();
    return 0;
}

static sandbox_properties_handle_t renderer_serialize_renderer(ecs_world_t* entity_world, ecs_entity_t renderer) {
    if (!entity_world) return {0};
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::renderer_module_t>();
    if (module) return module->serialize_renderer(flecs_world.entity(renderer)).get_raw();
    return {0};
}

static void renderer_register_renderer(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::renderer_module_t>();
    if (module) module->register_renderer(sandbox::properties(props));
}

static bool renderer_is_renderer(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::renderer_module_t>();
    if (module) return module->is_renderer();
    return false;
}
