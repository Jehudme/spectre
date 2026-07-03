#include "prefabs_module.h"
#include <sandbox/sdk/filesystem.hpp>
#include <sandbox/sdk/logs.hpp>
#include <stdexcept>
#include <algorithm>

namespace spectre::module {

    struct PrefabsModulePtr {
        PrefabsModule* ptr;
    };

    struct PrefabTemplateComponent {
        std::shared_ptr<sandbox::properties> props;
    };

    static bool is_prefab_empty(flecs::entity e) {
        bool has_custom = false;
        e.each([&](flecs::id id) {
            if (id == flecs::Prefab || id == flecs::Name) return;
            if (id == e.world().id<PrefabTemplateComponent>()) return;
            if (id.is_pair() && id.first() == flecs::ChildOf) return;
            has_custom = true;
        });
        return !has_custom;
    }

    static void apply_component(flecs::world& world, flecs::entity entity, const std::string& component_name,
                                const sandbox::properties& component_props, bool override_comp,
                                const std::unordered_map<std::string, spectre_component_factory_fn>& factories) {
        auto it = factories.find(component_name);
        if (it != factories.end()) {
            it->second(world.c_ptr(), entity.id(), component_props.get_raw());
            if (override_comp) {
                flecs::entity comp_entity = world.lookup(component_name.c_str());
                if (comp_entity.is_valid()) {
                    entity.auto_override(comp_entity);
                }
            }
        }
    }

    static void build_entity(flecs::world& world, flecs::entity entity, const sandbox::properties& props,
                             const std::unordered_map<std::string, spectre_component_factory_fn>& factories) {
        sandbox::properties components_props = props.sub("components");
        std::vector<std::string> auto_overrides;
        props.get_array("auto_overrides", auto_overrides);

        if (components_props.is_valid()) {
            for (const auto& comp_name : components_props.keys("")) {
                bool is_override = std::find(auto_overrides.begin(), auto_overrides.end(), comp_name) != auto_overrides.end();
                apply_component(world, entity, comp_name, components_props.sub(comp_name.c_str()), is_override, factories);
            }
        }

        sandbox::properties children_props = props.sub("children");
        if (children_props.is_valid()) {
            for (const auto& child_name : children_props.keys("")) {
                flecs::entity child_entity = entity.lookup(child_name.c_str());
                if (!child_entity.is_valid()) {
                    child_entity = world.entity(child_name.c_str()).child_of(entity);
                    if (entity.has(flecs::Prefab)) {
                        child_entity.add(flecs::Prefab);
                    }
                }
                build_entity(world, child_entity, children_props.sub(child_name.c_str()), factories);
            }
        }
    }

    PrefabsModule::PrefabsModule(flecs::world &ecs) : m_world(ecs) {
        m_world.set<PrefabsModulePtr>({this});
        
        sandbox::modules::logs::trace(m_world, "Prefabs Module: Initializing...");

        m_world.system("PrefabsStartupLoader")
            .kind(flecs::OnStart)
            .run([this](flecs::iter& /*iter*/) {
                const std::string prefabs_path = "app://state/prefabs.json";
                if (sandbox::modules::filesystem::exists(m_world, prefabs_path.c_str())) {
                    try {
                        std::string json_content = sandbox::modules::filesystem::read_all_text(m_world, prefabs_path.c_str());
                        sandbox::properties root_props(json_content, sandbox::properties::Format::JSON);
                        
                        auto prefabs_node = root_props.sub("prefabs");
                        if (prefabs_node.is_valid()) {
                            for (const auto& prefab_name : prefabs_node.keys("")) {
                                create_prefab(prefab_name.c_str(), prefabs_node.sub(prefab_name.c_str()));
                            }
                        }
                    } catch (const std::exception& ex) {
                        sandbox::modules::logs::error(m_world, "Prefabs Module: Failed to load '{}': {}", prefabs_path, ex.what());
                    }
                }
            });
    }

    void PrefabsModule::register_component_factory(std::string_view component_name, spectre_component_factory_fn factory_fn) {
        if (component_name.empty() || !factory_fn) return;
        m_component_factories[std::string(component_name)] = factory_fn;
    }

    bool PrefabsModule::has_component_factory(std::string_view component_name) const {
        return m_component_factories.count(std::string(component_name)) > 0;
    }

    flecs::entity PrefabsModule::create_prefab(const char* name, const sandbox::properties &props) {
        if (!name || name[0] == '\0') return flecs::entity::null();

        std::string global_name = name;
        if (global_name.rfind("::", 0) != 0) {
            global_name = "::" + global_name;
        }

        flecs::entity prefab_entity = m_world.lookup(global_name.c_str());
        
        if (!prefab_entity.is_valid()) {
            prefab_entity = m_world.prefab(global_name.c_str());
        }

        build_entity(m_world, prefab_entity, props, m_component_factories);
        return prefab_entity;
    }

    flecs::entity PrefabsModule::create_entity(const sandbox::properties& props) {
        flecs::entity instance = m_world.entity();

        std::string prefab_name = props.get<std::string>("prefab").value_or("");
        if (!prefab_name.empty()) {
            std::string global_name = prefab_name;
            if (global_name.rfind("::", 0) != 0) {
                global_name = "::" + global_name;
            }

            flecs::entity prefab_entity = m_world.lookup(global_name.c_str());
            
            if (!prefab_entity.is_valid()) {
                prefab_entity = m_world.prefab(global_name.c_str());
            }
            
            instance.is_a(prefab_entity);
        }

        build_entity(m_world, instance, props, m_component_factories);
        return instance;
    }

} // namespace spectre::module

// ─────────────────────────────────────────────────────────────────────────────
// C-ABI BRIDGE
// ─────────────────────────────────────────────────────────────────────────────

using spectre::module::PrefabsModule;

static PrefabsModule* get_module(ecs_world_t* ecs) {
    if (!ecs) return nullptr;
    if (const auto* wrapper = flecs::world(ecs).try_get<spectre::module::PrefabsModulePtr>()) {
        return wrapper->ptr;
    }
    return nullptr;
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

static ecs_entity_t abi_create_entity(ecs_world_t* ecs, sandbox_properties_handle_t props_handle) {
    if (!ecs) return 0;
    auto* module = get_module(ecs);
    if (!module) return 0;

    sandbox::properties props(props_handle, false);
    return module->create_entity(props).id();
}

static spectre_prefabs_api_t g_prefabs_api = {
    .register_component_factory = abi_register_component_factory,
    .has_component_factory      = abi_has_component_factory,
    .create_prefab              = abi_create_prefab,
    .create_entity              = abi_create_entity,
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
