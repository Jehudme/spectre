#include <spectre/sdk/prefabs.hpp>
#include "spectre/services/prefabs_service.h"
#include "prefabs_module.h"

static sandbox_properties_handle_t prefabs_serialize_entity(ecs_world_t* entity_world, ecs_entity_t entity);
static ecs_entity_t                prefabs_deserialize_entity(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static void                        prefabs_register_prefab(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static bool                        prefabs_has_prefab(ecs_world_t* entity_world, const char* name);
static bool                        prefabs_is_prefab(ecs_world_t* entity_world, ecs_entity_t entity);
static ecs_entity_t                prefabs_find_prefab(ecs_world_t* entity_world, const char* name);
static ecs_entity_t                prefabs_create_entity_from_props(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static ecs_entity_t                prefabs_create_entity_from_prefab(ecs_world_t* entity_world, ecs_entity_t prefab);
static ecs_entity_t                prefabs_create_entity_from_name(ecs_world_t* entity_world, const char* name);

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

static sandbox_properties_handle_t prefabs_serialize_entity(ecs_world_t* entity_world, ecs_entity_t entity) {
    if (!entity_world) return {0};
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::prefabs_module_t>();
    if (module) return module->serialize_entity(flecs_world.entity(entity)).get_raw();
    return {0};
}

static ecs_entity_t prefabs_deserialize_entity(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::prefabs_module_t>();
    if (module) return module->deserialize_entity(sandbox::properties(props)).id();
    return 0;
}

static void prefabs_register_prefab(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::prefabs_module_t>();
    if (module) module->register_prefab(sandbox::properties(props));
}

static bool prefabs_has_prefab(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::prefabs_module_t>();
    if (module) return module->has_prefab(name);
    return false;
}

static bool prefabs_is_prefab(ecs_world_t* entity_world, ecs_entity_t entity) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::prefabs_module_t>();
    if (module) return module->is_prefab(flecs_world.entity(entity));
    return false;
}

static ecs_entity_t prefabs_find_prefab(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::prefabs_module_t>();
    if (module) return module->find_prefab(name).id();
    return 0;
}

static ecs_entity_t prefabs_create_entity_from_props(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::prefabs_module_t>();
    if (module) return module->create_entity(sandbox::properties(props)).id();
    return 0;
}

static ecs_entity_t prefabs_create_entity_from_prefab(ecs_world_t* entity_world, ecs_entity_t prefab) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::prefabs_module_t>();
    if (module) return module->create_entity(flecs_world.entity(prefab)).id();
    return 0;
}

static ecs_entity_t prefabs_create_entity_from_name(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::prefabs_module_t>();
    if (module) return module->create_entity(std::string_view(name)).id();
    return 0;
}

// --- Public C API Implementations ---
sandbox_properties_handle_t spectre_prefabs_serialize_entity(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_entity) {
        return service->api->serialize_entity(world, entity);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}

ecs_entity_t spectre_prefabs_deserialize_entity(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_entity) {
        return service->api->deserialize_entity(world, props);
        
    }
    return (ecs_entity_t){0};
}

void spectre_prefabs_register_prefab(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->register_prefab) {
        service->api->register_prefab(world, props);
        return;
    }
    
}

bool spectre_prefabs_has_prefab(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->has_prefab) {
        return service->api->has_prefab(world, name);
        
    }
    return false;
}

bool spectre_prefabs_is_prefab(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->is_prefab) {
        return service->api->is_prefab(world, entity);
        
    }
    return false;
}

ecs_entity_t spectre_prefabs_find_prefab(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->find_prefab) {
        return service->api->find_prefab(world, name);
        
    }
    return (ecs_entity_t){0};
}

ecs_entity_t spectre_prefabs_create_entity_from_props(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->create_entity_from_props) {
        return service->api->create_entity_from_props(world, props);
        
    }
    return (ecs_entity_t){0};
}

ecs_entity_t spectre_prefabs_create_entity_from_prefab(ecs_world_t* world, ecs_entity_t prefab) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->create_entity_from_prefab) {
        return service->api->create_entity_from_prefab(world, prefab);
        
    }
    return (ecs_entity_t){0};
}

ecs_entity_t spectre_prefabs_create_entity_from_name(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();
#else
    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_singleton_get(world, spectre_prefabs_service_t);
#endif
#endif
    if (service && service->api && service->api->create_entity_from_name) {
        return service->api->create_entity_from_name(world, name);
        
    }
    return (ecs_entity_t){0};
}

// --- SDK Implementations ---
namespace spectre::modules {
sandbox_properties_handle_t prefabs::serialize_entity(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_prefabs_serialize_entity(entity_world.c_ptr(), entity);
        }

ecs_entity_t prefabs::deserialize_entity(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_prefabs_deserialize_entity(entity_world.c_ptr(), props);
        }

void prefabs::register_prefab(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_prefabs_register_prefab(entity_world.c_ptr(), props);}

bool prefabs::has_prefab(const flecs::world& entity_world, const char* name) {
            return spectre_prefabs_has_prefab(entity_world.c_ptr(), name);
        }

bool prefabs::is_prefab(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_prefabs_is_prefab(entity_world.c_ptr(), entity);
        }

ecs_entity_t prefabs::find_prefab(const flecs::world& entity_world, const char* name) {
            return spectre_prefabs_find_prefab(entity_world.c_ptr(), name);
        }

ecs_entity_t prefabs::create_entity_from_props(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_prefabs_create_entity_from_props(entity_world.c_ptr(), props);
        }

ecs_entity_t prefabs::create_entity_from_prefab(const flecs::world& entity_world, ecs_entity_t prefab) {
            return spectre_prefabs_create_entity_from_prefab(entity_world.c_ptr(), prefab);
        }

ecs_entity_t prefabs::create_entity_from_name(const flecs::world& entity_world, const char* name) {
            return spectre_prefabs_create_entity_from_name(entity_world.c_ptr(), name);
        }
}
