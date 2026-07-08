#include <spectre/sdk/serializer.hpp>
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

// --- Public C API Implementations ---
void spectre_serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_t* serializer) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->register_serializer) {
        service->api->register_serializer(world, type, serializer);
        return;
    }
    
}

bool spectre_serializer_has_serializer(ecs_world_t* world, const char* type) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->has_serializer) {
        return service->api->has_serializer(world, type);
        
    }
    return false;
}

ecs_entity_t spectre_serializer_find_serializer(ecs_world_t* world, const char* type) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->find_serializer) {
        return service->api->find_serializer(world, type);
        
    }
    return (ecs_entity_t){0};
}

sandbox_properties_handle_t spectre_serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_entity) {
        return service->api->serialize_entity(world, serializer, entity);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}

ecs_entity_t spectre_serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_serializer_service_t* service = flecs_world.try_get<spectre_serializer_service_t>();
#else
    const spectre_serializer_service_t* service = (const spectre_serializer_service_t*)ecs_singleton_get(world, spectre_serializer_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_entity) {
        return service->api->deserialize_entity(world, serializer, props);
        
    }
    return (ecs_entity_t){0};
}

// --- SDK Implementations ---
namespace spectre::modules {
void serializer::register_serializer(const flecs::world& entity_world, const char* type, const spectre_serializer_t* serializer) {
            spectre_serializer_register_serializer(entity_world.c_ptr(), type, serializer);}

bool serializer::has_serializer(const flecs::world& entity_world, const char* type) {
            return spectre_serializer_has_serializer(entity_world.c_ptr(), type);
        }

ecs_entity_t serializer::find_serializer(const flecs::world& entity_world, const char* type) {
            return spectre_serializer_find_serializer(entity_world.c_ptr(), type);
        }

sandbox_properties_handle_t serializer::serialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, ecs_entity_t entity) {
            return spectre_serializer_serialize_entity(entity_world.c_ptr(), serializer, entity);
        }

ecs_entity_t serializer::deserialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, sandbox_properties_handle_t props) {
            return spectre_serializer_deserialize_entity(entity_world.c_ptr(), serializer, props);
        }
}
