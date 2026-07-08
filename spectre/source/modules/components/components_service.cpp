#include <spectre/sdk/components.hpp>
#include <spectre/services/components_service.h>
#include "components_module.h"

static sandbox_properties_handle_t components_serialize_component(ecs_world_t* world, ecs_entity_t entity);
static ecs_entity_t components_deserialize_component(ecs_world_t* world, const char* type, sandbox_properties_handle_t props);
static void components_register_component(ecs_world_t* world, const char* type, spectre_component_registration_fn_t registration, spectre_serializer_component serializer);
static bool components_has_component(ecs_world_t* world, const char* type);
static bool components_is_component(ecs_world_t* world, ecs_entity_t entity);

spectre_components_api_t g_components_api = {
    .serialize_component = components_serialize_component,
    .deserialize_component = components_deserialize_component,
    .register_component = components_register_component,
    .has_component = components_has_component,
    .is_component = components_is_component,
};

SANDBOX_DEFINE_SERVICE(spectre_components_service_t, spectre_components_api_t, &g_components_api)

static sandbox_properties_handle_t components_serialize_component(ecs_world_t* world, ecs_entity_t entity) {
    if (!world) return {0};
    flecs::world flecs_world(world);
    auto* module = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (module) return module->serialize_component(flecs_world.entity(entity)).get_raw();
    return {0};
}

static ecs_entity_t components_deserialize_component(ecs_world_t* world, const char* type, sandbox_properties_handle_t props) {
    if (!world || !type) return 0;
    flecs::world flecs_world(world);
    auto* module = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (module) return module->deserialize_component(type, sandbox::properties(props)).id();
    return 0;
}

static void components_register_component(ecs_world_t* world, const char* type, spectre_component_registration_fn_t registration, spectre_serializer_component serializer) {
    if (!world || !type) return;
    flecs::world flecs_world(world);
    auto* module = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (module) module->register_component(type, registration, serializer);
}

static bool components_has_component(ecs_world_t* world, const char* type) {
    if (!world || !type) return false;
    flecs::world flecs_world(world);
    auto* module = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (module) return module->has_component(type);
    return false;
}

static bool components_is_component(ecs_world_t* world, ecs_entity_t entity) {
    if (!world) return false;
    flecs::world flecs_world(world);
    auto* module = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (module) return module->is_component(flecs_world.entity(entity));
    return false;
}

// --- Public C API Implementations ---
sandbox_properties_handle_t spectre_components_serialize_component(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_components_service_t* service = flecs_world.try_get<spectre_components_service_t>();
#else
    const spectre_components_service_t* service = (const spectre_components_service_t*)ecs_singleton_get(world, spectre_components_service_t);
#endif
    if (service && service->api && service->api->serialize_component) {
        return service->api->serialize_component(world, entity);
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}

ecs_entity_t spectre_components_deserialize_component(ecs_world_t* world, const char* type, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_components_service_t* service = flecs_world.try_get<spectre_components_service_t>();
#else
    const spectre_components_service_t* service = (const spectre_components_service_t*)ecs_singleton_get(world, spectre_components_service_t);
#endif
    if (service && service->api && service->api->deserialize_component) {
        return service->api->deserialize_component(world, type, props);
    }
    return 0;
}

void spectre_components_register_component(ecs_world_t* world, const char* type, spectre_component_registration_fn_t registration, spectre_serializer_component serializer) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_components_service_t* service = flecs_world.try_get<spectre_components_service_t>();
#else
    const spectre_components_service_t* service = (const spectre_components_service_t*)ecs_singleton_get(world, spectre_components_service_t);
#endif
    if (service && service->api && service->api->register_component) {
        service->api->register_component(world, type, registration, serializer);
    }
}

bool spectre_components_has_component(ecs_world_t* world, const char* type) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_components_service_t* service = flecs_world.try_get<spectre_components_service_t>();
#else
    const spectre_components_service_t* service = (const spectre_components_service_t*)ecs_singleton_get(world, spectre_components_service_t);
#endif
    if (service && service->api && service->api->has_component) {
        return service->api->has_component(world, type);
    }
    return false;
}

bool spectre_components_is_component(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_components_service_t* service = flecs_world.try_get<spectre_components_service_t>();
#else
    const spectre_components_service_t* service = (const spectre_components_service_t*)ecs_singleton_get(world, spectre_components_service_t);
#endif
    if (service && service->api && service->api->is_component) {
        return service->api->is_component(world, entity);
    }
    return false;
}

// --- SDK Implementations ---
namespace spectre::modules {
    sandbox::properties components::serialize_component(const flecs::world& entity_world, flecs::entity entity) {
        sandbox_properties_handle_t handle = spectre_components_serialize_component(entity_world.c_ptr(), entity.id());
        return sandbox::properties(handle);
    }

    flecs::entity components::deserialize_component(const flecs::world& entity_world, std::string_view type, sandbox::properties props) {
        ecs_entity_t ent = spectre_components_deserialize_component(entity_world.c_ptr(), type.data(), props.handle());
        return flecs::entity(entity_world, ent);
    }

    void components::register_component(const flecs::world& entity_world, std::string_view type, spectre_component_registration_fn_t registration, spectre_serializer_component serializer) {
        spectre_components_register_component(entity_world.c_ptr(), type.data(), registration, serializer);
    }

    bool components::has_component(const flecs::world& entity_world, std::string_view type) {
        return spectre_components_has_component(entity_world.c_ptr(), type.data());
    }

    bool components::is_component(const flecs::world& entity_world, flecs::entity entity) {
        return spectre_components_is_component(entity_world.c_ptr(), entity.id());
    }
}
