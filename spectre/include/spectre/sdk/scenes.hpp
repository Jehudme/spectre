#pragma once
#include <spectre/services/scenes_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class scenes {
    public:
        static sandbox_properties_handle_t serialize_state(const flecs::world& entity_world, ecs_entity_t state) {
            return spectre_scenes_serialize_state(entity_world.c_ptr(), state);
        }

        static ecs_entity_t deserialize_state(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_scenes_deserialize_state(entity_world.c_ptr(), props);
        }

        static sandbox_properties_handle_t serialize_scene(const flecs::world& entity_world, ecs_entity_t scene) {
            return spectre_scenes_serialize_scene(entity_world.c_ptr(), scene);
        }

        static ecs_entity_t deserialize_scene(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_scenes_deserialize_scene(entity_world.c_ptr(), props);
        }

        static void register_state(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_scenes_register_state(entity_world.c_ptr(), props);}

        static void register_scene(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_scenes_register_scene(entity_world.c_ptr(), props);}

        static ecs_entity_t find_state(const flecs::world& entity_world, const char* name) {
            return spectre_scenes_find_state(entity_world.c_ptr(), name);
        }

        static ecs_entity_t find_scene(const flecs::world& entity_world, const char* name) {
            return spectre_scenes_find_scene(entity_world.c_ptr(), name);
        }

        static bool has_state(const flecs::world& entity_world, const char* name) {
            return spectre_scenes_has_state(entity_world.c_ptr(), name);
        }

        static bool has_scene(const flecs::world& entity_world, const char* name) {
            return spectre_scenes_has_scene(entity_world.c_ptr(), name);
        }

        static bool is_state(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_scenes_is_state(entity_world.c_ptr(), entity);
        }

        static bool is_scene(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_scenes_is_scene(entity_world.c_ptr(), entity);
        }

        static ecs_entity_t find_current_state(const flecs::world& entity_world) {
            return spectre_scenes_find_current_state(entity_world.c_ptr());
        }

        static ecs_query_t* find_current_scenes(const flecs::world& entity_world) {
            return spectre_scenes_find_current_scenes(entity_world.c_ptr());
        }

        static void push_state(const flecs::world& entity_world, ecs_entity_t state) {
            spectre_scenes_push_state(entity_world.c_ptr(), state);}

        static void pop_state(const flecs::world& entity_world) {
            spectre_scenes_pop_state(entity_world.c_ptr());}
    };
}
#endif
