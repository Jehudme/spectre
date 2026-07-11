#include "prefabs_module.h"
#include "spectre/services/prefabs_service.h"
#include "spectre/sdk/serializer.hpp"
#include "spectre/sdk/scripts.hpp"
#include "spectre/sdk/components.hpp"
#include "sandbox/sdk/logs.hpp"
#include "../components/components_module.h"
#include <iostream>
#include <string>
#include <vector>

namespace spectre::modules {

    static ecs_entity_t deserialize_entity_cb(ecs_world_t* world, sandbox_properties_handle_t properties_handle);
    static sandbox_properties_handle_t serialize_entity_cb(ecs_world_t* world, ecs_entity_t entity_id);

    SANDBOX_DECLARE_MODULE(prefabs_module_t, {
        .name = "prefabs",
        .description = "Prefabs Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_prefabs_service_t_info,
        .requirements = nullptr,
        .requirement_count = 0
    })

    prefabs_module_t::prefabs_module_t(flecs::world& world) : m_world(world) {
        sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Prefabs Module] Initializing...");
        
        m_prefabs_root = m_world.entity("::prefabs");
        m_entity_prefab = m_world.prefab("::prefabs::prefab");

        spectre_serializer_component entity_serializer = {};
        entity_serializer.deserialize = deserialize_entity_cb;
        entity_serializer.serialize = serialize_entity_cb;
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
                    ::spectre::sdk::scripts::execute_on_destroy(target);
                }
            });

        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Prefabs Module] Initialized successfully.");
    }
    
    prefabs_module_t::~prefabs_module_t() = default;

    static ecs_entity_t deserialize_entity_cb(ecs_world_t* world, sandbox_properties_handle_t properties_handle) {
        if (!world) return 0;
        flecs::world flecs_world(world);
        auto* module_instance = flecs_world.try_get_mut<prefabs_module_t>();
        if (module_instance) {
            sandbox::properties parsed_properties(properties_handle, false);
            return module_instance->deserialize_entity(std::move(parsed_properties)).id();
        }
        return 0;
    }

    static sandbox_properties_handle_t serialize_entity_cb(ecs_world_t* world, ecs_entity_t entity_id) {
        if (!world || !entity_id) return {0};
        flecs::world flecs_world(world);
        auto* module_instance = flecs_world.try_get_mut<prefabs_module_t>();
        if (module_instance) {
            sandbox::properties serialized_properties = module_instance->serialize_entity(flecs_world.entity(entity_id));
            sandbox_properties_handle_t raw_handle = serialized_properties.get_raw();
            serialized_properties.release();
            return raw_handle;
        }
        return {0};
    }

    sandbox::properties prefabs_module_t::serialize_entity(flecs::entity entity_to_serialize) {
        sandbox::properties result_properties;
        if (!entity_to_serialize.is_valid()) {
            sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Prefabs Module] Cannot serialize invalid entity.");
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

        auto* components_module = const_cast<components_module_t*>(m_world.try_get_mut<components_module_t>());
        if (components_module) {
            sandbox::properties components_properties = components_module->serialize_component(entity_to_serialize);
            if (components_properties.is_valid()) {
                result_properties.merge("components", components_properties);
            }
        }
        
        if (m_script_args_serializer.id() != 0) {
            flecs::entity scripts_child = entity_to_serialize.lookup("scripts");
            if (scripts_child.is_valid()) {
                sandbox_properties_handle_t scripts_handle = spectre::modules::serializer::serialize_entity(m_world, m_script_args_serializer.id(), scripts_child.id());
                sandbox::properties scripts_properties(scripts_handle, false);
                if (scripts_properties.is_valid()) {
                    result_properties.merge("scripts", scripts_properties);
                }
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

    flecs::entity prefabs_module_t::deserialize_entity(sandbox::properties properties) {
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
        return deserialize_entity_target(new_entity, std::move(properties));
    }

    flecs::entity prefabs_module_t::deserialize_entity_target(flecs::entity target_entity, sandbox::properties properties) {
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
            auto* components_module = const_cast<components_module_t*>(m_world.try_get_mut<components_module_t>());
            if (components_module) {
                std::vector<std::string> component_keys = properties.keys("components");
                sandbox::properties components_node = properties.sub("components");
                for (const auto& component_name : component_keys) {
                    sandbox::properties component_properties = components_node.sub(component_name);
                    if (component_properties.is_valid()) {
                        flecs::entity temporary_component_entity = components_module->deserialize_component(component_name, std::move(component_properties));
                        if (temporary_component_entity.is_valid()) {
                            temporary_component_entity.each([&](flecs::id component_id) {
                                if (component_id.is_wildcard()) return;
                                const void* pointer = ecs_get_id(m_world.c_ptr(), temporary_component_entity.id(), component_id);
                                if (pointer) {
                                    const ecs_type_info_t* type_info = ecs_get_type_info(m_world.c_ptr(), component_id);
                                    size_t size = type_info ? type_info->size : 0;
                                    ecs_set_id(m_world.c_ptr(), target_entity.id(), component_id, size, pointer);
                                } else {
                                    target_entity.add(component_id);
                                }
                            });
                            temporary_component_entity.destruct();
                        }
                    }
                }
            }
        }
        
        if (properties.has("scripts") && m_script_args_serializer.id() != 0) {
            sandbox::properties scripts_node = properties.sub("scripts");
            sandbox_properties_handle_t handle = scripts_node.get_raw();
            ecs_entity_t raw_scripts_entity = spectre::modules::serializer::deserialize_entity(m_world, m_script_args_serializer.id(), handle);
            if (raw_scripts_entity != 0) {
                flecs::entity temporary_scripts_entity = m_world.entity(raw_scripts_entity);
                temporary_scripts_entity.child_of(target_entity);
                temporary_scripts_entity.set_name("scripts");
            }
        }

        if (properties.has("children")) {
            std::vector<std::string> child_keys = properties.keys("children");
            sandbox::properties children_node = properties.sub("children");
            for (const auto& child_key : child_keys) {
                sandbox::properties child_properties = children_node.sub(child_key);
                if (child_properties.is_valid()) {
                    flecs::entity child_entity = deserialize_entity(std::move(child_properties));
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
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Prefabs Module] Cannot register prefab with empty name.");
            return;
        }

        flecs::entity prefab = find_prefab(name);
        if (!prefab.is_valid()) {
            prefab = m_world.prefab(std::string(name).c_str())
                            .child_of(m_prefabs_root)
                            .is_a(m_entity_prefab);
        }

        if (properties.is_valid()) {
            deserialize_entity_target(prefab, std::move(properties));
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

        ::spectre::sdk::scripts::execute_on_create(target_entity);
    }

    flecs::entity prefabs_module_t::create_entity(sandbox::properties properties) {
        flecs::entity created_entity = deserialize_entity(std::move(properties));
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
