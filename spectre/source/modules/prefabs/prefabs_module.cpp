#include "prefabs_module.h"
#include <sandbox/sdk/filesystem.hpp>
#include <sandbox/sdk/logs.hpp>
#include <stdexcept>
#include <algorithm>

namespace spectre::module {

    PrefabsModule::PrefabsModule(flecs::world &ecs) : m_world(ecs) {
        sandbox::modules::logs::trace(m_world, "Prefabs Module: Initializing...");

        const std::string prefabs_path = "app://state/prefabs.json";
        try {
            std::string json_content =
                sandbox::modules::filesystem::read_all_text(m_world, prefabs_path.c_str());

            m_root_props = std::make_unique<sandbox::properties>(
                json_content, sandbox::properties::Format::JSON);

            auto prefabs_node = m_root_props->sub("prefabs");
            if (!prefabs_node.is_valid()) {
                sandbox::modules::logs::warn(m_world,
                    "Prefabs Module: '{}' parsed successfully but has no 'prefabs' root key.", prefabs_path);
                return;
            }

            // PASS 1: Create all prefab entities so they exist in Flecs before any children are added.
            for (const auto& prefab_name : prefabs_node.keys("")) {
                m_world.prefab(prefab_name.c_str());

                PrefabTemplate prefab_template;
                prefab_template.name           = prefab_name;
                prefab_template.template_props = prefabs_node.sub(prefab_name.c_str());
                m_prefab_templates[prefab_name] = std::move(prefab_template);
            }

            sandbox::modules::logs::info(m_world,
                "Prefabs Module: Loaded {} prefab(s) from '{}'.",
                m_prefab_templates.size(), prefabs_path);

        } catch (const std::exception& ex) {
            sandbox::modules::logs::error(m_world,
                "Prefabs Module: Failed to load '{}': {}", prefabs_path, ex.what());
        }
    }

    void PrefabsModule::register_component_factory(std::string_view component_name, spectre_component_factory_fn factory_fn) {
        if (component_name.empty() || !factory_fn) return;
        m_component_factories[std::string(component_name)] = factory_fn;
    }

    bool PrefabsModule::has_component_factory(std::string_view component_name) const {
        return m_component_factories.count(std::string(component_name)) > 0;
    }

    // Recursively build a prefab and its inline children.
    void PrefabsModule::build_prefab_hierarchy(flecs::entity prefab_entity, const sandbox::properties& props) {
        sandbox::properties components_props = props.sub("components");

        std::vector<std::string> overrides_components;
        props.get_array("auto_overrides", overrides_components);

        if (components_props.is_valid()) {
            for (const auto& comp_name : components_props.keys("")) {
                bool is_override = std::find(overrides_components.begin(), overrides_components.end(), comp_name) != overrides_components.end();
                apply_component(prefab_entity, comp_name, components_props.sub(comp_name.c_str()), is_override);
            }
        }

        sandbox::properties children_props = props.sub("children");
        if (children_props.is_valid()) {
            for (const auto& child_name : children_props.keys("")) {
                flecs::entity child_entity = prefab_entity.lookup(child_name.c_str());
                if (!child_entity.is_valid()) {
                    std::string child_path = std::string(prefab_entity.path()) + "::" + child_name;
                    child_entity = m_world.prefab(child_path.c_str());
                }
                
                // Recursively build the child's components
                build_prefab_hierarchy(child_entity, children_props.sub(child_name.c_str()));
            }
        }
    }

    flecs::entity PrefabsModule::create_prefab(std::string_view prefab_name, const sandbox::properties &override_props) {
        auto template_it = m_prefab_templates.find(std::string(prefab_name));
        if (template_it == m_prefab_templates.end()) {
            sandbox::modules::logs::error(m_world, "Prefabs Module: Prefab '{}' not found in templates.", prefab_name);
            return flecs::entity::null();
        }

        flecs::entity prefab_template = m_world.prefab(prefab_name.data());

        // PASS 2: Lazy evaluation. We only apply components once per prefab template.
        if (!template_it->second.is_built) {
            template_it->second.is_built = true;
            build_prefab_hierarchy(prefab_template, template_it->second.template_props);
        }

        // Instantiate the entity from the template using is_a
        flecs::entity instance = m_world.entity().is_a(prefab_template);

        // Apply any overrides directly to the instance
        if (override_props.is_valid()) {
             build_prefab_hierarchy(instance, override_props);
        }

        return instance;
    }

    void PrefabsModule::apply_component(flecs::entity prefab_entity, std::string_view component_name, const sandbox::properties &component_props, bool override_comp) {
        const std::string key(component_name);
        auto factory_it = m_component_factories.find(key);
        if (factory_it == m_component_factories.end()) return;

        factory_it->second(m_world.c_ptr(), prefab_entity.id(), component_props.get_raw());

        if (override_comp) {
            flecs::entity comp_entity = m_world.lookup(key.c_str());
            if (comp_entity.is_valid()) {
                prefab_entity.auto_override(comp_entity);
            }
        }
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
                                      sandbox_properties_handle_t prefab_props_handle) {
    if (!ecs || !prefab_name || prefab_name[0] == '\0') return 0;
    auto* module = get_module(ecs);
    if (!module) return 0;

    sandbox::properties props(prefab_props_handle, false);
    return module->create_prefab(prefab_name, props).id();
}

static spectre_prefabs_api_t g_prefabs_api = {
    .register_component_factory = abi_register_component_factory,
    .has_component_factory      = abi_has_component_factory,
    .create_prefab              = abi_create_prefab,
};

SANDBOX_DEFINE_SERVICE(spectre_prefabs_service_t, spectre_prefabs_api_t, &g_prefabs_api)

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
        .description   = "Spawns ECS prefabs entities with components and overrides.",
        .architecture  = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service       = &spectre_prefabs_service_t_info,
        .requirements  = prefabs_requirements,
        .requirement_count = 2
    })

} // namespace spectre::module
