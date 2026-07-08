#include "spectre/services/resources_service.h"
#include "resources_module.h"

static ecs_entity_t                resources_deserialize_resource(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static sandbox_properties_handle_t resources_serialize_resource(ecs_world_t* entity_world, ecs_entity_t resourceEntity);
static void                         resources_register_resource_loader(ecs_world_t* entity_world, const char* type, spectre_resource_loader_t loader);
static void                         resources_register_resource(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static bool                         resources_has_resource_loader(ecs_world_t* entity_world, const char* type);
static bool                         resources_has_resource(ecs_world_t* entity_world, const char* name);
static bool                         resources_is_resource(ecs_world_t* entity_world, ecs_entity_t entity);
static ecs_entity_t                resources_find_resource_loader(ecs_world_t* entity_world, const char* type);
static ecs_entity_t                resources_find_resource(ecs_world_t* entity_world, const char* name);
static bool                         resources_is_resource_loaded(ecs_world_t* entity_world, ecs_entity_t resource);
static void                         resources_load_resource(ecs_world_t* entity_world, ecs_entity_t resourceEntity);
static void                         resources_free_resource(ecs_world_t* entity_world, ecs_entity_t resourceEntity);

spectre_resources_api_t g_resources_api = {
    .deserialize_resource    = resources_deserialize_resource,
    .serialize_resource      = resources_serialize_resource,
    .register_resource_loader = resources_register_resource_loader,
    .register_resource       = resources_register_resource,
    .has_resource_loader     = resources_has_resource_loader,
    .has_resource            = resources_has_resource,
    .is_resource             = resources_is_resource,
    .find_resource_loader    = resources_find_resource_loader,
    .find_resource           = resources_find_resource,
    .is_resource_loaded      = resources_is_resource_loaded,
    .load_resource           = resources_load_resource,
    .free_resource           = resources_free_resource,
};

SANDBOX_DEFINE_SERVICE(spectre_resources_service_t, spectre_resources_api_t, &g_resources_api)

static ecs_entity_t resources_deserialize_resource(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) return module->deserialize_resource(sandbox::properties(props)).id();
    return 0;
}

static sandbox_properties_handle_t resources_serialize_resource(ecs_world_t* entity_world, ecs_entity_t resourceEntity) {
    if (!entity_world) return {0};
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) return module->serialize_resource(flecs_world.entity(resourceEntity)).get_raw();
    return {0};
}

static void resources_register_resource_loader(ecs_world_t* entity_world, const char* type, spectre_resource_loader_t loader) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) module->register_resource_loader(type, loader);
}

static void resources_register_resource(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) module->register_resource(sandbox::properties(props));
}

static bool resources_has_resource_loader(ecs_world_t* entity_world, const char* type) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) return module->has_resource_loader(type);
    return false;
}

static bool resources_has_resource(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) return module->has_resource(name);
    return false;
}

static bool resources_is_resource(ecs_world_t* entity_world, ecs_entity_t entity) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) return module->is_resource(flecs_world.entity(entity));
    return false;
}

static ecs_entity_t resources_find_resource_loader(ecs_world_t* entity_world, const char* type) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) return module->find_resource_loader(type).id();
    return 0;
}

static ecs_entity_t resources_find_resource(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) return module->find_resource(name).id();
    return 0;
}

static bool resources_is_resource_loaded(ecs_world_t* entity_world, ecs_entity_t resource) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) return module->is_resource_loaded(flecs_world.entity(resource));
    return false;
}

static void resources_load_resource(ecs_world_t* entity_world, ecs_entity_t resourceEntity) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) module->load_resource(flecs_world.entity(resourceEntity));
}

static void resources_free_resource(ecs_world_t* entity_world, ecs_entity_t resourceEntity) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::resource_module_t>();
    if (module) module->free_resource(flecs_world.entity(resourceEntity));
}
