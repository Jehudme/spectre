#include "spectre/services/components_service.h"
#include <spectre/sdk/components.hpp>
#include "components_module.h"
#include <sandbox/abi/bootstrapper.h>

void spectre_components_register_component(ecs_world_t* world, const char* name, spectre_component_registration_fn_t registration_fn, spectre_serializer_component serializer);
ecs_entity_t spectre_components_find_component(ecs_world_t* world, const char* name);
bool spectre_components_has_component(ecs_world_t* world, const char* name);
bool spectre_components_is_component(ecs_world_t* world, ecs_entity_t entity);

spectre_components_api_t g_components_api = {
    .register_component = spectre_components_register_component,
    .find_component = spectre_components_find_component,
    .has_component = spectre_components_has_component,
    .is_component = spectre_components_is_component,
    .list_components = spectre_components_list_components,
    .register_dynamic_component = spectre_components_register_dynamic_component,
    .import_configuration = spectre_components_import_configuration,
    .export_configuration = spectre_components_export_configuration
};

SANDBOX_DEFINE_SERVICE(spectre_components_service_t, spectre_components_api_t, &g_components_api)

void spectre_components_register_component(ecs_world_t* world, const char* name, spectre_component_registration_fn_t registration_fn, spectre_serializer_component serializer) {
    if (!world) return;
    flecs::world flecs_world(world);
    auto* mod = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (mod) {
        mod->register_component(name ? name : "", registration_fn, serializer);
    } else {
        printf("[Components Service] FATAL: components_module_t singleton not found!\n");
    }
}

ecs_entity_t spectre_components_find_component(ecs_world_t* world, const char* name) {
    if (!world || !name) return 0;
    flecs::world flecs_world(world);
    auto* mod = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (mod) {
        return mod->find_component(name).id();
    }
    return 0;
}

bool spectre_components_has_component(ecs_world_t* world, const char* name) {
    if (!world || !name) return false;
    flecs::world flecs_world(world);
    auto* mod = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (mod) {
        return mod->has_component(name);
    }
    return false;
}

bool spectre_components_is_component(ecs_world_t* world, ecs_entity_t entity) {
    if (!world || !entity) return false;
    flecs::world flecs_world(world);
    auto* mod = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (mod) {
        return mod->is_component(flecs_world.entity(entity));
    }
    return false;
}

ecs_entity_t* spectre_components_list_components(ecs_world_t* world, size_t* count) {
    static std::vector<ecs_entity_t> result;
    result.clear();
    *count = 0;
    if (!world) return nullptr;
    flecs::world flecs_world(world);
    auto* mod = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (mod) {
        auto entities = mod->list_components();
        for (auto& e : entities) result.push_back(e.id());
    }
    *count = result.size();
    return result.empty() ? nullptr : result.data();
}

void spectre_components_register_dynamic_component(ecs_world_t* world, const char* name, sandbox_properties_handle_t properties) {
    if (!world || !name) return;
    flecs::world flecs_world(world);
    auto* mod = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (mod) {
        sandbox::properties props(properties, false);
        mod->register_component(name, std::move(props));
    }
}

void spectre_components_import_configuration(ecs_world_t* world, const char* directory_path) {
    if (!world || !directory_path) return;
    flecs::world flecs_world(world);
    auto* mod = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (mod) {
        mod->import_configuration(directory_path);
    }
}

void spectre_components_export_configuration(ecs_world_t* world, const char* directory_path) {
    if (!world || !directory_path) return;
    flecs::world flecs_world(world);
    auto* mod = flecs_world.try_get_mut<spectre::modules::components_module_t>();
    if (mod) {
        mod->export_configuration(directory_path);
    }
}

#ifdef __cplusplus
namespace spectre::modules {

void components::register_component(const flecs::world& entity_world, const char* name,
                                    spectre_component_registration_fn_t registration_fn,
                                    spectre_serializer_component serializer) {
    spectre_components_register_component(entity_world.c_ptr(), name, registration_fn, serializer);
}

ecs_entity_t components::find_component(const flecs::world& entity_world, const char* name) {
    return spectre_components_find_component(entity_world.c_ptr(), name);
}

bool components::has_component(const flecs::world& entity_world, const char* name) {
    return spectre_components_has_component(entity_world.c_ptr(), name);
}

bool components::is_component(const flecs::world& entity_world, flecs::entity entity) {
    return spectre_components_is_component(entity_world.c_ptr(), entity.id());
}

std::vector<flecs::entity> components::list_components(const flecs::world& entity_world) {
    size_t count = 0;
    ecs_entity_t* entities = spectre_components_list_components(entity_world.c_ptr(), &count);
    std::vector<flecs::entity> list;
    if (entities && count > 0) {
        list.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            list.push_back(entity_world.entity(entities[i]));
        }
    }
    return list;
}

void components::register_component(const flecs::world& entity_world, const char* name, sandbox::properties properties) {
    spectre_components_register_dynamic_component(entity_world.c_ptr(), name, properties.get_raw());
}

void components::import_configuration(const flecs::world& entity_world, const char* directory_path) {
    spectre_components_import_configuration(entity_world.c_ptr(), directory_path);
}

void components::export_configuration(const flecs::world& entity_world, const char* directory_path) {
    spectre_components_export_configuration(entity_world.c_ptr(), directory_path);
}

} // namespace spectre::modules
#endif
