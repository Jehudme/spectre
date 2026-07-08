#include "spectre/services/serializer_service.h"
#include "serializer_module.h"

static void                         serializer_register_serializer(ecs_world_t* entity_world, const char* type, const spectre_serializer_t* serializer);
static bool                         serializer_has_serializer(ecs_world_t* entity_world, const char* type);
static ecs_entity_t                serializer_find_serializer(ecs_world_t* entity_world, const char* type);
static sandbox_properties_handle_t serializer_serialize_entity(ecs_world_t* entity_world, ecs_entity_t serializer, ecs_entity_t entity);
static ecs_entity_t                serializer_deserialize_entity(ecs_world_t* entity_world, ecs_entity_t serializer, sandbox_properties_handle_t props);

spectre_serializer_api_t g_serializer_api = {
    .register_serializer = serializer_register_serializer,
    .has_serializer      = serializer_has_serializer,
    .find_serializer     = serializer_find_serializer,
    .serialize_entity    = serializer_serialize_entity,
    .deserialize_entity  = serializer_deserialize_entity,
};

SANDBOX_DEFINE_SERVICE(spectre_serializer_service_t, spectre_serializer_api_t, &g_serializer_api)

static void serializer_register_serializer(ecs_world_t* entity_world, const char* type, const spectre_serializer_t* serializer) {
    if (!entity_world || !serializer) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::serializer_module>();
    if (module) module->register_serializer(type, *serializer);
}

static bool serializer_has_serializer(ecs_world_t* entity_world, const char* type) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::serializer_module>();
    if (module) return module->has_serializer(type);
    return false;
}

static ecs_entity_t serializer_find_serializer(ecs_world_t* entity_world, const char* type) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::serializer_module>();
    if (module) return module->find_serializer(type).id();
    return 0;
}

static sandbox_properties_handle_t serializer_serialize_entity(ecs_world_t* entity_world, ecs_entity_t serializer, ecs_entity_t entity) {
    if (!entity_world) return {0};
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::serializer_module>();
    if (module) return module->serialize_entity(flecs_world.entity(serializer), flecs_world.entity(entity)).get_raw();
    return {0};
}

static ecs_entity_t serializer_deserialize_entity(ecs_world_t* entity_world, ecs_entity_t serializer, sandbox_properties_handle_t props) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::serializer_module>();
    if (module) return module->deserialize_entity(flecs_world.entity(serializer), sandbox::properties(props)).id();
    return 0;
}
