#include <spectre/sdk/renderer.hpp>
#include <sandbox/sdk/logs.hpp>
#include "spectre/services/renderer_service.h"
#include "renderer_module.h"

static void                renderer_deserialize_renderer(ecs_world_t* entity_world, ecs_entity_t target, sandbox_properties_handle_t props);
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

static void renderer_deserialize_renderer(ecs_world_t* entity_world, ecs_entity_t target, sandbox_properties_handle_t props) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.lookup("spectre::modules::renderer_module_t").is_valid() ? flecs_world.try_get_mut<spectre::modules::renderer_module_t>() : nullptr;
    if (module) module->deserialize_renderer(flecs_world.entity(target), sandbox::properties(props, false));
}

static sandbox_properties_handle_t renderer_serialize_renderer(ecs_world_t* entity_world, ecs_entity_t renderer) {
    if (!entity_world) return {0};
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.lookup("spectre::modules::renderer_module_t").is_valid() ? flecs_world.try_get_mut<spectre::modules::renderer_module_t>() : nullptr;
    if (module) {
        sandbox::properties props = module->serialize_renderer(flecs_world.entity(renderer));
        sandbox_properties_handle_t raw = props.get_raw();
        props.release();
        return raw;
    }
    return {0};
}

static void renderer_register_renderer(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.lookup("spectre::modules::renderer_module_t").is_valid() ? flecs_world.try_get_mut<spectre::modules::renderer_module_t>() : nullptr;
    if (module) module->register_renderer(sandbox::properties(props, false));
}

static bool renderer_is_renderer(ecs_world_t* entity_world) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.lookup("spectre::modules::renderer_module_t").is_valid() ? flecs_world.try_get_mut<spectre::modules::renderer_module_t>() : nullptr;
    if (module) return module->is_renderer();
    return false;
}

// --- Public C API Implementations ---
void spectre_renderer_deserialize_renderer(ecs_world_t* world, ecs_entity_t target, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
    const spectre_renderer_service_t* service = (const spectre_renderer_service_t*)ecs_singleton_get(world, spectre_renderer_service_t);
#endif
    if (service && service->api && service->api->deserialize_renderer) {
        service->api->deserialize_renderer(world, target, props);
        return;
    } else {
        sandbox::modules::logs::error(flecs_world, "[Renderer Module] Service not initialized!");
    }
    renderer_deserialize_renderer(world, target, props);
}

sandbox_properties_handle_t spectre_renderer_serialize_renderer(ecs_world_t* world, ecs_entity_t renderer) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
    const spectre_renderer_service_t* service = (const spectre_renderer_service_t*)ecs_singleton_get(world, spectre_renderer_service_t);
#endif
    if (service && service->api && service->api->serialize_renderer) {
        return service->api->serialize_renderer(world, renderer);
    } else {
        sandbox::modules::logs::error(flecs_world, "[Renderer Module] Service not initialized!");
    }
    return renderer_serialize_renderer(world, renderer);
}

void spectre_renderer_register_renderer(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
    const spectre_renderer_service_t* service = (const spectre_renderer_service_t*)ecs_singleton_get(world, spectre_renderer_service_t);
#endif
    if (service && service->api && service->api->register_renderer) {
        service->api->register_renderer(world, props);
        return;
    } else {
        sandbox::modules::logs::error(flecs_world, "[Renderer Module] Service not initialized!");
    }
    renderer_register_renderer(world, props);
}

bool spectre_renderer_is_renderer(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_renderer_service_t* service = flecs_world.try_get<spectre_renderer_service_t>();
#else
    const spectre_renderer_service_t* service = (const spectre_renderer_service_t*)ecs_singleton_get(world, spectre_renderer_service_t);
#endif
    if (service && service->api && service->api->is_renderer) {
        return service->api->is_renderer(world);
    } else {
        sandbox::modules::logs::error(flecs_world, "[Renderer Module] Service not initialized!");
    }
    return renderer_is_renderer(world);
}

// --- SDK Implementations ---
namespace spectre::modules {
void renderer::deserialize_renderer(const flecs::world& entity_world, ecs_entity_t target, sandbox_properties_handle_t props) {
            spectre_renderer_deserialize_renderer(entity_world.c_ptr(), target, props);
        }

sandbox_properties_handle_t renderer::serialize_renderer(const flecs::world& entity_world, ecs_entity_t renderer) {
            return spectre_renderer_serialize_renderer(entity_world.c_ptr(), renderer);
        }

void renderer::register_renderer(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_renderer_register_renderer(entity_world.c_ptr(), props);}

bool renderer::is_renderer(const flecs::world& entity_world) {
            return spectre_renderer_is_renderer(entity_world.c_ptr());
        }
}
