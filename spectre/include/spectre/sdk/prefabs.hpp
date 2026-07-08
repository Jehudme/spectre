#pragma once
#include <spectre/services/prefabs_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class prefabs {
    public:
        static sandbox_properties_handle_t serialize_entity(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_prefabs_serialize_entity(entity_world.c_ptr(), entity);
        }

        static ecs_entity_t deserialize_entity(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_prefabs_deserialize_entity(entity_world.c_ptr(), props);
        }

        static void register_prefab(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_prefabs_register_prefab(entity_world.c_ptr(), props);}

        static bool has_prefab(const flecs::world& entity_world, const char* name) {
            return spectre_prefabs_has_prefab(entity_world.c_ptr(), name);
        }

        static bool is_prefab(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_prefabs_is_prefab(entity_world.c_ptr(), entity);
        }

        static ecs_entity_t find_prefab(const flecs::world& entity_world, const char* name) {
            return spectre_prefabs_find_prefab(entity_world.c_ptr(), name);
        }

        static ecs_entity_t create_entity_from_props(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_prefabs_create_entity_from_props(entity_world.c_ptr(), props);
        }

        static ecs_entity_t create_entity_from_prefab(const flecs::world& entity_world, ecs_entity_t prefab) {
            return spectre_prefabs_create_entity_from_prefab(entity_world.c_ptr(), prefab);
        }

        static ecs_entity_t create_entity_from_name(const flecs::world& entity_world, const char* name) {
            return spectre_prefabs_create_entity_from_name(entity_world.c_ptr(), name);
        }
    };
}
#endif
