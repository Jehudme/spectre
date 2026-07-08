#pragma once
#include <spectre/services/renderer_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class renderer {
    public:
        static ecs_entity_t deserialize_renderer(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_renderer_service_t)) {
                if (service->api && service->api->deserialize_renderer) {
                    return service->api->deserialize_renderer(entity_world.c_ptr(), props);
                }
            } return 0;
        }

        static sandbox_properties_handle_t serialize_renderer(const flecs::world& entity_world, ecs_entity_t renderer) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_renderer_service_t)) {
                if (service->api && service->api->serialize_renderer) {
                    return service->api->serialize_renderer(entity_world.c_ptr(), renderer);
                }
            } return {0};
        }

        static void register_renderer(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_renderer_service_t)) {
                if (service->api && service->api->register_renderer) {
                    service->api->register_renderer(entity_world.c_ptr(), props);
                }
            }
        }

        static bool is_renderer(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_renderer_service_t)) {
                if (service->api && service->api->is_renderer) {
                    return service->api->is_renderer(entity_world.c_ptr());
                }
            } return false;
        }
    };
}
#endif
