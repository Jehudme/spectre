#pragma once
#include <spectre/services/renderer_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class renderer {
    public:
    static void deserialize_renderer(const flecs::world& entity_world, ecs_entity_t target, sandbox_properties_handle_t props);

    static sandbox_properties_handle_t serialize_renderer(const flecs::world& entity_world, ecs_entity_t renderer);

    static void register_renderer(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static bool is_renderer(const flecs::world& entity_world);
    };
}
#endif
