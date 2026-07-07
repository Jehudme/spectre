#pragma once
#include "flecs/addons/cpp/entity.hpp"
#include "flecs/addons/cpp/world.hpp"
#include "sandbox/sdk/properties.hpp"
#include "sandbox/sdk/detail/bootstrapper.inl"

namespace spectre::modules {
    class components_module_t {
    public:
        explicit components_module_t(flecs::world& world);
        ~components_module_t();

        components_module_t(const components_module_t&) = delete;
        components_module_t& operator=(const components_module_t&) = delete;
        components_module_t(components_module_t&&) = delete;
        components_module_t& operator=(components_module_t&&) = delete;

        sandbox::properties serialize_entity(flecs::entity entity);
        flecs::entity deserialize_entity(sandbox::properties props);

        void register_prefab(sandbox::properties props);
        bool has_prefab(std::string_view name) const;
        bool is_prefab(flecs::entity entity) const;
        flecs::entity find_prefab(std::string_view name);

        flecs::entity create_entity(sandbox::properties props);
        flecs::entity create_entity(flecs::entity prefab);
        flecs::entity create_entity(std::string_view name);

    private:
        flecs::world m_world;
        flecs::entity m_prefabs_module;
        flecs::entity m_entity_serializer;
    };
}
