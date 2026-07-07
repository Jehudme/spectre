//
// Created by jehud on 2026-07-04.
//

#include "prefabs_module.h"

namespace spectre::modules {
    components_module_t::components_module_t(flecs::world &world) {
    }

    components_module_t::~components_module_t() {
    }

    sandbox::properties components_module_t::serialize_entity(flecs::entity entity) {
    }

    flecs::entity components_module_t::deserialize_entity(sandbox::properties props) {
    }

    void components_module_t::register_prefab(sandbox::properties props) {
    }

    bool components_module_t::has_prefab(std::string_view name) const {
    }

    bool components_module_t::is_prefab(flecs::entity entity) const {
    }

    flecs::entity components_module_t::find_prefab(std::string_view name) {
    }

    flecs::entity components_module_t::create_entity(sandbox::properties props) {
    }

    flecs::entity components_module_t::create_entity(flecs::entity prefab) {
    }

    flecs::entity components_module_t::create_entity(std::string_view name) {
    }
}
