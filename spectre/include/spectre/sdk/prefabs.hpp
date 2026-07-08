#pragma once
#include <spectre/services/prefabs_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class prefabs {
    public:
        static sandbox_properties_handle_t serialize_entity(const flecs::world& entity_world, ecs_entity_t entity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_prefabs_service_t)) {
                if (service->api && service->api->serialize_entity) {
                    return service->api->serialize_entity(entity_world.c_ptr(), entity);
                }
            } return {0};
        }

        static ecs_entity_t deserialize_entity(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_prefabs_service_t)) {
                if (service->api && service->api->deserialize_entity) {
                    return service->api->deserialize_entity(entity_world.c_ptr(), props);
                }
            } return 0;
        }

        static void register_prefab(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_prefabs_service_t)) {
                if (service->api && service->api->register_prefab) {
                    service->api->register_prefab(entity_world.c_ptr(), props);
                }
            }
        }

        static bool has_prefab(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_prefabs_service_t)) {
                if (service->api && service->api->has_prefab) {
                    return service->api->has_prefab(entity_world.c_ptr(), name);
                }
            } return false;
        }

        static bool is_prefab(const flecs::world& entity_world, ecs_entity_t entity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_prefabs_service_t)) {
                if (service->api && service->api->is_prefab) {
                    return service->api->is_prefab(entity_world.c_ptr(), entity);
                }
            } return false;
        }

        static ecs_entity_t find_prefab(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_prefabs_service_t)) {
                if (service->api && service->api->find_prefab) {
                    return service->api->find_prefab(entity_world.c_ptr(), name);
                }
            } return 0;
        }

        static ecs_entity_t create_entity_from_props(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_prefabs_service_t)) {
                if (service->api && service->api->create_entity_from_props) {
                    return service->api->create_entity_from_props(entity_world.c_ptr(), props);
                }
            } return 0;
        }

        static ecs_entity_t create_entity_from_prefab(const flecs::world& entity_world, ecs_entity_t prefab) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_prefabs_service_t)) {
                if (service->api && service->api->create_entity_from_prefab) {
                    return service->api->create_entity_from_prefab(entity_world.c_ptr(), prefab);
                }
            } return 0;
        }

        static ecs_entity_t create_entity_from_name(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_prefabs_service_t)) {
                if (service->api && service->api->create_entity_from_name) {
                    return service->api->create_entity_from_name(entity_world.c_ptr(), name);
                }
            } return 0;
        }
    };
}
#endif
