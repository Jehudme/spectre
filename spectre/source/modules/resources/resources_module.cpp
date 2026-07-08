#include "resources_module.h"
#include "spectre/services/resources_service.h"
#include <iostream>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(resource_module_t, {
        .name = "resources",
        .description = "Resources Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_resources_service_t_info,
        .requirements = nullptr,
        .requirement_count = 0
    })

    resource_module_t::resource_module_t(flecs::world& world) : m_world(world) {
        std::cout << "[SPECTRE] Initializing " << "resources" << " module" << std::endl;
    }
    
    resource_module_t::~resource_module_t() = default;

    flecs::entity resource_module_t::deserialize_resource(sandbox::properties props) {
        return flecs::entity::null();
    }

    sandbox::properties resource_module_t::serialize_resource(flecs::entity resourceEntity) {
        return sandbox::properties{};
    }

    void resource_module_t::register_resource_loader(std::string_view type, ResourceLoader loader) {
    }

    void resource_module_t::register_resource(sandbox::properties props) {
    }

    bool resource_module_t::has_resource_loader(std::string_view type) const {
        return false;
    }

    bool resource_module_t::has_resource(std::string_view name) const {
        return false;
    }

    bool resource_module_t::is_resource(flecs::entity entity) const {
        return false;
    }

    flecs::entity resource_module_t::find_resource_loader(std::string_view type) {
        return flecs::entity::null();
    }

    flecs::entity resource_module_t::find_resource(std::string_view name) {
        return flecs::entity::null();
    }

    bool resource_module_t::is_resource_loaded(flecs::entity resource) const {
        return false;
    }

    void resource_module_t::load_resource(flecs::entity resourceEntity) {
    }

    void resource_module_t::free_resource(flecs::entity resourceEntity) {
    }

}
