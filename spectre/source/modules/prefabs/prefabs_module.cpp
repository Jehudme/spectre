#include "prefabs_module.h"
#include <sandbox/sdk/filesystem.hpp>
#include <sandbox/sdk/logs.hpp>
#include <stdexcept>
#include <algorithm>
#include <spectre/abi/scripts_components.h>
#include <cstring>

namespace spectre::module {

    // Component factory storage in the translation unit
    static std::unordered_map<std::string, spectre_component_factory_fn> s_component_factories;

    std::string PrefabsModule::ensure_global_name(const std::string& name) {
        if (name.empty()) return name;
        if (name.starts_with("::")) return name;
        return "::" + name;
    }

    void PrefabsModule::apply_component(flecs::world& world, flecs::entity entity, const std::string& component_name, const sandbox::properties& component_props) {
        auto it = s_component_factories.find(component_name);
        if (it != s_component_factories.end()) {
            it->second(world.c_ptr(), entity.id(), component_props.get_raw());
        } else {
            sandbox::modules::logs::warn(world, "Prefabs Module: Unknown component '{}' ignored.", component_name);
        }
    }

    void PrefabsModule::build_entity(flecs::world& world, flecs::entity entity, const sandbox::properties& props) {
        sandbox::properties components_props = props.sub("components");
        if (components_props.is_valid()) {
            for (const auto& comp_name : components_props.keys("")) {
                apply_component(world, entity, comp_name, components_props.sub(comp_name.c_str()));
            }
        }
        sandbox::properties scripts_props = props.sub("scripts");
        if (scripts_props.is_valid()) {
            std::vector<std::string> on_update_scripts;
            scripts_props.get_array<std::string>("on_update", on_update_scripts);
            for (const auto& func : on_update_scripts) {
                spectre_script_t script = {};
                script.target_entity = 0;
                script.function_name = ecs_os_strdup(func.c_str());
                entity.set<spectre_script_t>(script);
                entity.add<spectre_script_on_update_t>();
            }
        }

        sandbox::properties children_props = props.sub("children");
        if (children_props.is_valid()) {
            for (const auto& key : children_props.keys("")) {
                sandbox::properties child_props = children_props.sub(key);
                // Use 'name' field if present, otherwise fall back to the object key
                std::string child_name = child_props.get<std::string>("name").value_or(key);
                if (child_name.empty()) {
                    sandbox::modules::logs::warn(world, "Prefabs Module: Child missing 'name' property, ignoring.");
                    continue;
                }

                flecs::entity child_entity = entity.lookup(child_name.c_str());
                if (!child_entity.is_valid()) {
                    child_entity = world.entity(child_name.c_str()).child_of(entity);
                    if (entity.has(flecs::Prefab)) {
                        child_entity.add(flecs::Prefab);
                    }
                }

                std::string child_prefab = child_props.get<std::string>("prefab").value_or("");
                if (!child_prefab.empty()) {
                    std::string global_name = ensure_global_name(child_prefab);
                    flecs::entity child_prefab_entity = world.lookup(global_name.c_str());
                    if (!child_prefab_entity.is_valid()) {
                        child_prefab_entity = world.prefab(global_name.c_str());
                    }
                    child_entity.is_a(child_prefab_entity);
                }

                build_entity(world, child_entity, child_props);
            }
        }
    }

