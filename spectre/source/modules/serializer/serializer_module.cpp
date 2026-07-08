#include "serializer_module.h"
#include "spectre/services/serializer_service.h"
#include <iostream>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(serializer_module, {
        .name = "serializer",
        .description = "Serializer Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_serializer_service_t_info_decl,
        .requirements = nullptr,
        .requirement_count = 0
    })

    serializer_module::serializer_module(flecs::world& world) : m_world(world) {
        std::cout << "[SPECTRE] Initializing " << "serializer" << " module" << std::endl;
    }
    
    serializer_module::~serializer_module() = default;

    void serializer_module::register_serializer(std::string_view type, const serializer_t& serializer) {
    }

    bool serializer_module::has_serializer(std::string_view type) const {
        return false;
    }

    flecs::entity serializer_module::find_serializer(std::string_view type) const {
        return flecs::entity::null();
    }

    sandbox::properties serializer_module::serialize_entity(flecs::entity serializer, flecs::entity entity) {
        return sandbox::properties{};
    }

    flecs::entity serializer_module::deserialize_entity(flecs::entity serializer, const sandbox::properties& props) {
        return flecs::entity::null();
    }

}
