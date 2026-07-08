#pragma once
#include <spectre/services/scripts_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class scripts {
    public:
    static sandbox_properties_handle_t serialize_script(const flecs::world& entity_world, ecs_entity_t script);

    static ecs_entity_t deserialize_script(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static void register_script(const flecs::world& entity_world, sandbox_properties_handle_t props);

    static bool has_script(const flecs::world& entity_world, const char* name);

    static bool is_script(const flecs::world& entity_world, ecs_entity_t entity);

    static ecs_entity_t find_script(const flecs::world& entity_world, const char* name);

    static void include_code(const flecs::world& entity_world, const char* path);

    static void execute_script(const flecs::world& entity_world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count);
    };
}
#endif
