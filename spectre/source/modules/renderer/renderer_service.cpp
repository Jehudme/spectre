#include "spectre/services/renderer_service.h"
#include "renderer_module.h"

static ecs_entity_t                renderer_deserialize_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
static sandbox_properties_handle_t renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer);
static void                         renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props);
static bool                         renderer_is_renderer(ecs_world_t* world);

spectre_renderer_api_t g_renderer_api = {
    .deserialize_renderer = renderer_deserialize_renderer,
    .serialize_renderer   = renderer_serialize_renderer,
    .register_renderer    = renderer_register_renderer,
    .is_renderer          = renderer_is_renderer,
};

SANDBOX_DEFINE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t, &g_renderer_api)

static ecs_entity_t renderer_deserialize_renderer(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::renderer_module_t>();
    if (m) return m->deserialize_renderer(sandbox::properties(props)).id();
    return 0;
}

static sandbox_properties_handle_t renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer) {
    if (!world) return {0};
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::renderer_module_t>();
    if (m) return m->serialize_renderer(w.entity(renderer)).get_raw();
    return {0};
}

static void renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::renderer_module_t>();
    if (m) m->register_renderer(sandbox::properties(props));
}

static bool renderer_is_renderer(ecs_world_t* world) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::renderer_module_t>();
    if (m) return m->is_renderer();
    return false;
}
