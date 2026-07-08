#include <spectre/sdk/scenes.hpp>
#include "spectre/services/scenes_service.h"
#include "scenes_module.h"

static sandbox_properties_handle_t scenes_serialize_state(ecs_world_t* entity_world, ecs_entity_t state);
static ecs_entity_t                scenes_deserialize_state(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static sandbox_properties_handle_t scenes_serialize_scene(ecs_world_t* entity_world, ecs_entity_t scene);
static ecs_entity_t                scenes_deserialize_scene(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static void                         scenes_register_state(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static void                         scenes_register_scene(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static ecs_entity_t                scenes_find_state(ecs_world_t* entity_world, const char* name);
static ecs_entity_t                scenes_find_scene(ecs_world_t* entity_world, const char* name);
static bool                         scenes_has_state(ecs_world_t* entity_world, const char* name);
static bool                         scenes_has_scene(ecs_world_t* entity_world, const char* name);
static bool                         scenes_is_state(ecs_world_t* entity_world, ecs_entity_t entity);
static bool                         scenes_is_scene(ecs_world_t* entity_world, ecs_entity_t entity);
static ecs_entity_t                scenes_find_current_state(ecs_world_t* entity_world);
static ecs_query_t*                scenes_find_current_scenes(ecs_world_t* entity_world);
static void                         scenes_push_state(ecs_world_t* entity_world, ecs_entity_t state);
static void                         scenes_pop_state(ecs_world_t* entity_world);

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

static sandbox_properties_handle_t scenes_serialize_state(ecs_world_t* entity_world, ecs_entity_t state) {
    if (!entity_world) return {0};
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->serialize_state(flecs_world.entity(state)).get_raw();
    return {0};
}

static ecs_entity_t scenes_deserialize_state(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->deserialize_state(sandbox::properties(props)).id();
    return 0;
}

static sandbox_properties_handle_t scenes_serialize_scene(ecs_world_t* entity_world, ecs_entity_t scene) {
    if (!entity_world) return {0};
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->serialize_scene(flecs_world.entity(scene)).get_raw();
    return {0};
}

static ecs_entity_t scenes_deserialize_scene(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->deserialize_scene(sandbox::properties(props)).id();
    return 0;
}

static void scenes_register_state(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) module->register_state(sandbox::properties(props));
}

static void scenes_register_scene(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) module->register_scene(sandbox::properties(props));
}

static ecs_entity_t scenes_find_state(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->find_state(name).id();
    return 0;
}

static ecs_entity_t scenes_find_scene(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->find_scene(name).id();
    return 0;
}

static bool scenes_has_state(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->has_state(name);
    return false;
}

static bool scenes_has_scene(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->has_scene(name);
    return false;
}

static bool scenes_is_state(ecs_world_t* entity_world, ecs_entity_t entity) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->is_state(flecs_world.entity(entity));
    return false;
}

static bool scenes_is_scene(ecs_world_t* entity_world, ecs_entity_t entity) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->is_scene(flecs_world.entity(entity));
    return false;
}

static ecs_entity_t scenes_find_current_state(ecs_world_t* entity_world) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) return module->find_current_state().id();
    return 0;
}

static ecs_query_t* scenes_find_current_scenes(ecs_world_t* entity_world) {
    // TODO: map flecs::query<> to ecs_query_t*
    return nullptr;
}

static void scenes_push_state(ecs_world_t* entity_world, ecs_entity_t state) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) module->push_state(flecs_world.entity(state));
}

static void scenes_pop_state(ecs_world_t* entity_world) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::scenes_module_t>();
    if (module) module->pop_state();
}

// --- Public C API Implementations ---
sandbox_properties_handle_t spectre_scenes_serialize_state(ecs_world_t* world, ecs_entity_t state) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_state) {
        return service->api->serialize_state(world, state);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}

ecs_entity_t spectre_scenes_deserialize_state(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_state) {
        return service->api->deserialize_state(world, props);
        
    }
    return (ecs_entity_t){0};
}

sandbox_properties_handle_t spectre_scenes_serialize_scene(ecs_world_t* world, ecs_entity_t scene) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->serialize_scene) {
        return service->api->serialize_scene(world, scene);
        
    }
    sandbox_properties_handle_t invalid = {0}; return invalid;
}

ecs_entity_t spectre_scenes_deserialize_scene(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->deserialize_scene) {
        return service->api->deserialize_scene(world, props);
        
    }
    return (ecs_entity_t){0};
}

