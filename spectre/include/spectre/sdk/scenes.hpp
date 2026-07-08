#pragma once
#include <spectre/services/scenes_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class scenes {
    public:
        static sandbox_properties_handle_t serialize_state(const flecs::world& entity_world, ecs_entity_t state) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->serialize_state) {
                    return service->api->serialize_state(entity_world.c_ptr(), state);
                }
            } return {0};
        }

        static ecs_entity_t deserialize_state(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->deserialize_state) {
                    return service->api->deserialize_state(entity_world.c_ptr(), props);
                }
            } return 0;
        }

        static sandbox_properties_handle_t serialize_scene(const flecs::world& entity_world, ecs_entity_t scene) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->serialize_scene) {
                    return service->api->serialize_scene(entity_world.c_ptr(), scene);
                }
            } return {0};
        }

        static ecs_entity_t deserialize_scene(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->deserialize_scene) {
                    return service->api->deserialize_scene(entity_world.c_ptr(), props);
                }
            } return 0;
        }

        static void register_state(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->register_state) {
                    service->api->register_state(entity_world.c_ptr(), props);
                }
            }
        }

        static void register_scene(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->register_scene) {
                    service->api->register_scene(entity_world.c_ptr(), props);
                }
            }
        }

        static ecs_entity_t find_state(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->find_state) {
                    return service->api->find_state(entity_world.c_ptr(), name);
                }
            } return 0;
        }

        static ecs_entity_t find_scene(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->find_scene) {
                    return service->api->find_scene(entity_world.c_ptr(), name);
                }
            } return 0;
        }

        static bool has_state(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->has_state) {
                    return service->api->has_state(entity_world.c_ptr(), name);
                }
            } return false;
        }

        static bool has_scene(const flecs::world& entity_world, const char* name) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->has_scene) {
                    return service->api->has_scene(entity_world.c_ptr(), name);
                }
            } return false;
        }

        static bool is_state(const flecs::world& entity_world, ecs_entity_t entity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->is_state) {
                    return service->api->is_state(entity_world.c_ptr(), entity);
                }
            } return false;
        }

        static bool is_scene(const flecs::world& entity_world, ecs_entity_t entity) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->is_scene) {
                    return service->api->is_scene(entity_world.c_ptr(), entity);
                }
            } return false;
        }

        static ecs_entity_t find_current_state(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->find_current_state) {
                    return service->api->find_current_state(entity_world.c_ptr());
                }
            } return 0;
        }

        static ecs_query_t* find_current_scenes(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->find_current_scenes) {
                    return service->api->find_current_scenes(entity_world.c_ptr());
                }
            } return nullptr;
        }

        static void push_state(const flecs::world& entity_world, ecs_entity_t state) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->push_state) {
                    service->api->push_state(entity_world.c_ptr(), state);
                }
            }
        }

        static void pop_state(const flecs::world& entity_world) {
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_scenes_service_t)) {
                if (service->api && service->api->pop_state) {
                    service->api->pop_state(entity_world.c_ptr());
                }
            }
        }
    };
}
#endif
