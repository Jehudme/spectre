#pragma once
#include <flecs.h>
#include <functional>

#include "sandbox/sdk/properties.hpp"
#include "sandbox/sdk/detail/bootstrapper.inl"
#include "spectre/components.h"

namespace spectre::modules {

    using component_serializer_t = spectre_component_serializer_t;
    using component_registration_fn = ecs_entity_t (*)(ecs_world_t* world);

    class components_module_t {
    public:
        explicit components_module_t(flecs::world& world);
        ~components_module_t();

        components_module_t(const components_module_t&) = delete;
        components_module_t& operator=(const components_module_t&) = delete;
        components_module_t(components_module_t&&) = delete;
        components_module_t& operator=(components_module_t&&) = delete;

        sandbox::properties serialize_component(flecs::entity entity);
        flecs::entity deserialize_component(std::string_view type, sandbox::properties props);

        void register_component(std::string_view type, component_registration_fn registration, spectre_serializer_component serializer);
        bool has_component(std::string_view type) const;
        bool is_component(flecs::entity entity) const;

    private:
        flecs::world m_world;
        flecs::entity m_prefabs_module;
    };
}
