#pragma once

#include <spectre/abi/prefabs_service.h>
#include <sandbox/sdk/properties.hpp>
#include <flecs.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace spectre::module {

    /**
     * @brief Holds the parsed template data for a single prefab definition.
     *        The `template_props` must outlive any entity created from this prefab,
     *        which is why it is owned by `m_root_props` on the module rather than
     *        being a value copy.
     */
    struct PrefabTemplate {
        std::string        name;
        sandbox::properties template_props;
    };

    /**
     * @brief Loads prefab definitions from app://state/prefabs.json and spawns
     *        entities from them on demand.
     *
     * Other modules (e.g. RendererModule) register component factory functions
     * before creating prefab instances. Each factory receives the raw JSON
     * properties for that component and is responsible for attaching the right
     * ECS component to the entity.
     *
     * Prefab hierarchy is supported via a "children" object in the JSON:
     *   { "children": { "child_name": { "components": { ... } } } }
     */
    class PrefabsModule {
    public:
        explicit PrefabsModule(flecs::world& ecs);
        ~PrefabsModule() = default;

        // ── Factory Registration ───────────────────────────────────────────────
        /** Register a factory function that creates a component by name. */
        void register_component_factory(std::string_view component_name,
                                        spectre_component_factory_fn factory_fn);

        /** Returns true if a factory for the given component name is registered. */
        bool has_component_factory(std::string_view component_name) const;

        // ── Entity Creation ────────────────────────────────────────────────────
        /**
         * @brief Instantiates an entity from the prefab named `prefab_name`.
         *        Returns a null entity if the prefab is not found.
         *        Override properties can be passed via `override_props` (unused placeholder).
         */
        flecs::entity create_prefab(const std::string& prefab_name,
                                    const sandbox::properties& override_props);

    private:
        void apply_component(flecs::entity entity, std::string_view component_name,
                             const sandbox::properties& component_props);
        void parse_entity_recursive(flecs::entity entity, const sandbox::properties& entity_props);

        flecs::world m_entity_world;
        std::unique_ptr<sandbox::properties> m_root_props;
        std::unordered_map<std::string, PrefabTemplate>                m_prefab_templates;
        std::unordered_map<std::string, spectre_component_factory_fn>  m_component_factories;
    };

} // namespace spectre::module
