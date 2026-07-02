#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <flecs.h>
#include <sandbox/sdk/properties.hpp>
#include <spectre/abi/prefabs_service.h>

namespace spectre::module {

    struct prefab_type_t {
        std::string name;
        sandbox_properties_handle_t template_props;
    };

    class prefabs_module {
    public:
        prefabs_module(flecs::world& ecs);
        ~prefabs_module();

        void register_component_factory(std::string_view name, spectre_component_factory_fn create_fn);
        bool has_component_factory(std::string_view name) const;

        flecs::entity create_component(flecs::entity entity, std::string_view name, const sandbox::properties& component_props);
        flecs::entity create_prefab(std::string name, const sandbox::properties& entity_props);

    private:
        void parse_prefab_recursive(flecs::entity e, const sandbox::properties& props);

        std::unique_ptr<sandbox::properties> m_root_props;
        flecs::world m_entity_world;
        std::unordered_map<std::string, prefab_type_t> m_prefabs_types;
        std::unordered_map<std::string, spectre_component_factory_fn> m_component_factories;
    };
}
