#pragma once
#include "spectre/services/scripts_service.h"
#include "spectre/spectre.h"
#include <flecs.h>
#include <string>
#include <vector>

namespace spectre::modules {

class scripts {
  public:
    static bool has_script(const flecs::world& entity_world, const std::string& function_name,
                           const std::vector<spectre_script_argument_type_t>& arg_types) {
        return spectre_scripts_has_script(entity_world.c_ptr(), function_name.c_str(), arg_types.data(),
                                          arg_types.size());
    }

    static bool is_script(const flecs::world& entity_world, flecs::entity entity) {
        return spectre_scripts_is_script(entity_world.c_ptr(), entity.id());
    }

    static flecs::entity find_script(const flecs::world& entity_world, const std::string& function_name) {
        ecs_entity_t id = spectre_scripts_find_script(entity_world.c_ptr(), function_name.c_str());
        return flecs::entity(entity_world, id);
    }

    static void include_code(const flecs::world& entity_world, const std::string& path) {
        spectre_scripts_include_code(entity_world.c_ptr(), path.c_str());
    }

    static void execute_script(const flecs::world& entity_world, const std::string& function_name,
                               std::vector<spectre_script_argument_t>& args) {
        spectre_scripts_execute_script(entity_world.c_ptr(), function_name.c_str(), args.data(), args.size());
    }

    static sandbox::properties serialize_scripts(const flecs::world& entity_world, flecs::entity entity) {
        sandbox_properties_handle_t handle = spectre_scripts_serialize_scripts(entity_world.c_ptr(), entity.id());
        return sandbox::properties(handle, true);
    }

    static void deserialize_scripts(const flecs::world& entity_world, ecs_entity_t target,
                                    sandbox_properties_handle_t props) {
        spectre_scripts_deserialize_scripts(entity_world.c_ptr(), target, props);
    }

    static void execute_on_create(flecs::entity entity) {
        spectre_scripts_execute_on_create(entity.world().c_ptr(), entity.id());
    }

    static void execute_on_destroy(flecs::entity entity) {
        spectre_scripts_execute_on_destroy(entity.world().c_ptr(), entity.id());
    }

    static void execute_on_update(flecs::entity entity) {
        spectre_scripts_execute_on_update(entity.world().c_ptr(), entity.id());
    }

    static void execute_on_enter(flecs::entity entity) {
        spectre_scripts_execute_on_enter(entity.world().c_ptr(), entity.id());
    }

    static void execute_on_exit(flecs::entity entity) {
        spectre_scripts_execute_on_exit(entity.world().c_ptr(), entity.id());
    }
};

} // namespace spectre::modules
