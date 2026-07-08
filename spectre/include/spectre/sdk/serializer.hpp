#pragma once
#include <spectre/services/serializer_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class serializer {
    public:
        static void register_serializer(const flecs::world& entity_world, const char* type, const spectre_serializer_t* serializer) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_serializer_service_t)) {
                if (service->api && service->api->register_serializer) {
                    service->api->register_serializer(entity_world.c_ptr(), type, serializer);
                }
            }
        }

        static bool has_serializer(const flecs::world& entity_world, const char* type) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_serializer_service_t)) {
                if (service->api && service->api->has_serializer) {
                    return service->api->has_serializer(entity_world.c_ptr(), type);
                }
            } return false;
        }

        static ecs_entity_t find_serializer(const flecs::world& entity_world, const char* type) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_serializer_service_t)) {
                if (service->api && service->api->find_serializer) {
                    return service->api->find_serializer(entity_world.c_ptr(), type);
                }
            } return 0;
        }

        static sandbox_properties_handle_t serialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, ecs_entity_t entity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_serializer_service_t)) {
                if (service->api && service->api->serialize_entity) {
                    return service->api->serialize_entity(entity_world.c_ptr(), serializer, entity);
                }
            } return {0};
        }

        static ecs_entity_t deserialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_serializer_service_t)) {
                if (service->api && service->api->deserialize_entity) {
                    return service->api->deserialize_entity(entity_world.c_ptr(), serializer, props);
                }
            } return 0;
        }
    };
}
#endif
