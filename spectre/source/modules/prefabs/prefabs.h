#pragma once

#include <spectre/abi/prefabs_service.h>
#include <sandbox/sdk/properties.hpp>
#include <flecs.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace spectre::module {

    struct PrefabTemplate {
        std::string name;
        sandbox::properties template_props;
    };

    class PrefabsModule {
    public:
        explicit PrefabsModule(flecs::world& ecs);
        ~PrefabsModule() = default;

        void register_component_factory(std::string_view component_name,
                                        spectre_component_factory_fn factory_fn);
        bool has_component_factory(std::string_view component_name) const;

        flecs::entity create_prefab(std::string_view prefab_name,
                                    const sandbox::properties& override_props);

    private:
        void apply_component(flecs::entity entity, std::string_view component_name, 
                             const sandbox::properties& component_props, bool override_comp);

        flecs::world m_world;
        std::unique_ptr<sandbox::properties> m_root_props;
        std::unordered_map<std::string, PrefabTemplate> m_prefab_templates;
        std::unordered_map<std::string, spectre_component_factory_fn> m_component_factories;
    };

} // namespace spectre::module
