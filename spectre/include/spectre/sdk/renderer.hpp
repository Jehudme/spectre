#pragma once
#include <spectre/services/renderer_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class renderer {
    public:
        static ecs_entity_t deserialize_renderer(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_renderer_deserialize_renderer(entity_world.c_ptr(), props);
        }

        static sandbox_properties_handle_t serialize_renderer(const flecs::world& entity_world, ecs_entity_t renderer) {
            return spectre_renderer_serialize_renderer(entity_world.c_ptr(), renderer);
        }

        static void register_renderer(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_renderer_register_renderer(entity_world.c_ptr(), props);}

        static bool is_renderer(const flecs::world& entity_world) {
            return spectre_renderer_is_renderer(entity_world.c_ptr());
        }
    };
}
#endif
