//
// Created by jehud on 2026-07-04.
//

#include "resources_module.h"

namespace spectre::modules {
    resource_module_t::resource_module_t(flecs::world &world) {
    }

    resource_module_t::~resource_module_t() {
    }

    flecs::entity resource_module_t::deserialize_resource(sandbox::properties props) {
    }

    sandbox::properties resource_module_t::serialize_resource(flecs::entity resourceEntity) {
    }

    void resource_module_t::register_resource_loader(std::string_view type, ResourceLoader loader) {
    }

    void resource_module_t::register_resource(sandbox::properties props) {
    }

    bool resource_module_t::has_resource_loader(std::string_view type) const {
    }

    bool resource_module_t::has_resource(std::string_view name) const {
    }

    bool resource_module_t::is_resource(flecs::entity entity) const {
    }

    flecs::entity resource_module_t::find_resource_loader(std::string_view type) {
    }

    flecs::entity resource_module_t::find_resource(std::string_view name) {
    }

    bool resource_module_t::is_resource_loaded(flecs::entity resource) const {
    }

    void resource_module_t::load_resource(flecs::entity resourceEntity) {
    }

    void resource_module_t::free_resource(flecs::entity resourceEntity) {
    }
}
