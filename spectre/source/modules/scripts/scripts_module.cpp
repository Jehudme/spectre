//
// Created by jehud on 2026-07-04.
//

#include "scripts_module.h"

namespace spectre::modules {

    static inline sandbox::properties serialize_script(flecs::world world, flecs::entity script_entity) {
        const script_t& script = script_entity.get<script_t>();
        sandbox::properties props(sandbox_properties_create(), false);

        props.set("name/entry_point", script.function_name);

        return props;
    }

    static inline flecs::entity deserialize_script(flecs::world world, sandbox::properties props) {

    }

    script_module_t::script_module_t(flecs::world &world) : m_world(world) {
        // Register script serialization in the serializer module
        // store the serializer compoenent
        // Instantiate the lua_state
        // Include all lua code from the app://state/scripts folder (use filesystem module)
        // Register all scripts into m_scripts from app://state/scripts.json
        // Verify that function_name exist the lua_state, if not logs an error and do not add it to m_scripts
    }

    script_module_t::~script_module_t() {
        // free the lua_state
    }

    sandbox::properties script_module_t::serialize_script(flecs::entity script_module) {
        // Call serialize from serializer module with as m_
    }

    flecs::entity script_module_t::deserialize_script(sandbox::properties props) {
    }

    void script_module_t::register_script(sandbox::properties props) {
        // deserialize using the this->deserialize_script function
    }

    bool script_module_t::has_script(std::string_view name) const {
    }

    bool script_module_t::is_script(flecs::entity entity) const {
        // Check if the entity has the script component
    }

    flecs::entity script_module_t::find_script(std::string_view name) {
    }

    void script_module_t::include_code(std::string_view path) {
        // Load the lua code from the given path with filesystem module and include it into the lua_state
    }

    void script_module_t::execute_script(flecs::entity script_entity, script_arguments_t args) {
        // run the function from lua state from the function_name inside of scriipt_entity
    }
}
