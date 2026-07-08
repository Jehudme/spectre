#include "spectre/services/prefabs_service.h"
#include "prefabs_module.h"

static sandbox_properties_handle_t prefabs_serialize_entity(ecs_world_t* world, ecs_entity_t entity);
static ecs_entity_t                prefabs_deserialize_entity(ecs_world_t* world, sandbox_properties_handle_t props);
static void                         prefabs_register_prefab(ecs_world_t* world, sandbox_properties_handle_t props);
static bool                         prefabs_has_prefab(ecs_world_t* world, const char* name);
static bool                         prefabs_is_prefab(ecs_world_t* world, ecs_entity_t entity);
static ecs_entity_t                prefabs_find_prefab(ecs_world_t* world, const char* name);
static ecs_entity_t                prefabs_create_entity_from_props(ecs_world_t* world, sandbox_properties_handle_t props);
static ecs_entity_t                prefabs_create_entity_from_prefab(ecs_world_t* world, ecs_entity_t prefab);
static ecs_entity_t                prefabs_create_entity_from_name(ecs_world_t* world, const char* name);

spectre_prefabs_api_t g_prefabs_api = {
    .serialize_entity         = prefabs_serialize_entity,
    .deserialize_entity       = prefabs_deserialize_entity,
    .register_prefab          = prefabs_register_prefab,
    .has_prefab               = prefabs_has_prefab,
    .is_prefab                = prefabs_is_prefab,
    .find_prefab              = prefabs_find_prefab,
    .create_entity_from_props  = prefabs_create_entity_from_props,
    .create_entity_from_prefab = prefabs_create_entity_from_prefab,
    .create_entity_from_name   = prefabs_create_entity_from_name,
};

SANDBOX_DEFINE_SERVICE(spectre_prefabs_service_t, spectre_prefabs_api_t, &g_prefabs_api)

static sandbox_properties_handle_t prefabs_serialize_entity(ecs_world_t* world, ecs_entity_t entity) {
    if (!world) return {0};
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::components_module_t>();
    if (m) return m->serialize_entity(w.entity(entity)).get_raw();
    return {0};
}

static ecs_entity_t prefabs_deserialize_entity(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::components_module_t>();
    if (m) return m->deserialize_entity(sandbox::properties(props)).id();
    return 0;
}

static void prefabs_register_prefab(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::components_module_t>();
    if (m) m->register_prefab(sandbox::properties(props));
}

static bool prefabs_has_prefab(ecs_world_t* world, const char* name) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::components_module_t>();
    if (m) return m->has_prefab(name);
    return false;
}

static bool prefabs_is_prefab(ecs_world_t* world, ecs_entity_t entity) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::components_module_t>();
    if (m) return m->is_prefab(w.entity(entity));
    return false;
}

static ecs_entity_t prefabs_find_prefab(ecs_world_t* world, const char* name) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::components_module_t>();
    if (m) return m->find_prefab(name).id();
    return 0;
}

static ecs_entity_t prefabs_create_entity_from_props(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::components_module_t>();
    if (m) return m->create_entity(sandbox::properties(props)).id();
    return 0;
}

static ecs_entity_t prefabs_create_entity_from_prefab(ecs_world_t* world, ecs_entity_t prefab) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::components_module_t>();
    if (m) return m->create_entity(w.entity(prefab)).id();
    return 0;
}

static ecs_entity_t prefabs_create_entity_from_name(ecs_world_t* world, const char* name) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::components_module_t>();
    if (m) return m->create_entity(std::string_view(name)).id();
    return 0;
}
