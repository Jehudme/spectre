#pragma once
#include <spectre/services/scripts_service.h>

#ifdef __cplusplus
namespace spectre::modules {
    class scripts {
    public:
        static sandbox_properties_handle_t serialize_script(const flecs::world& entity_world, ecs_entity_t script) {
            return spectre_scripts_serialize_script(entity_world.c_ptr(), script);
        }

        static ecs_entity_t deserialize_script(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            return spectre_scripts_deserialize_script(entity_world.c_ptr(), props);
        }

        static void register_script(const flecs::world& entity_world, sandbox_properties_handle_t props) {
            spectre_scripts_register_script(entity_world.c_ptr(), props);}

        static bool has_script(const flecs::world& entity_world, const char* name) {
            return spectre_scripts_has_script(entity_world.c_ptr(), name);
        }

        static bool is_script(const flecs::world& entity_world, ecs_entity_t entity) {
            return spectre_scripts_is_script(entity_world.c_ptr(), entity);
        }

        static ecs_entity_t find_script(const flecs::world& entity_world, const char* name) {
            return spectre_scripts_find_script(entity_world.c_ptr(), name);
        }

        static void include_code(const flecs::world& entity_world, const char* path) {
            spectre_scripts_include_code(entity_world.c_ptr(), path);}

        static void execute_script(const flecs::world& entity_world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count) {
            spectre_scripts_execute_script(entity_world.c_ptr(), script_entity, args, arg_count);}
    };
}
#endif
