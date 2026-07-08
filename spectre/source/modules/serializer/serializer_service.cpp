#include "spectre/services/serializer_service.h"
#include "serializer_module.h"

static void                         serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_t* serializer);
static bool                         serializer_has_serializer(ecs_world_t* world, const char* type);
static ecs_entity_t                serializer_find_serializer(ecs_world_t* world, const char* type);
static sandbox_properties_handle_t serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);
static ecs_entity_t                serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, sandbox_properties_handle_t props);

spectre_serializer_api_t g_serializer_api = {
    .register_serializer = serializer_register_serializer,
    .has_serializer      = serializer_has_serializer,
    .find_serializer     = serializer_find_serializer,
    .serialize_entity    = serializer_serialize_entity,
    .deserialize_entity  = serializer_deserialize_entity,
};

SANDBOX_DEFINE_SERVICE(spectre_serializer_service_t, spectre_serializer_api_t, &g_serializer_api)

static void serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_t* serializer) {
    if (!world || !serializer) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::serializer_module>();
    if (m) m->register_serializer(type, *serializer);
}

static bool serializer_has_serializer(ecs_world_t* world, const char* type) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::serializer_module>();
    if (m) return m->has_serializer(type);
    return false;
}

static ecs_entity_t serializer_find_serializer(ecs_world_t* world, const char* type) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::serializer_module>();
    if (m) return m->find_serializer(type).id();
    return 0;
}

static sandbox_properties_handle_t serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity) {
    if (!world) return {0};
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::serializer_module>();
    if (m) return m->serialize_entity(w.entity(serializer), w.entity(entity)).get_raw();
    return {0};
}

static ecs_entity_t serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, sandbox_properties_handle_t props) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::serializer_module>();
    if (m) return m->deserialize_entity(w.entity(serializer), sandbox::properties(props)).id();
    return 0;
}
