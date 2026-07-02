#include "prefabs.h"
#include <sandbox/sdk/filesystem.hpp>
#include <sandbox/sdk/logs.hpp>
#include <stdexcept>

namespace spectre::module {

// ─────────────────────────────────────────────────────────────────────────────
// Helpers (declarations at top, implementations at bottom of file)
// ─────────────────────────────────────────────────────────────────────────────

    inline bool is_valid_prefab_name(const char* name);

// ─────────────────────────────────────────────────────────────────────────────
// MODULE LIFECYCLE
// ─────────────────────────────────────────────────────────────────────────────

    PrefabsModule::PrefabsModule(flecs::world& ecs) : m_entity_world(ecs) {
        sandbox::modules::logs::trace(m_entity_world, "Prefabs Module: Initializing...");

        const std::string prefabs_path = "app://state/prefabs.json";

        try {
            std::string json_content =
                sandbox::modules::filesystem::read_all_text(m_entity_world, prefabs_path.c_str());

            m_root_props = std::make_unique<sandbox::properties>(
                json_content, sandbox::properties::Format::JSON);

            auto prefabs_node = m_root_props->sub("prefabs");
            if (!prefabs_node.is_valid()) {
                sandbox::modules::logs::warn(m_entity_world,
                    "Prefabs Module: '{}' parsed successfully but has no 'prefabs' root key.", prefabs_path);
                return;
            }

            for (const auto& prefab_name : prefabs_node.keys("")) {
                PrefabTemplate prefab_template;
                prefab_template.name           = prefab_name;
                prefab_template.template_props = prefabs_node.sub(prefab_name.c_str());
                m_prefab_templates[prefab_name] = std::move(prefab_template);
            }

            sandbox::modules::logs::info(m_entity_world,
                "Prefabs Module: Loaded {} prefab(s) from '{}'.",
                m_prefab_templates.size(), prefabs_path);

        } catch (const std::exception& ex) {
            sandbox::modules::logs::error(m_entity_world,
                "Prefabs Module: Failed to load '{}': {}", prefabs_path, ex.what());
        }
    }

// ─────────────────────────────────────────────────────────────────────────────
// FACTORY REGISTRATION
// ─────────────────────────────────────────────────────────────────────────────

    void PrefabsModule::register_component_factory(std::string_view component_name,
                                                   spectre_component_factory_fn factory_fn) {
        if (component_name.empty()) {
            sandbox::modules::logs::warn(m_entity_world,
                "Prefabs Module: register_component_factory() called with empty component name — ignoring.");
            return;
        }
        if (!factory_fn) {
            sandbox::modules::logs::warn(m_entity_world,
                "Prefabs Module: register_component_factory('{}') — factory_fn is null — ignoring.",
                std::string(component_name));
            return;
        }

        const std::string key(component_name);
        if (m_component_factories.count(key)) {
            sandbox::modules::logs::warn(m_entity_world,
                "Prefabs Module: Component factory for '{}' already registered — overwriting.", key);
        }

        m_component_factories[key] = factory_fn;
    }

    bool PrefabsModule::has_component_factory(std::string_view component_name) const {
        return m_component_factories.count(std::string(component_name)) > 0;
    }

// ─────────────────────────────────────────────────────────────────────────────
// ENTITY CREATION
// ─────────────────────────────────────────────────────────────────────────────

    void PrefabsModule::apply_component(flecs::entity entity, std::string_view component_name,
                                        const sandbox::properties& component_props) {
        const std::string key(component_name);
        auto factory_it = m_component_factories.find(key);
        if (factory_it == m_component_factories.end()) {
            sandbox::modules::logs::warn(m_entity_world,
                "Prefabs Module: No factory registered for component '{}' — skipping.", key);
            return;
        }
        factory_it->second(m_entity_world.c_ptr(), entity.id(), component_props.get_raw());
    }

