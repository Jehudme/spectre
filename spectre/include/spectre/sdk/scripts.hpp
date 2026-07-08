#pragma once
#include <spectre/services/scripts_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class scripts {
    public:
        static sandbox_properties_handle_t serialize_script(const flecs::world& entity_world, ecs_entity_t script) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scripts_service_t)) {
                if (service->api && service->api->serialize_script) {
                    return service->api->serialize_script(entity_world.c_ptr(), script);
                }
            } return {0};
        }

        static ecs_entity_t deserialize_script(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scripts_service_t)) {
                if (service->api && service->api->deserialize_script) {
                    return service->api->deserialize_script(entity_world.c_ptr(), props);
                }
            } return 0;
        }

        static void register_script(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scripts_service_t)) {
                if (service->api && service->api->register_script) {
                    service->api->register_script(entity_world.c_ptr(), props);
                }
            }
        }

        static bool has_script(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scripts_service_t)) {
                if (service->api && service->api->has_script) {
                    return service->api->has_script(entity_world.c_ptr(), name);
                }
            } return false;
        }

        static bool is_script(const flecs::world& entity_world, ecs_entity_t entity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scripts_service_t)) {
                if (service->api && service->api->is_script) {
                    return service->api->is_script(entity_world.c_ptr(), entity);
                }
            } return false;
        }

        static ecs_entity_t find_script(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scripts_service_t)) {
                if (service->api && service->api->find_script) {
                    return service->api->find_script(entity_world.c_ptr(), name);
                }
            } return 0;
        }

        static void include_code(const flecs::world& entity_world, const char* path) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scripts_service_t)) {
                if (service->api && service->api->include_code) {
                    service->api->include_code(entity_world.c_ptr(), path);
                }
            }
        }

        static void execute_script(const flecs::world& entity_world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scripts_service_t)) {
                if (service->api && service->api->execute_script) {
                    service->api->execute_script(entity_world.c_ptr(), script_entity, args, arg_count);
                }
            }
        }
    };
}
#endif
