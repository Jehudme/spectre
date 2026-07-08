#pragma once
#include <spectre/services/resources_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class resources {
    public:
        static ecs_entity_t deserialize_resource(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_resources_deserialize_resource(entity_world.c_ptr(), props);
        }

        static sandbox_properties_handle_t serialize_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity) {
            return spectre_resources_serialize_resource(entity_world.c_ptr(), resourceEntity);
        }

        static void register_resource_loader(const flecs::world& entity_world, const char* type, spectre_resource_loader_t loader) {
            spectre_resources_register_resource_loader(entity_world.c_ptr(), type, loader);}

        static void register_resource(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_resources_register_resource(entity_world.c_ptr(), props);}

        static bool has_resource_loader(const flecs::world& entity_world, const char* type) {
            return spectre_resources_has_resource_loader(entity_world.c_ptr(), type);
        }

        static bool has_resource(const flecs::world& entity_world, const char* name) {
            return spectre_resources_has_resource(entity_world.c_ptr(), name);
        }

        static bool is_resource(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_resources_is_resource(entity_world.c_ptr(), entity);
        }

        static ecs_entity_t find_resource_loader(const flecs::world& entity_world, const char* type) {
            return spectre_resources_find_resource_loader(entity_world.c_ptr(), type);
        }

        static ecs_entity_t find_resource(const flecs::world& entity_world, const char* name) {
            return spectre_resources_find_resource(entity_world.c_ptr(), name);
        }

        static bool is_resource_loaded(const flecs::world& entity_world, ecs_entity_t resource) {
            return spectre_resources_is_resource_loaded(entity_world.c_ptr(), resource);
        }

        static void load_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity) {
            spectre_resources_load_resource(entity_world.c_ptr(), resourceEntity);}

        static void free_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity) {
            spectre_resources_free_resource(entity_world.c_ptr(), resourceEntity);}
    };
}
#endif
