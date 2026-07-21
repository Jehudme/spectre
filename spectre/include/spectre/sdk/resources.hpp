#pragma once
#include <spectre/services/resources_service.h>

#ifdef __cplusplus
namespace spectre::modules {
class resources {
  public:
    static void deserialize_resource(const flecs::world& entity_world, ecs_entity_t target,
                                     sandbox_properties_handle_t props);

    static sandbox_properties_handle_t serialize_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity);

    static void register_resource_loader(const flecs::world& entity_world, const char* type,
                                         spectre_resource_loader_component_t loader);

    static void register_resource(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static bool has_resource_loader(const flecs::world& entity_world, const char* type);

    static bool has_resource(const flecs::world& entity_world, const char* name);

    static bool is_resource(const flecs::world& entity_world, ecs_entity_t entity);

    static ecs_entity_t find_resource_loader(const flecs::world& entity_world, const char* type);

    static ecs_entity_t find_resource(const flecs::world& entity_world, const char* name);

    static bool is_resource_loaded(const flecs::world& entity_world, ecs_entity_t resource);

    static void load_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity);

    static void free_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity);

    static void* get_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity);
};
} // namespace spectre::modules
#endif
