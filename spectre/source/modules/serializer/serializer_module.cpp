#include "serializer_module.h"

#include <format>

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
        .service = &spectre_serializer_service_t_info,
        .requirements = nullptr,
        .requirement_count = 0
    })

    serializer_module::serializer_module(flecs::world& world) : m_world(world) {
        m_serializer = m_world.entity("::serializer");
    }
    
    serializer_module::~serializer_module() = default;

    void serializer_module::register_serializer(std::string_view type, const serializer_t& serializer) {
        auto  entity = m_world.entity(type.data()).set<serializer_t>(serializer);

        m_serializer.child(entity);

    }

    bool serializer_module::has_serializer(std::string_view type) const {
        return find_serializer(type).is_valid();
    }

    flecs::entity serializer_module::find_serializer(std::string_view type) const {
        return m_serializer.lookup(type.data());
    }

    sandbox::properties serializer_module::serialize_entity(flecs::entity serializer, flecs::entity entity) {
        if (!serializer.is_valid() || !entity.is_valid()) return sandbox::properties{};
        const serializer_t* s = &serializer.get<serializer_t>();
        if (s && s->serialize) {
            return sandbox::properties{s->serialize(m_world.c_ptr(), entity.id())};
        }
        return sandbox::properties{};
    }

    flecs::entity serializer_module::deserialize_entity(flecs::entity serializer, const sandbox::properties& props) {
        if (!serializer.is_valid()) return flecs::entity::null();
        const serializer_t* s = &serializer.get<serializer_t>();
        if (s && s->deserialize) {
            return flecs::entity(m_world, s->deserialize(m_world.c_ptr(), props.get_raw()));
        }
        return flecs::entity::null();
    }

}
