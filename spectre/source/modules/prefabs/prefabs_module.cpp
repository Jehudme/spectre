#include "prefabs_module.h"
#include "spectre/services/prefabs_service.h"
#include <iostream>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(prefabs_module_t, {
        .name = "prefabs",
        .description = "Prefabs Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_prefabs_service_t_info,
        .requirements = nullptr,
        .requirement_count = 0
    })

    prefabs_module_t::prefabs_module_t(flecs::world& world) : m_world(world) {
        std::cout << "[SPECTRE] Initializing " << "prefabs" << " module" << std::endl;
    }
    
    prefabs_module_t::~prefabs_module_t() = default;

    sandbox::properties prefabs_module_t::serialize_entity(flecs::entity entity) {
        return sandbox::properties{};
    }

    flecs::entity prefabs_module_t::deserialize_entity(sandbox::properties props) {
        return flecs::entity::null();
    }

    void prefabs_module_t::register_prefab(sandbox::properties props) {
    }

    bool prefabs_module_t::has_prefab(std::string_view name) const {
        return false;
    }

    bool prefabs_module_t::is_prefab(flecs::entity entity) const {
        return false;
    }

    flecs::entity prefabs_module_t::find_prefab(std::string_view name) {
        return flecs::entity::null();
    }

    flecs::entity prefabs_module_t::create_entity(sandbox::properties props) {
        return flecs::entity::null();
    }

    flecs::entity prefabs_module_t::create_entity(flecs::entity prefab) {
        return flecs::entity::null();
    }

    flecs::entity prefabs_module_t::create_entity(std::string_view name) {
        return flecs::entity::null();
    }

}
