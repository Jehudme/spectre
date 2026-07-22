#pragma once
#include <spectre/services/components_service.h>

#ifdef __cplusplus
#include <flecs.h>

namespace spectre::modules {
class components {
  public:
    static void register_component(const flecs::world& entity_world, const char* name,
                                   spectre_component_registration_fn_t registration_fn,
                                   spectre_serializer_component serializer);

    static ecs_entity_t find_component(const flecs::world& entity_world, const char* name);

    static bool has_component(const flecs::world& entity_world, const char* name);

    static bool is_component(const flecs::world& entity_world, flecs::entity entity);
};
} // namespace spectre::modules
#endif
