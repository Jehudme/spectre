#include "spectre/services/scenes_service.h"
#include "scenes_module.h"

static sandbox_properties_handle_t scenes_serialize_state(ecs_world_t* world, ecs_entity_t state);
static ecs_entity_t                scenes_deserialize_state(ecs_world_t* world, sandbox_properties_handle_t props);
static sandbox_properties_handle_t scenes_serialize_scene(ecs_world_t* world, ecs_entity_t scene);
static ecs_entity_t                scenes_deserialize_scene(ecs_world_t* world, sandbox_properties_handle_t props);
static void                         scenes_register_state(ecs_world_t* world, sandbox_properties_handle_t props);
static void                         scenes_register_scene(ecs_world_t* world, sandbox_properties_handle_t props);
static ecs_entity_t                scenes_find_state(ecs_world_t* world, const char* name);
static ecs_entity_t                scenes_find_scene(ecs_world_t* world, const char* name);
static bool                         scenes_has_state(ecs_world_t* world, const char* name);
static bool                         scenes_has_scene(ecs_world_t* world, const char* name);
static bool                         scenes_is_state(ecs_world_t* world, ecs_entity_t entity);
static bool                         scenes_is_scene(ecs_world_t* world, ecs_entity_t entity);
static ecs_entity_t                scenes_find_current_state(ecs_world_t* world);
static ecs_query_t*                scenes_find_current_scenes(ecs_world_t* world);
static void                         scenes_push_state(ecs_world_t* world, ecs_entity_t state);
static void                         scenes_pop_state(ecs_world_t* world);

spectre_scenes_api_t g_scenes_api = {
    .serialize_state     = scenes_serialize_state,
    .deserialize_state   = scenes_deserialize_state,
    .serialize_scene     = scenes_serialize_scene,
    .deserialize_scene   = scenes_deserialize_scene,
    .register_state      = scenes_register_state,
    .register_scene      = scenes_register_scene,
    .find_state          = scenes_find_state,
    .find_scene          = scenes_find_scene,
    .has_state           = scenes_has_state,
    .has_scene           = scenes_has_scene,
    .is_state            = scenes_is_state,
    .is_scene            = scenes_is_scene,
    .find_current_state  = scenes_find_current_state,
    .find_current_scenes = scenes_find_current_scenes,
    .push_state          = scenes_push_state,
    .pop_state           = scenes_pop_state,
};

SANDBOX_DEFINE_SERVICE(spectre_scenes_service_t, spectre_scenes_api_t, &g_scenes_api)

static sandbox_properties_handle_t scenes_serialize_state(ecs_world_t* world, ecs_entity_t state) {
    if (!world) return {0};
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->serialize_state(w.entity(state)).get_raw();
    return {0};
}

static ecs_entity_t scenes_deserialize_state(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->deserialize_state(sandbox::properties(props)).id();
    return 0;
}

static sandbox_properties_handle_t scenes_serialize_scene(ecs_world_t* world, ecs_entity_t scene) {
    if (!world) return {0};
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->serialize_scene(w.entity(scene)).get_raw();
    return {0};
}

static ecs_entity_t scenes_deserialize_scene(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->deserialize_scene(sandbox::properties(props)).id();
    return 0;
}

static void scenes_register_state(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) m->register_state(sandbox::properties(props));
}

static void scenes_register_scene(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) m->register_scene(sandbox::properties(props));
}

static ecs_entity_t scenes_find_state(ecs_world_t* world, const char* name) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->find_state(name).id();
    return 0;
}

static ecs_entity_t scenes_find_scene(ecs_world_t* world, const char* name) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->find_scene(name).id();
    return 0;
}

static bool scenes_has_state(ecs_world_t* world, const char* name) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->has_state(name);
    return false;
}

static bool scenes_has_scene(ecs_world_t* world, const char* name) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->has_scene(name);
    return false;
}

static bool scenes_is_state(ecs_world_t* world, ecs_entity_t entity) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->is_state(w.entity(entity));
    return false;
}

static bool scenes_is_scene(ecs_world_t* world, ecs_entity_t entity) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->is_scene(w.entity(entity));
    return false;
}

static ecs_entity_t scenes_find_current_state(ecs_world_t* world) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) return m->find_current_state().id();
    return 0;
}

static ecs_query_t* scenes_find_current_scenes(ecs_world_t* world) {
    // TODO: map flecs::query<> to ecs_query_t*
    return nullptr;
}

static void scenes_push_state(ecs_world_t* world, ecs_entity_t state) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) m->push_state(w.entity(state));
}

static void scenes_pop_state(ecs_world_t* world) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::scenes_module_t>();
    if (m) m->pop_state();
}