    PrefabsModule::PrefabsModule(flecs::world &ecs) : m_world(ecs) {
        sandbox::modules::logs::trace(m_world, "Prefabs Module: Initializing...");

        m_world.system("PrefabsStartupLoader")
            .kind(flecs::OnStart)
            .run([this](flecs::iter& /*iter*/) {
                sandbox::modules::logs::info(m_world, "PrefabsModule: Startup loader running.");
                const std::string prefabs_path = "app://state/prefabs.json";
                if (sandbox::modules::filesystem::exists(m_world, prefabs_path.c_str())) {
                    sandbox::modules::logs::info(m_world, "PrefabsModule: Found {}.", prefabs_path);
                    try {
                        std::string json_content = sandbox::modules::filesystem::read_all_text(m_world, prefabs_path.c_str());
                        sandbox::properties root_props(json_content, sandbox::properties::Format::JSON);
                        
                        // Root of prefabs.json is an array of prefabs
                        if (root_props.is_valid()) {
                            auto keys = root_props.keys("");
                            sandbox::modules::logs::info(m_world, "PrefabsModule: Found {} prefabs in file.", keys.size());
                            for (const auto& key : keys) {
                                sandbox::properties prefab_props = root_props.sub(key);
                                std::string prefab_name = prefab_props.get<std::string>("name").value_or("");
                                if (!prefab_name.empty()) {
                                    sandbox::modules::logs::info(m_world, "PrefabsModule: Creating prefab '{}'.", prefab_name);
                                    create_prefab(m_world, prefab_name.c_str(), prefab_props);
                                } else {
                                    sandbox::modules::logs::error(m_world, "Prefabs Module: Prefab missing name.");
                                }
                            }
                        }
                    } catch (const std::exception& ex) {
                        sandbox::modules::logs::error(m_world, "Prefabs Module: Failed to load '{}': {}", prefabs_path, ex.what());
                    }
                } else {
                    sandbox::modules::logs::warn(m_world, "PrefabsModule: File not found: {}", prefabs_path);
                }
            });
    }

    flecs::entity PrefabsModule::create_prefab(flecs::world& world, const char* name, const sandbox::properties &props) {
        if (!name || name[0] == '\0') {
            sandbox::modules::logs::error(world, "Prefabs Module: Cannot create prefab with empty name.");
            return flecs::entity::null();
        }

        std::string global_name = ensure_global_name(name);
        flecs::entity prefab_entity = world.lookup(global_name.c_str());
        
        if (!prefab_entity.is_valid()) {
            prefab_entity = world.prefab(global_name.c_str());
        }

        build_entity(world, prefab_entity, props);
        return prefab_entity;
    }

    flecs::entity PrefabsModule::create_entity(flecs::world& world, const sandbox::properties& props) {
        flecs::entity instance = world.entity();
        std::string name = props.get<std::string>("name").value_or("");
        if (!name.empty()) {
            instance = world.entity(name.c_str());
        }

        std::string prefab_name = props.get<std::string>("prefab").value_or("");
        if (!prefab_name.empty()) {
            std::string global_name = ensure_global_name(prefab_name);
            flecs::entity prefab_entity = world.lookup(global_name.c_str());
            
            if (!prefab_entity.is_valid()) {
                prefab_entity = world.prefab(global_name.c_str());
            }
            
            instance.is_a(prefab_entity);
        }

        build_entity(world, instance, props);
        return instance;
    }

} // namespace spectre::module

// ─────────────────────────────────────────────────────────────────────────────
// C-ABI BRIDGE
// ─────────────────────────────────────────────────────────────────────────────

static void abi_register_component_factory(ecs_world_t* /*ecs*/, const char* component_name,
                                           spectre_component_factory_fn factory_fn) {
    if (component_name && component_name[0] != '\0' && factory_fn) {
        spectre::module::s_component_factories[component_name] = factory_fn;
    }
}

static bool abi_has_component_factory(ecs_world_t* /*ecs*/, const char* component_name) {
    if (!component_name) return false;
    return spectre::module::s_component_factories.count(component_name) > 0;
}

static ecs_entity_t abi_create_prefab(ecs_world_t* ecs, const char* prefab_name,
                                      sandbox_properties_handle_t prefab_props_handle) {
    if (!ecs || !prefab_name || prefab_name[0] == '\0') return 0;

    sandbox::properties props(prefab_props_handle, false);
    flecs::world world(ecs);
    return spectre::module::PrefabsModule::create_prefab(world, prefab_name, props).id();
}

static ecs_entity_t abi_create_entity(ecs_world_t* ecs, sandbox_properties_handle_t props_handle) {
    if (!ecs) return 0;

    sandbox::properties props(props_handle, false);
    flecs::world world(ecs);
    return spectre::module::PrefabsModule::create_entity(world, props).id();
}

static spectre_prefabs_api_t g_prefabs_api = {
    .register_component_factory = abi_register_component_factory,
    .has_component_factory      = abi_has_component_factory,
    .create_prefab              = abi_create_prefab,
    .create_entity              = abi_create_entity,
};
extern "C" {
SANDBOX_DEFINE_SERVICE(spectre_prefabs_service_t, spectre_prefabs_api_t, &g_prefabs_api)
}
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
