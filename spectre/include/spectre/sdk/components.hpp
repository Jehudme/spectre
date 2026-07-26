#pragma once
#include <spectre/services/components_service.h>

#ifdef __cplusplus
#    include <flecs.h>
#    include <sandbox/sdk/properties.hpp>

namespace spectre::modules {
class components {
  public:
    static void register_component(const flecs::world& entity_world, const char* name,
                                   spectre_component_registration_fn_t registration_fn,
                                   spectre_serializer_component serializer = spectre_serializer_component{});

    static ecs_entity_t find_component(const flecs::world& entity_world, const char* name);

    static bool has_component(const flecs::world& entity_world, const char* name);

    static bool is_component(const flecs::world& entity_world, flecs::entity entity);

    static void register_component(const flecs::world& entity_world, const char* name, sandbox::properties properties);
    static void import_configuration(const flecs::world& entity_world, const char* directory_path);
    static void export_configuration(const flecs::world& entity_world, const char* directory_path);
};
} // namespace spectre::modules
#endif
