#include <spectre/sdk/resources.hpp>
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

// --- Public C API Implementations ---
ecs_entity_t spectre_resources_deserialize_resource(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_resource) {
        return service->api->deserialize_resource(world, props);
        
    }
    return (ecs_entity_t){0};
}

sandbox_properties_handle_t spectre_resources_serialize_resource(ecs_world_t* world, ecs_entity_t resourceEntity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_resource) {
        return service->api->serialize_resource(world, resourceEntity);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}

void spectre_resources_register_resource_loader(ecs_world_t* world, const char* type, spectre_resource_loader_t loader) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->register_resource_loader) {
        service->api->register_resource_loader(world, type, loader);
        return;
    }
    
}

void spectre_resources_register_resource(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->register_resource) {
        service->api->register_resource(world, props);
        return;
    }
    
}

bool spectre_resources_has_resource_loader(ecs_world_t* world, const char* type) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->has_resource_loader) {
        return service->api->has_resource_loader(world, type);
        
    }
    return false;
}

bool spectre_resources_has_resource(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->has_resource) {
        return service->api->has_resource(world, name);
        
    }
    return false;
}

bool spectre_resources_is_resource(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->is_resource) {
        return service->api->is_resource(world, entity);
        
    }
    return false;
}

ecs_entity_t spectre_resources_find_resource_loader(ecs_world_t* world, const char* type) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->find_resource_loader) {
        return service->api->find_resource_loader(world, type);
        
    }
    return (ecs_entity_t){0};
}

ecs_entity_t spectre_resources_find_resource(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->find_resource) {
        return service->api->find_resource(world, name);
        
    }
    return (ecs_entity_t){0};
}

bool spectre_resources_is_resource_loaded(ecs_world_t* world, ecs_entity_t resource) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->is_resource_loaded) {
        return service->api->is_resource_loaded(world, resource);
        
    }
    return false;
}

void spectre_resources_load_resource(ecs_world_t* world, ecs_entity_t resourceEntity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->load_resource) {
        service->api->load_resource(world, resourceEntity);
        return;
    }
    
}

void spectre_resources_free_resource(ecs_world_t* world, ecs_entity_t resourceEntity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_resources_service_t* service = flecs_world.try_get<spectre_resources_service_t>();
#else
    const spectre_resources_service_t* service = (const spectre_resources_service_t*)ecs_singleton_get(world, spectre_resources_service_t);
#endif
#endif
    if (service && service->api && service->api->free_resource) {
        service->api->free_resource(world, resourceEntity);
        return;
    }
    
}

// --- SDK Implementations ---
namespace spectre::modules {
ecs_entity_t resources::deserialize_resource(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_resources_deserialize_resource(entity_world.c_ptr(), props);
        }

sandbox_properties_handle_t resources::serialize_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity) {
            return spectre_resources_serialize_resource(entity_world.c_ptr(), resourceEntity);
        }

void resources::register_resource_loader(const flecs::world& entity_world, const char* type, spectre_resource_loader_t loader) {
            spectre_resources_register_resource_loader(entity_world.c_ptr(), type, loader);}

void resources::register_resource(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_resources_register_resource(entity_world.c_ptr(), props);}

bool resources::has_resource_loader(const flecs::world& entity_world, const char* type) {
            return spectre_resources_has_resource_loader(entity_world.c_ptr(), type);
        }

bool resources::has_resource(const flecs::world& entity_world, const char* name) {
            return spectre_resources_has_resource(entity_world.c_ptr(), name);
        }

bool resources::is_resource(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_resources_is_resource(entity_world.c_ptr(), entity);
        }

ecs_entity_t resources::find_resource_loader(const flecs::world& entity_world, const char* type) {
            return spectre_resources_find_resource_loader(entity_world.c_ptr(), type);
        }

ecs_entity_t resources::find_resource(const flecs::world& entity_world, const char* name) {
            return spectre_resources_find_resource(entity_world.c_ptr(), name);
        }

bool resources::is_resource_loaded(const flecs::world& entity_world, ecs_entity_t resource) {
            return spectre_resources_is_resource_loaded(entity_world.c_ptr(), resource);
        }

void resources::load_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity) {
            spectre_resources_load_resource(entity_world.c_ptr(), resourceEntity);}

void resources::free_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity) {
            spectre_resources_free_resource(entity_world.c_ptr(), resourceEntity);}
}
