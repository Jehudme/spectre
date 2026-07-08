#pragma once
#include <spectre/services/components_service.h>
#include <sandbox/sdk/properties.hpp>
#include <flecs.h>
#include <string_view>

namespace spectre::modules {
    class components {
    public:
        static sandbox::properties serialize_component(const flecs::world& entity_world, flecs::entity entity);
        static flecs::entity deserialize_component(const flecs::world& entity_world, std::string_view type, sandbox::properties props);
        static void register_component(const flecs::world& entity_world, std::string_view type, spectre_component_registration_fn_t registration, spectre_serializer_component serializer);
        static bool has_component(const flecs::world& entity_world, std::string_view type);
        static bool is_component(const flecs::world& entity_world, flecs::entity entity);
    };
}