void spectre_scenes_register_state(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->register_state) {
        service->api->register_state(world, props);
        return;
    }
    
}

void spectre_scenes_register_scene(ecs_world_t* world, sandbox_properties_handle_t props) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->register_scene) {
        service->api->register_scene(world, props);
        return;
    }
    
}

ecs_entity_t spectre_scenes_find_state(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->find_state) {
        return service->api->find_state(world, name);
        
    }
    return (ecs_entity_t){0};
}

ecs_entity_t spectre_scenes_find_scene(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->find_scene) {
        return service->api->find_scene(world, name);
        
    }
    return (ecs_entity_t){0};
}

bool spectre_scenes_has_state(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->has_state) {
        return service->api->has_state(world, name);
        
    }
    return false;
}

bool spectre_scenes_has_scene(ecs_world_t* world, const char* name) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->has_scene) {
        return service->api->has_scene(world, name);
        
    }
    return false;
}

bool spectre_scenes_is_state(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->is_state) {
        return service->api->is_state(world, entity);
        
    }
    return false;
}

bool spectre_scenes_is_scene(ecs_world_t* world, ecs_entity_t entity) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->is_scene) {
        return service->api->is_scene(world, entity);
        
    }
    return false;
}

ecs_entity_t spectre_scenes_find_current_state(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->find_current_state) {
        return service->api->find_current_state(world);
        
    }
    return (ecs_entity_t){0};
}

ecs_query_t* spectre_scenes_find_current_scenes(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->find_current_scenes) {
        return service->api->find_current_scenes(world);
        
    }
    return NULL;
}

void spectre_scenes_push_state(ecs_world_t* world, ecs_entity_t state) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->push_state) {
        service->api->push_state(world, state);
        return;
    }
    
}

void spectre_scenes_pop_state(ecs_world_t* world) {
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
#ifdef __cplusplus
    flecs::world flecs_world(world);
    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();
#else
    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_singleton_get(world, spectre_scenes_service_t);
#endif
#endif
    if (service && service->api && service->api->pop_state) {
        service->api->pop_state(world);
        return;
    }
    
}

// --- SDK Implementations ---
namespace spectre::modules {
sandbox_properties_handle_t scenes::serialize_state(const flecs::world& entity_world, ecs_entity_t state) {
            return spectre_scenes_serialize_state(entity_world.c_ptr(), state);
        }

ecs_entity_t scenes::deserialize_state(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_scenes_deserialize_state(entity_world.c_ptr(), props);
        }

sandbox_properties_handle_t scenes::serialize_scene(const flecs::world& entity_world, ecs_entity_t scene) {
            return spectre_scenes_serialize_scene(entity_world.c_ptr(), scene);
        }

ecs_entity_t scenes::deserialize_scene(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_scenes_deserialize_scene(entity_world.c_ptr(), props);
        }

void scenes::register_state(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_scenes_register_state(entity_world.c_ptr(), props);}

void scenes::register_scene(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_scenes_register_scene(entity_world.c_ptr(), props);}

ecs_entity_t scenes::find_state(const flecs::world& entity_world, const char* name) {
            return spectre_scenes_find_state(entity_world.c_ptr(), name);
        }

ecs_entity_t scenes::find_scene(const flecs::world& entity_world, const char* name) {
            return spectre_scenes_find_scene(entity_world.c_ptr(), name);
        }

bool scenes::has_state(const flecs::world& entity_world, const char* name) {
            return spectre_scenes_has_state(entity_world.c_ptr(), name);
        }

bool scenes::has_scene(const flecs::world& entity_world, const char* name) {
            return spectre_scenes_has_scene(entity_world.c_ptr(), name);
        }

bool scenes::is_state(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_scenes_is_state(entity_world.c_ptr(), entity);
        }

bool scenes::is_scene(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_scenes_is_scene(entity_world.c_ptr(), entity);
        }

ecs_entity_t scenes::find_current_state(const flecs::world& entity_world) {
            return spectre_scenes_find_current_state(entity_world.c_ptr());
        }

ecs_query_t* scenes::find_current_scenes(const flecs::world& entity_world) {
            return spectre_scenes_find_current_scenes(entity_world.c_ptr());
        }

void scenes::push_state(const flecs::world& entity_world, ecs_entity_t state) {
            spectre_scenes_push_state(entity_world.c_ptr(), state);}

void scenes::pop_state(const flecs::world& entity_world) {
            spectre_scenes_pop_state(entity_world.c_ptr());}
}
