#include "spectre/services/serializer_service.h"
#include "serializer_module.h"
#include <sandbox/sdk/logs.hpp>
#include <spectre/sdk/serializer.hpp>

static void
serializer_register_serializer(ecs_world_t *entity_world, const char *type,
                               const spectre_serializer_component *serializer);
static bool serializer_has_serializer(ecs_world_t *entity_world,
                                      const char *type);
static bool serializer_is_serializer(ecs_world_t *entity_world,
                                     ecs_entity_t entity);
static ecs_entity_t serializer_find_serializer(ecs_world_t *entity_world,
                                               const char *type);
static sandbox_properties_handle_t
serializer_serialize_entity(ecs_world_t *entity_world, ecs_entity_t serializer,
                            ecs_entity_t entity);
static void serializer_deserialize_entity(ecs_world_t *entity_world,
                                          ecs_entity_t serializer,
                                          ecs_entity_t entity,
                                          sandbox_properties_handle_t props);

spectre_serializer_api_t g_serializer_api = {
    .register_serializer = serializer_register_serializer,
    .has_serializer = serializer_has_serializer,
    .is_serializer = serializer_is_serializer,
    .find_serializer = serializer_find_serializer,
    .serialize_entity = serializer_serialize_entity,
    .deserialize_entity = serializer_deserialize_entity,
};

SANDBOX_DEFINE_SERVICE(spectre_serializer_service_t, spectre_serializer_api_t,
                       &g_serializer_api)

static void
serializer_register_serializer(ecs_world_t *entity_world, const char *type,
                               const spectre_serializer_component *serializer) {
  if (!entity_world || !serializer)
    return;
  flecs::world flecs_world(entity_world);
  flecs::entity module_ent =
      flecs_world.lookup("spectre::modules::serializer_module");
  if (module_ent.is_valid()) {
    auto *module =
        flecs_world.lookup("spectre::modules::serializer_module").is_valid()
            ? flecs_world.try_get_mut<spectre::modules::serializer_module>()
            : nullptr;
    if (module)
      module->register_serializer(type, *serializer);
  }
}

static bool serializer_has_serializer(ecs_world_t *entity_world,
                                      const char *type) {
  if (!entity_world)
    return false;
  flecs::world flecs_world(entity_world);
  flecs::entity module_ent =
      flecs_world.lookup("spectre::modules::serializer_module");
  if (module_ent.is_valid()) {
    auto *module =
        flecs_world.lookup("spectre::modules::serializer_module").is_valid()
            ? flecs_world.try_get_mut<spectre::modules::serializer_module>()
            : nullptr;
    if (module)
      return module->has_serializer(type);
  }
  return false;
}

static bool serializer_is_serializer(ecs_world_t *entity_world,
                                     ecs_entity_t entity) {
  if (!entity_world)
    return false;
  flecs::world flecs_world(entity_world);
  flecs::entity module_ent =
      flecs_world.lookup("spectre::modules::serializer_module");
  if (module_ent.is_valid()) {
    auto *module =
        flecs_world.lookup("spectre::modules::serializer_module").is_valid()
            ? flecs_world.try_get_mut<spectre::modules::serializer_module>()
            : nullptr;
    if (module)
      return module->is_serializer(flecs_world.entity(entity));
  }
  return false;
}

static ecs_entity_t serializer_find_serializer(ecs_world_t *entity_world,
                                               const char *type) {
  if (!entity_world)
    return 0;
  flecs::world flecs_world(entity_world);
  flecs::entity module_ent =
      flecs_world.lookup("spectre::modules::serializer_module");
  if (module_ent.is_valid()) {
    auto *module =
        flecs_world.lookup("spectre::modules::serializer_module").is_valid()
            ? flecs_world.try_get_mut<spectre::modules::serializer_module>()
            : nullptr;
    if (module)
      return module->find_serializer(type).id();
  }
  return 0;
}

static sandbox_properties_handle_t
serializer_serialize_entity(ecs_world_t *entity_world, ecs_entity_t serializer,
                            ecs_entity_t entity) {
  if (!entity_world)
    return {0};
  flecs::world flecs_world(entity_world);
  flecs::entity module_ent =
      flecs_world.lookup("spectre::modules::serializer_module");
  if (module_ent.is_valid()) {
    auto *module =
        flecs_world.lookup("spectre::modules::serializer_module").is_valid()
            ? flecs_world.try_get_mut<spectre::modules::serializer_module>()
            : nullptr;
    if (module) {
      sandbox::properties props = module->serialize_entity(
          flecs_world.entity(serializer), flecs_world.entity(entity));
      sandbox_properties_handle_t raw = props.get_raw();
      props.release();
      return raw;
    }
  }
  return {0};
}

static void serializer_deserialize_entity(ecs_world_t *entity_world,
                                          ecs_entity_t serializer,
                                          ecs_entity_t target,
                                          sandbox_properties_handle_t props) {
  if (!entity_world)
    return;
  flecs::world flecs_world(entity_world);
  flecs::entity module_ent =
      flecs_world.lookup("spectre::modules::serializer_module");
  if (module_ent.is_valid()) {
    auto *module =
        flecs_world.lookup("spectre::modules::serializer_module").is_valid()
            ? flecs_world.try_get_mut<spectre::modules::serializer_module>()
            : nullptr;
    if (module)
      module->deserialize_entity(flecs_world.entity(serializer),
                                 flecs_world.entity(target),
                                 sandbox::properties(props, false));
  }
}

