#pragma once
#include <flecs.h>
#include "sandbox/sdk/properties.hpp"
#include "sandbox/sdk/detail/bootstrapper.inl"

namespace spectre::modules {
    class prefabs_module_t {
    public:
        explicit prefabs_module_t(flecs::world& world);
        ~prefabs_module_t();

        prefabs_module_t(const prefabs_module_t&) = delete;
        prefabs_module_t& operator=(const prefabs_module_t&) = delete;
        prefabs_module_t(prefabs_module_t&&) = delete;
        prefabs_module_t& operator=(prefabs_module_t&&) = delete;

        sandbox::properties serialize_entity(flecs::entity entity);

        // We merge deserialize_entity and deserialize_entity_target functionality to directly inject components into the given entity
        flecs::entity deserialize_entity(sandbox::properties properties);
        flecs::entity deserialize_entity_target(flecs::entity target_entity, sandbox::properties properties);

        void register_prefab(std::string_view name, sandbox::properties props);
        bool has_prefab(std::string_view name) const;
        bool is_prefab(flecs::entity entity) const;
        flecs::entity find_prefab(std::string_view name);

        flecs::entity create_entity(sandbox::properties props);
        flecs::entity create_entity(flecs::entity prefab);
        flecs::entity create_entity(std::string_view name);

    private:
        flecs::world m_world;
        flecs::entity m_prefabs_root;
        flecs::entity m_entity_prefab;
        flecs::entity m_entity_serializer;
        flecs::entity m_script_args_serializer;
    };
}
