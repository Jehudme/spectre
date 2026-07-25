#pragma once
#include <spectre/services/prefabs_service.h>

#ifdef __cplusplus
namespace spectre::modules {
class prefabs {
  public:
    static sandbox_properties_handle_t serialize_entity(const flecs::world& entity_world, ecs_entity_t entity);

    static void deserialize_entity(const flecs::world& entity_world, ecs_entity_t target,
                                   sandbox_properties_handle_t props);

    static void register_prefab(const flecs::world& entity_world, const char* name, sandbox_properties_handle_t props);

    static bool has_prefab(const flecs::world& entity_world, const char* name);

    static bool is_prefab(const flecs::world& entity_world, ecs_entity_t entity);

    static ecs_entity_t find_prefab(const flecs::world& entity_world, const char* name);

    static ecs_entity_t create_entity_from_props(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static ecs_entity_t create_entity_from_prefab(const flecs::world& entity_world, ecs_entity_t prefab);

    static ecs_entity_t create_entity_from_name(const flecs::world& entity_world, const char* name);

    static void import_configuration(const flecs::world& entity_world, const char* directory_path) {
        spectre_prefabs_import_configuration(entity_world.c_ptr(), directory_path);
    }
    
    static void export_configuration(const flecs::world& entity_world, const char* directory_path) {
        spectre_prefabs_export_configuration(entity_world.c_ptr(), directory_path);
    }
};
} // namespace spectre::modules
#endif
