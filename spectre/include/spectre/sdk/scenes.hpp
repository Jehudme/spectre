#pragma once
#include <spectre/services/scenes_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class scenes {
    public:
    static sandbox_properties_handle_t serialize_state(const flecs::world& entity_world, ecs_entity_t state);

    static ecs_entity_t deserialize_state(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static sandbox_properties_handle_t serialize_scene(const flecs::world& entity_world, ecs_entity_t scene);

    static ecs_entity_t deserialize_scene(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static void register_state(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static void register_scene(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static ecs_entity_t find_state(const flecs::world& entity_world, const char* name);

    static ecs_entity_t find_scene(const flecs::world& entity_world, const char* name);

    static bool has_state(const flecs::world& entity_world, const char* name);

    static bool has_scene(const flecs::world& entity_world, const char* name);

    static bool is_state(const flecs::world& entity_world, ecs_entity_t entity);

    static bool is_scene(const flecs::world& entity_world, ecs_entity_t entity);

    static ecs_entity_t find_current_state(const flecs::world& entity_world);

    static ecs_query_t* find_current_scenes(const flecs::world& entity_world);

    static void push_state(const flecs::world& entity_world, ecs_entity_t state);

    static void pop_state(const flecs::world& entity_world);
    };
}
#endif
