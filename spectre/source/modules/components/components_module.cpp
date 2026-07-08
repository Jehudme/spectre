#include "components_module.h"

namespace spectre::modules {

    components_module_t::components_module_t(flecs::world& world) : m_world(world) {
        
    }

    components_module_t::~components_module_t() {
        
    }

    sandbox::properties components_module_t::serialize_component(flecs::entity entity) {
        // TODO: Implement
        return sandbox::properties{};
    }

    flecs::entity components_module_t::deserialize_component(std::string_view type, sandbox::properties props) {
        // TODO: Implement
        return flecs::entity::null();
    }

    void components_module_t::register_component(std::string_view type, component_registration_fn registration, spectre_serializer_component serializer) {
        // TODO: Implement
    }

    bool components_module_t::has_component(std::string_view type) const {
        // TODO: Implement
        return false;
    }

    bool components_module_t::is_component(flecs::entity entity) const {
        // TODO: Implement
        return false;
    }

}
