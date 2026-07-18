#include "prefabs_module.h"
#include "spectre/services/prefabs_service.h"
#include "spectre/sdk/serializer.hpp"
#include "spectre/sdk/scripts.hpp"
#include "sandbox/sdk/logs.hpp"
#include <iostream>
#include <string>
#include <vector>

#include "spectre/sdk/prefabs.hpp"

namespace spectre::modules {

    static void deserialize_entity_cb(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t properties_handle);
    static sandbox_properties_handle_t serialize_entity_cb(ecs_world_t* world, ecs_entity_t entity_id);

    static sandbox_requirement_info_t prefabs_requirements[] = {
        {
            .kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name = "logs",
            .architecture = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(prefabs_module_t, {
        .name = "prefabs",
        .description = "Prefabs Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_prefabs_service_t_info,
        .requirements = prefabs_requirements,
        .requirement_count = 1
    })

    prefabs_module_t::prefabs_module_t(flecs::world& world) : m_world(world) {
        sandbox::modules::logs::trace(m_world, "[Prefabs Module] Initializing...");
        
        m_prefabs_root = m_world.entity("::prefabs");
        m_entity_prefab = m_world.prefab("::prefabs::prefab");

        spectre_serializer_component entity_serializer = {
            .deserialize = deserialize_entity_cb,
            .serialize = serialize_entity_cb,
        };
        spectre::modules::serializer::register_serializer(m_world, "entity", &entity_serializer);
        
        m_script_args_serializer = m_world.entity(spectre::modules::serializer::find_serializer(m_world, "scripts"));
        m_entity_serializer = m_world.entity(spectre::modules::serializer::find_serializer(m_world, "entity"));
        
        // Register observer for entity destruction to run on_destroy scripts
        m_world.observer<spectre_use_script_on_destroy_relation_t>()
            .event(flecs::OnRemove)
            .each([](flecs::iter& iterator, size_t index, spectre_use_script_on_destroy_relation_t& relation) {
                flecs::entity entity = iterator.entity(index);
                flecs::entity target = entity.parent();
                if (!target.is_valid() || target.name() != "scripts") target = entity;
                else target = target.parent();
                
                if (target.is_valid()) {
                    ::spectre::modules::scripts::execute_on_destroy(target);
                }
            });

        sandbox::modules::logs::info(m_world, "[Prefabs Module] Initialized successfully.");
    }
    
    prefabs_module_t::~prefabs_module_t() = default;

    static void deserialize_entity_cb(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t properties_handle) {
        spectre::modules::prefabs::deserialize_entity(flecs::world(world), entity, properties_handle);
    }

    static sandbox_properties_handle_t serialize_entity_cb(ecs_world_t* world, ecs_entity_t entity_id) {
        return spectre::modules::prefabs::serialize_entity(flecs::world(world), entity_id);   
    }

    sandbox::properties prefabs_module_t::serialize_entity(flecs::entity entity_to_serialize) {
        sandbox::properties result_properties;
        if (!entity_to_serialize.is_valid()) {
            sandbox::modules::logs::warn(m_world, "[Prefabs Module] Cannot serialize invalid entity.");
            return result_properties;
        }

        if (entity_to_serialize.name().c_str()) {
            result_properties.set<std::string>("name", entity_to_serialize.name().c_str());
        }

        std::vector<std::string> prefabs_list;
        entity_to_serialize.each(flecs::IsA, [&](flecs::entity prefab) {
            if (prefab.is_valid() && prefab.name().c_str()) {
                prefabs_list.push_back(prefab.name().c_str());
            }
        });
        if (!prefabs_list.empty()) {
            result_properties.set_array<std::string>("prefabs", prefabs_list);
        }

        sandbox::properties components_properties;
        entity_to_serialize.each([&](flecs::id id) {
            if (id.is_pair()) return;
            flecs::entity comp_entity = id.entity();
            if (!comp_entity.is_valid() || !comp_entity.name().c_str()) return;
            
            std::string s_name = comp_entity.name().c_str();
            ecs_entity_t serializer_id = spectre::modules::serializer::find_serializer(m_world, s_name.c_str());
            if (serializer_id != 0) {
                sandbox_properties_handle_t props_handle = spectre::modules::serializer::serialize_entity(m_world, serializer_id, entity_to_serialize.id());
                sandbox::properties props(props_handle, false);
                if (props.is_valid()) {
                    components_properties.merge(s_name, props);
                }
            }
        });
        
        if (components_properties.is_valid()) {
            result_properties.merge("components", components_properties);
        }
        
        if (m_script_args_serializer.id() != 0) {
            sandbox_properties_handle_t scripts_handle = spectre::modules::serializer::serialize_entity(m_world, m_script_args_serializer.id(), entity_to_serialize.id());
            sandbox::properties scripts_properties(scripts_handle, false);
            if (scripts_properties.is_valid()) {
                result_properties.merge("scripts", scripts_properties);
            }
        }

        sandbox::properties children_properties;
        bool has_children = false;
        entity_to_serialize.children([&](flecs::entity child) {
            sandbox::properties child_properties = serialize_entity(child);
            if (child_properties.is_valid()) {
                std::string child_key = child.name().c_str() ? child.name().c_str() : std::to_string(child.id());
                children_properties.merge(child_key, child_properties);
                has_children = true;
            }
        });
        
        if (has_children) {
            result_properties.merge("children", children_properties);
        }

        return result_properties;
    }

    // We inject components directly into the target entity to avoid complex merging logic
    // We also link scripts directly to the entity instead of using a child entity for cleaner architecture
    flecs::entity prefabs_module_t::deserialize_entity(flecs::entity target_entity, sandbox::properties properties) {
        if (!properties.is_valid() || !target_entity.is_valid()) return target_entity;

        std::vector<std::string> prefabs_list;
        if (properties.get_array<std::string>("prefabs", prefabs_list)) {
            for (const auto& prefab_name : prefabs_list) {
                flecs::entity prefab = find_prefab(prefab_name);
                if (!prefab.is_valid()) {
                    prefab = m_world.prefab(prefab_name.c_str())
                                    .child_of(m_prefabs_root)
                                    .is_a(m_entity_prefab);
                }
                target_entity.is_a(prefab);
            }
        }

        if (properties.has("components")) {
            std::vector<std::string> component_keys = properties.keys("components");
            sandbox::properties components_node = properties.sub("components");
            for (const auto& component_name : component_keys) {
                sandbox::properties component_properties = components_node.sub(component_name);
                if (component_properties.is_valid()) {
                    ecs_entity_t serializer_id = spectre::modules::serializer::find_serializer(m_world, component_name.c_str());
                    if (serializer_id != 0) {
                        spectre::modules::serializer::deserialize_entity(m_world, serializer_id, target_entity.id(), component_properties.get_raw());
                    } else {
                        sandbox::modules::logs::info(m_world, "[Prefabs Module] No serializer for component '{}', trying fallback lookup", component_name);
                        // If no serializer, try to just add the component
                        flecs::entity comp_entity = m_world.lookup(component_name.c_str());
                        if (comp_entity.is_valid()) {
                            target_entity.add(comp_entity);
                        }
                    }
                }
            }
        }


        if (properties.has("children")) {
            std::vector<std::string> child_keys = properties.keys("children");
            sandbox::properties children_node = properties.sub("children");
            for (const auto& child_key : child_keys) {
                sandbox::properties child_properties = children_node.sub(child_key);
                if (child_properties.is_valid()) {
                    flecs::entity new_child = m_world.entity();
                    std::string entity_name;
                    if (child_properties.get<std::string>("name", entity_name)) {
                        flecs::entity existing_entity = m_world.lookup(entity_name.c_str());
                        if (existing_entity.is_valid()) {
                            new_child = existing_entity;
                        } else {
                            new_child.set_name(entity_name.c_str());
                        }
                    }
                    flecs::entity child_entity = deserialize_entity(new_child, std::move(child_properties));
                    if (child_entity.is_valid()) {
                        child_entity.child_of(target_entity);
                    }
                }
            }
        }

        return target_entity;
    }

    void prefabs_module_t::register_prefab(std::string_view name, sandbox::properties properties) {
        if (name.empty()) {
            sandbox::modules::logs::error(m_world, "[Prefabs Module] Cannot register prefab with empty name.");
            return;
        }

        flecs::entity prefab = find_prefab(name);
        if (!prefab.is_valid()) {
            prefab = m_world.prefab(std::string(name).c_str())
                            .child_of(m_prefabs_root)
                            .is_a(m_entity_prefab);
        }

        if (properties.is_valid()) {
            deserialize_entity(prefab, std::move(properties));
            sandbox::modules::logs::info(m_world, "[Prefabs Module] Registered prefab: '{}'", name);
        }
    }

    bool prefabs_module_t::has_prefab(std::string_view name) const {
        if (name.empty()) return false;
        flecs::entity prefab = m_prefabs_root.lookup(std::string(name).c_str());
        return is_prefab(prefab);
    }

    bool prefabs_module_t::is_prefab(flecs::entity entity_to_check) const {
        return entity_to_check.is_valid() && entity_to_check.has(flecs::IsA, m_entity_prefab);
    }

    flecs::entity prefabs_module_t::find_prefab(std::string_view name) {
        if (name.empty()) return flecs::entity::null();
        return m_prefabs_root.lookup(std::string(name).c_str());
    }
    
    static void run_on_create_scripts(flecs::entity target_entity, flecs::world& world) {
        if (!target_entity.is_valid()) return;
        
        // Run on_create for children first
        target_entity.children([&](flecs::entity child) {
            run_on_create_scripts(child, world);
        });

        spectre::modules::scripts::execute_on_create(target_entity);
    }

    flecs::entity prefabs_module_t::create_entity(sandbox::properties properties) {
        if (!properties.is_valid()) return flecs::entity::null();
        
        flecs::entity new_entity = m_world.entity();
        std::string entity_name;
        if (properties.get<std::string>("name", entity_name)) {
            flecs::entity existing_entity = m_world.lookup(entity_name.c_str());
            if (existing_entity.is_valid()) {
                new_entity = existing_entity;
            } else {
                new_entity.set_name(entity_name.c_str());
            }
        }
        
        flecs::entity created_entity = deserialize_entity(new_entity, std::move(properties));
        run_on_create_scripts(created_entity, m_world);
        return created_entity;
    }

    flecs::entity prefabs_module_t::create_entity(flecs::entity prefab) {
        if (!prefab.is_valid()) return flecs::entity::null();
        flecs::entity created_entity = m_world.entity().is_a(prefab);
        run_on_create_scripts(created_entity, m_world);
        return created_entity;
    }

    flecs::entity prefabs_module_t::create_entity(std::string_view name) {
        flecs::entity prefab = find_prefab(name);
        if (!prefab.is_valid()) return flecs::entity::null();
        flecs::entity created_entity = m_world.entity().is_a(prefab);
        run_on_create_scripts(created_entity, m_world);
        return created_entity;
    }

}