    void PrefabsModule::parse_entity_recursive(flecs::entity entity,
                                               const sandbox::properties& entity_props) {
        // Apply all components defined for this entity.
        auto components_node = entity_props.sub("components");
        if (components_node.is_valid()) {
            for (const auto& component_name : components_node.keys("")) {
                apply_component(entity, component_name,
                                components_node.sub(component_name.c_str()));
            }
        } else {
            sandbox::modules::logs::warn(m_entity_world,
                "Prefabs Module: Entity has no 'components' node.");
        }

        // Recursively create child entities.
        auto children_node = entity_props.sub("children");
        if (!children_node.is_valid()) return;

        for (const auto& child_name : children_node.keys("")) {
            flecs::entity child_entity = m_entity_world.prefab(child_name.c_str()).child_of(entity);
            parse_entity_recursive(child_entity, children_node.sub(child_name.c_str()));
        }
    }

    flecs::entity PrefabsModule::create_prefab(const std::string& prefab_name,
                                               const sandbox::properties& /*override_props*/) {
        auto template_it = m_prefab_templates.find(prefab_name);
        if (template_it == m_prefab_templates.end()) {
            sandbox::modules::logs::error(m_entity_world,
                "Prefabs Module: Prefab '{}' not found — was it defined in prefabs.json?", prefab_name);
            return flecs::entity::null();
        }

        flecs::entity new_entity = m_entity_world.prefab(prefab_name.c_str());
        parse_entity_recursive(new_entity, template_it->second.template_props);
        return new_entity;
    }

// ─────────────────────────────────────────────────────────────────────────────
// HELPERS
// ─────────────────────────────────────────────────────────────────────────────

    inline bool is_valid_prefab_name(const char* name) {
        return name != nullptr && name[0] != '\0';
    }

} // namespace spectre::module


// ─────────────────────────────────────────────────────────────────────────────
// C-ABI BRIDGE
// ─────────────────────────────────────────────────────────────────────────────

using spectre::module::PrefabsModule;

static PrefabsModule* get_module(ecs_world_t* ecs) {
    if (!ecs) return nullptr;
    return flecs::world(ecs).try_get_mut<PrefabsModule>();
}

static void abi_register_component_factory(ecs_world_t* ecs, const char* component_name,
                                           spectre_component_factory_fn factory_fn) {
    if (auto* module = get_module(ecs)) module->register_component_factory(component_name, factory_fn);
}

static bool abi_has_component_factory(ecs_world_t* ecs, const char* component_name) {
    if (auto* module = get_module(ecs)) return module->has_component_factory(component_name);
    return false;
}

static ecs_entity_t abi_create_prefab(ecs_world_t* ecs, const char* prefab_name,
                                      sandbox_properties_handle_t override_props) {
    if (!ecs || !prefab_name || prefab_name[0] == '\0') return 0;
    auto* module = get_module(ecs);
    if (!module) return 0;

    sandbox::properties props(override_props, false);
    return module->create_prefab(prefab_name, props).id();
}

static spectre_prefabs_api_t g_prefabs_api = {
    .register_component_factory = abi_register_component_factory,
    .has_component_factory      = abi_has_component_factory,
    .create_prefab              = abi_create_prefab,
};

SANDBOX_DEFINE_SERVICE(spectre_prefabs_service_t, spectre_prefabs_api_t, &g_prefabs_api)

// ─────────────────────────────────────────────────────────────────────────────
// MODULE REGISTRATION
// ─────────────────────────────────────────────────────────────────────────────

namespace spectre::module {

    static sandbox_requirement_info_t prefabs_requirements[] = {
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "logs",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "filesystem",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(PrefabsModule, {
        .name          = "prefabs",
        .description   = "Loads prefab definitions from JSON and spawns ECS entities from them.",
        .architecture  = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service       = &spectre_prefabs_service_t_info,
        .requirements  = prefabs_requirements,
        .requirement_count = 2
    })

} // namespace spectre::module
