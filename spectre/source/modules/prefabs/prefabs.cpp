#include "prefabs.h"
#include <sandbox/sdk/filesystem.hpp>
#include <sandbox/sdk/logs.hpp>
#include <stdexcept>

namespace spectre::module {

    prefabs_module::prefabs_module(flecs::world& ecs) : m_entity_world(ecs) {
        sandbox::modules::logs::trace(m_entity_world, "Prefabs Module: Initializing...");

        try {
            std::string content = sandbox::modules::filesystem::read_all_text(m_entity_world, "app://state/prefabs.json");
            m_root_props = std::make_unique<sandbox::properties>(content, sandbox::properties::Format::JSON);
            
            auto prefabs_node = m_root_props->sub("prefabs");
            if (prefabs_node.is_valid()) {
                std::vector<std::string> keys = prefabs_node.keys("");

                for (const auto& key : keys) {
                    prefab_type_t pt;
                    pt.name = key;
                    pt.template_props = prefabs_node.sub(key.c_str());
                    m_prefabs_types[key] = std::move(pt);
                }
            }
            sandbox::modules::logs::info(m_entity_world, "Prefabs Module: Loaded {} prefabs from JSON", m_prefabs_types.size());
        } catch (const std::exception& e) {
            sandbox::modules::logs::error(m_entity_world, "Prefabs Module: Failed to load prefabs.json: {}", e.what());
        }
    }

    prefabs_module::~prefabs_module() {
    }

    void prefabs_module::register_component_factory(std::string_view name, spectre_component_factory_fn create_fn) {
        m_component_factories[std::string(name)] = create_fn;
    }

    bool prefabs_module::has_component_factory(std::string_view name) const {
        return m_component_factories.find(std::string(name)) != m_component_factories.end();
    }

    flecs::entity prefabs_module::create_component(flecs::entity entity, std::string_view name, const sandbox::properties& component_props) {
        auto it = m_component_factories.find(std::string(name));
        if (it != m_component_factories.end()) {
            it->second(m_entity_world.c_ptr(), entity.id(), component_props.get_raw());
        } else {
            sandbox::modules::logs::warn(m_entity_world, "Prefabs Module: Component factory for '{}' not found", std::string(name));
        }
        return entity;
    }

    void prefabs_module::parse_prefab_recursive(flecs::entity e, const sandbox::properties& props) {
        auto components_node = props.sub("components");
        if (components_node.is_valid()) {
            std::vector<std::string> keys = components_node.keys("");
            for (const auto& comp_name : keys) {
                create_component(e, comp_name, components_node.sub(comp_name.c_str()));
            }
        }
        
        auto children_node = props.sub("children");
        if (children_node.is_valid()) {
            std::vector<std::string> keys = children_node.keys("");
            for (const auto& child_name : keys) {
                flecs::entity child = m_entity_world.entity().child_of(e);
                parse_prefab_recursive(child, children_node.sub(child_name.c_str()));
            }
        }
    }

    flecs::entity prefabs_module::create_prefab(std::string name, const sandbox::properties& entity_props) {
        auto it = m_prefabs_types.find(name);
        if (it == m_prefabs_types.end()) {
            sandbox::modules::logs::error(m_entity_world, "Prefabs Module: Prefab '{}' not found", name);
            return flecs::entity::null();
        }

        flecs::entity e = m_entity_world.entity();
        parse_prefab_recursive(e, it->second.template_props);
        return e;
    }
}

using spectre::module::prefabs_module;

static void api_register_component_factory(ecs_world_t* ecs, const char* name, spectre_component_factory_fn create_fn) {
    if (!ecs) return;
    flecs::world w(ecs);
    if (auto* mod = w.try_get_mut<prefabs_module>()) {
        mod->register_component_factory(name, create_fn);
    }
}

static bool api_has_component_factory(ecs_world_t* ecs, const char* name) {
    if (!ecs) return false;
    flecs::world w(ecs);
    if (auto* mod = w.try_get_mut<prefabs_module>()) {
        return mod->has_component_factory(name);
    }
    return false;
}

static ecs_entity_t api_create_prefab(ecs_world_t* ecs, const char* name, sandbox_properties_handle_t entity_props) {
    if (!ecs) return 0;
    flecs::world w(ecs);
    if (auto* mod = w.try_get_mut<prefabs_module>()) {
        sandbox::properties props(entity_props, false);
        return mod->create_prefab(name, props).id();
    }
    return 0;
}

static spectre_prefabs_api_t g_prefabs_api = {
    .register_component_factory = api_register_component_factory,
    .has_component_factory = api_has_component_factory,
    .create_prefab = api_create_prefab
};

SANDBOX_DEFINE_SERVICE(spectre_prefabs_service_t, spectre_prefabs_api_t, &g_prefabs_api)

namespace spectre::module {

    static sandbox_requirement_info_t prefabs_requirements[] = {
        {
            .kind = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name = "logs",
            .architecture = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name = "filesystem",
            .architecture = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(prefabs_module, {
        .name = "prefabs",
        .description = "Prefabs Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .service = &spectre_prefabs_service_t_info,
        .requirements = prefabs_requirements,
        .requirement_count = 2
    })
}
