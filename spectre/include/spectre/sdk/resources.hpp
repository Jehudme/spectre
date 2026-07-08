#pragma once
#include <spectre/services/resources_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class resources {
    public:
        static ecs_entity_t deserialize_resource(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->deserialize_resource) {
                    return service->api->deserialize_resource(entity_world.c_ptr(), props);
                }
            } return 0;
        }

        static sandbox_properties_handle_t serialize_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->serialize_resource) {
                    return service->api->serialize_resource(entity_world.c_ptr(), resourceEntity);
                }
            } return {0};
        }

        static void register_resource_loader(const flecs::world& entity_world, const char* type, spectre_resource_loader_t loader) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->register_resource_loader) {
                    service->api->register_resource_loader(entity_world.c_ptr(), type, loader);
                }
            }
        }

        static void register_resource(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->register_resource) {
                    service->api->register_resource(entity_world.c_ptr(), props);
                }
            }
        }

        static bool has_resource_loader(const flecs::world& entity_world, const char* type) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->has_resource_loader) {
                    return service->api->has_resource_loader(entity_world.c_ptr(), type);
                }
            } return false;
        }

        static bool has_resource(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->has_resource) {
                    return service->api->has_resource(entity_world.c_ptr(), name);
                }
            } return false;
        }

        static bool is_resource(const flecs::world& entity_world, ecs_entity_t entity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->is_resource) {
                    return service->api->is_resource(entity_world.c_ptr(), entity);
                }
            } return false;
        }

        static ecs_entity_t find_resource_loader(const flecs::world& entity_world, const char* type) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->find_resource_loader) {
                    return service->api->find_resource_loader(entity_world.c_ptr(), type);
                }
            } return 0;
        }

        static ecs_entity_t find_resource(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->find_resource) {
                    return service->api->find_resource(entity_world.c_ptr(), name);
                }
            } return 0;
        }

        static bool is_resource_loaded(const flecs::world& entity_world, ecs_entity_t resource) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->is_resource_loaded) {
                    return service->api->is_resource_loaded(entity_world.c_ptr(), resource);
                }
            } return false;
        }

        static void load_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->load_resource) {
                    service->api->load_resource(entity_world.c_ptr(), resourceEntity);
                }
            }
        }

        static void free_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_resources_service_t)) {
                if (service->api && service->api->free_resource) {
                    service->api->free_resource(entity_world.c_ptr(), resourceEntity);
                }
            }
        }
    };
}
#endif
