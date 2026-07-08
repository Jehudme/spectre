#include "prefabs_module.h"
#include "spectre/services/prefabs_service.h"
#include <iostream>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(components_module_t, {
        .name = "prefabs",
        .description = "Prefabs Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_prefabs_service_t_info_decl,
        .requirements = nullptr,
        .requirement_count = 0
    })

    components_module_t::components_module_t(flecs::world& world) : m_world(world) {
        std::cout << "[SPECTRE] Initializing " << "prefabs" << " module" << std::endl;
    }
    
    components_module_t::~components_module_t() = default;

    sandbox::properties components_module_t::serialize_entity(flecs::entity entity) {
        return sandbox::properties{};
    }

    flecs::entity components_module_t::deserialize_entity(sandbox::properties props) {
        return flecs::entity::null();
    }

    void components_module_t::register_prefab(sandbox::properties props) {
    }

    bool components_module_t::has_prefab(std::string_view name) const {
        return false;
    }

    bool components_module_t::is_prefab(flecs::entity entity) const {
        return false;
    }

    flecs::entity components_module_t::find_prefab(std::string_view name) {
        return flecs::entity::null();
    }

    flecs::entity components_module_t::create_entity(sandbox::properties props) {
        return flecs::entity::null();
    }

    flecs::entity components_module_t::create_entity(flecs::entity prefab) {
        return flecs::entity::null();
    }

    flecs::entity components_module_t::create_entity(std::string_view name) {
        return flecs::entity::null();
    }

}