// --- Public C API Implementations ---
void spectre_serializer_register_serializer(
    ecs_world_t *world, const char *type,
    const spectre_serializer_component *serializer) {
#ifdef __cplusplus
  flecs::world flecs_world(world);
  const spectre_serializer_service_t *service =
      flecs_world.try_get<spectre_serializer_service_t>();
#else
  const spectre_serializer_service_t *service =
      (const spectre_serializer_service_t *)ecs_singleton_get(
          world, spectre_serializer_service_t);
#endif
  if (service && service->api && service->api->register_serializer) {
    service->api->register_serializer(world, type, serializer);
    return;
  } else {
    sandbox::modules::logs::error(
        flecs_world, "[Serializer Module] Service not initialized!");
  }
  serializer_register_serializer(world, type, serializer);
}

bool spectre_serializer_has_serializer(ecs_world_t *world, const char *type) {
#ifdef __cplusplus
  flecs::world flecs_world(world);
  const spectre_serializer_service_t *service =
      flecs_world.try_get<spectre_serializer_service_t>();
#else
  const spectre_serializer_service_t *service =
      (const spectre_serializer_service_t *)ecs_singleton_get(
          world, spectre_serializer_service_t);
#endif
  if (service && service->api && service->api->has_serializer) {
    return service->api->has_serializer(world, type);
  } else {
    sandbox::modules::logs::error(
        flecs_world, "[Serializer Module] Service not initialized!");
  }
  return serializer_has_serializer(world, type);
}

bool spectre_serializer_is_serializer(ecs_world_t *world, ecs_entity_t entity) {
#ifdef __cplusplus
  flecs::world flecs_world(world);
  const spectre_serializer_service_t *service =
      flecs_world.try_get<spectre_serializer_service_t>();
#else
  const spectre_serializer_service_t *service =
      (const spectre_serializer_service_t *)ecs_singleton_get(
          world, spectre_serializer_service_t);
#endif
  if (service && service->api && service->api->is_serializer) {
    return service->api->is_serializer(world, entity);
  } else {
    sandbox::modules::logs::error(
        flecs_world, "[Serializer Module] Service not initialized!");
  }
  return serializer_is_serializer(world, entity);
}

ecs_entity_t spectre_serializer_find_serializer(ecs_world_t *world,
                                                const char *type) {
#ifdef __cplusplus
  flecs::world flecs_world(world);
  const spectre_serializer_service_t *service =
      flecs_world.try_get<spectre_serializer_service_t>();
#else
  const spectre_serializer_service_t *service =
      (const spectre_serializer_service_t *)ecs_singleton_get(
          world, spectre_serializer_service_t);
#endif
  if (service && service->api && service->api->find_serializer) {
    return service->api->find_serializer(world, type);
  } else {
    sandbox::modules::logs::error(
        flecs_world, "[Serializer Module] Service not initialized!");
  }
  return serializer_find_serializer(world, type);
}

sandbox_properties_handle_t
spectre_serializer_serialize_entity(ecs_world_t *world, ecs_entity_t serializer,
                                    ecs_entity_t entity) {
#ifdef __cplusplus
  flecs::world flecs_world(world);
  const spectre_serializer_service_t *service =
      flecs_world.try_get<spectre_serializer_service_t>();
#else
  const spectre_serializer_service_t *service =
      (const spectre_serializer_service_t *)ecs_singleton_get(
          world, spectre_serializer_service_t);
#endif
  if (service && service->api && service->api->serialize_entity) {
    return service->api->serialize_entity(world, serializer, entity);
  } else {
    sandbox::modules::logs::error(
        flecs_world, "[Serializer Module] Service not initialized!");
  }
  return serializer_serialize_entity(world, serializer, entity);
}

void spectre_serializer_deserialize_entity(ecs_world_t *world,
                                           ecs_entity_t serializer,
                                           ecs_entity_t target,
                                           sandbox_properties_handle_t props) {
#ifdef __cplusplus
  flecs::world flecs_world(world);
  const spectre_serializer_service_t *service =
      flecs_world.try_get<spectre_serializer_service_t>();
#else
  const spectre_serializer_service_t *service =
      (const spectre_serializer_service_t *)ecs_singleton_get(
          world, spectre_serializer_service_t);
#endif
  if (service && service->api && service->api->deserialize_entity) {
    service->api->deserialize_entity(world, serializer, target, props);
    return;
  } else {
    sandbox::modules::logs::error(
        flecs_world, "[Serializer Module] Service not initialized!");
  }
  serializer_deserialize_entity(world, serializer, target, props);
}

// --- SDK Implementations ---
namespace spectre::modules {
void serializer::register_serializer(
    const flecs::world &entity_world, const char *type,
    const spectre_serializer_component *serializer) {
  spectre_serializer_register_serializer(entity_world.c_ptr(), type,
                                         serializer);
}

bool serializer::has_serializer(const flecs::world &entity_world,
                                const char *type) {
  return spectre_serializer_has_serializer(entity_world.c_ptr(), type);
}

bool serializer::is_serializer(const flecs::world &entity_world,
                               flecs::entity entity) {
  return spectre_serializer_is_serializer(entity_world.c_ptr(), entity.id());
}

ecs_entity_t serializer::find_serializer(const flecs::world &entity_world,
                                         const char *type) {
  return spectre_serializer_find_serializer(entity_world.c_ptr(), type);
}

sandbox_properties_handle_t
serializer::serialize_entity(const flecs::world &entity_world,
                             ecs_entity_t serializer, ecs_entity_t entity) {
  return spectre_serializer_serialize_entity(entity_world.c_ptr(), serializer,
                                             entity);
}

void serializer::deserialize_entity(const flecs::world &entity_world,
                                    ecs_entity_t serializer,
                                    ecs_entity_t target,
                                    sandbox_properties_handle_t props) {
  spectre_serializer_deserialize_entity(entity_world.c_ptr(), serializer,
                                        target, props);
}
} // namespace spectre::modules
