#include "prefabs_module.h"
#include "spectre/services/prefabs_service.h"
#include <iostream>
#include <string>
#include <vector>

#include "sandbox/sdk/logs.hpp"
#include "../components/components_module.h"
#include "../serializer/serializer_module.h"
#include "../scripts/scripts_module.h"


namespace spectre::modules {

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
        // make an argument serializer using the serializer module but in the  script module. but you can fetch and store this serializer in m_script_args_serializer and use it in the deserialize_entity function to deserialize scripts props.
        sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "Initializing prefabs module...");
        
        m_prefabs_root = m_world.entity("::prefabs");
        m_entity_prefab = m_world.prefab("::prefabs::prefab");

        auto* serializer_mod = const_cast<serializer_module*>(&m_world.get_mut<serializer_module>());
        if (serializer_mod) {
            m_script_args_serializer = serializer_mod->find_serializer("scripts");
        }
        
        // Register observer for entity destruction to run on_destroy scripts
        m_world.observer<spectre_use_script_on_destroy_relation_t>()
            .event(flecs::OnRemove)
            .each([](flecs::iter& it, size_t i, spectre_use_script_on_destroy_relation_t& rel) {
                flecs::entity entity = it.entity(i);
                flecs::id id = it.event_id();
                flecs::entity script_ent = id.second();
                
                auto* scripts_mod = const_cast<script_module_t*>(&it.world().get_mut<script_module_t>());
                if (scripts_mod && script_ent.is_valid()) {
                    std::vector<spectre_script_argument_t> args;
                    if (rel.argument_count > 0 && rel.arguments) {
                        args.assign(rel.arguments, rel.arguments + rel.argument_count);
                    }
                    scripts_mod->execute_script(script_ent.name().c_str(), args);
                }
            });


        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "Prefabs module initialized successfully.");
    }
    
    prefabs_module_t::~prefabs_module_t() = default;

    sandbox::properties prefabs_module_t::serialize_entity(flecs::entity entity) {
        // should also be able to serialize scripting like in reverse of his deserialization
        sandbox::properties result;
        if (!entity.is_valid()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Cannot serialize invalid entity.");
            return result;
        }

        if (entity.name().c_str()) {
            result.set<std::string>("name", entity.name().c_str());
        }

        std::vector<std::string> prefabs;
        entity.each(flecs::IsA, [&](flecs::entity prefab) {
            if (prefab.is_valid() && prefab.name().c_str()) {
                prefabs.push_back(prefab.name().c_str());
            }
        });
        if (!prefabs.empty()) {
            result.set_array<std::string>("prefabs", prefabs);
        }

        auto* components_mod = const_cast<components_module_t*>(&m_world.get_mut<components_module_t>());
        if (components_mod) {
            sandbox::properties comps = components_mod->serialize_component(entity);
            if (comps.is_valid()) {
                result.merge("components", comps);
            }
        }

        
        if (m_script_args_serializer.is_valid()) {
            auto* serializer_mod = const_cast<serializer_module*>(&m_world.get_mut<serializer_module>());
            if (serializer_mod) {
                sandbox::properties scripts_props = serializer_mod->serialize_entity(m_script_args_serializer, entity);
                if (scripts_props.is_valid()) {
                    result.merge("scripts", scripts_props);
                }
            }
        }

        sandbox::properties children_props;
        bool has_children = false;
        entity.children([&](flecs::entity child) {
            sandbox::properties child_props = serialize_entity(child);
            if (child_props.is_valid()) {
                std::string child_key = child.name().c_str() ? child.name().c_str() : std::to_string(child.id());
                children_props.merge(child_key, child_props);
                has_children = true;
            }
        });
        
        if (has_children) {
            result.merge("children", children_props);
        }

        return result;
    }

    flecs::entity prefabs_module_t::deserialize_entity(sandbox::properties props) {
        // i want you to use the argument serializer to deserialize ex:
        // scripts : { "on_create(or on_exit, or on_enter or... the relation are define component.h)": [{ "function": "...", "arguments": {...}}, ... (they can be many function to be executed on enter)]}
        // and store the function name and the argument on the proper script relation in you can find components.h, you can use these same connection to serialize the entity with the correct script and arguments
        // when calling one of the on create_entity function it automaticaly run all of the on_create script, and when the entity is destroyed it will run all of the on_delete scripts.
        if (!props.is_valid()) return flecs::entity::null();
        
        flecs::entity entity = m_world.entity();
        
        std::string name;
        if (props.get<std::string>("name", name)) {
            flecs::entity existing = m_world.lookup(name.c_str());
            if (existing.is_valid()) {
                entity = existing;
            } else {
                entity.set_name(name.c_str());
            }
        }
        return deserialize_entity_target(entity, std::move(props));
    }

    flecs::entity prefabs_module_t::deserialize_entity_target(flecs::entity entity, sandbox::properties props) {
        if (!props.is_valid() || !entity.is_valid()) return entity;

        std::vector<std::string> prefabs;
        if (props.get_array<std::string>("prefabs", prefabs)) {
            for (const auto& p_name : prefabs) {
                flecs::entity prefab = find_prefab(p_name);
                if (!prefab.is_valid()) {
                    prefab = m_world.entity(p_name.c_str())
                                    .child_of(m_prefabs_root)
                                    .is_a(m_entity_prefab);
                }
                entity.is_a(prefab);
            }
        }

        if (props.has("components")) {
            auto* components_mod = const_cast<components_module_t*>(&m_world.get_mut<components_module_t>());
            if (components_mod) {
                std::vector<std::string> comp_keys = props.keys("components");
                sandbox::properties components_node = props.sub("components");
                for (const auto& comp_name : comp_keys) {
                    sandbox::properties comp_props = components_node.sub(comp_name);
                    if (comp_props.is_valid()) {
                        flecs::entity temp_comp_ent = components_mod->deserialize_component(comp_name, std::move(comp_props));
                        if (temp_comp_ent.is_valid()) {
                            // Copy all components from temp_comp_ent to entity
                            temp_comp_ent.each([&](flecs::id id) {
                                if (id.is_wildcard()) return;
                                const void* ptr = ecs_get_id(m_world.c_ptr(), temp_comp_ent.id(), id);
                                if (ptr) {
                                    const ecs_type_info_t* ti = ecs_get_type_info(m_world.c_ptr(), id);
                                    size_t size = ti ? ti->size : 0;
                                    ecs_set_id(m_world.c_ptr(), entity.id(), id, size, ptr);
                                } else {
                                    entity.add(id);
                                }
                            });
                            temp_comp_ent.destruct();
                        }
                    }
                }
            }
        }

        
        if (props.has("scripts") && m_script_args_serializer.is_valid()) {
            auto* serializer_mod = const_cast<serializer_module*>(&m_world.get_mut<serializer_module>());
            if (serializer_mod) {
                sandbox::properties scripts_node = props.sub("scripts");
                flecs::entity temp_scripts_ent = serializer_mod->deserialize_entity(m_script_args_serializer, scripts_node);
                if (temp_scripts_ent.is_valid()) {
                    temp_scripts_ent.each([&](flecs::id id) {
                        if (id.is_wildcard()) return;
                        const void* ptr = ecs_get_id(m_world.c_ptr(), temp_scripts_ent.id(), id);
                        if (ptr) {
                            const ecs_type_info_t* ti = ecs_get_type_info(m_world.c_ptr(), id);
                            size_t size = ti ? ti->size : 0;
                            ecs_set_id(m_world.c_ptr(), entity.id(), id, size, ptr);
                        }
                    });
                    temp_scripts_ent.destruct();
                }
            }
        }

        if (props.has("children")) {
            std::vector<std::string> child_keys = props.keys("children");
            sandbox::properties children_node = props.sub("children");
            for (const auto& child_key : child_keys) {
                sandbox::properties child_props = children_node.sub(child_key);
                if (child_props.is_valid()) {
                    flecs::entity child_ent = deserialize_entity(std::move(child_props));
                    if (child_ent.is_valid()) {
                        child_ent.child_of(entity);
                    }
                }
            }
        }

        return entity;
    }

    void prefabs_module_t::register_prefab(std::string_view name, sandbox::properties props) {
        if (name.empty()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Cannot register prefab with empty name.");
            return;
        }

        flecs::entity prefab = find_prefab(name);
        if (!prefab.is_valid()) {
            prefab = m_world.entity(std::string(name).c_str())
                            .child_of(m_prefabs_root)
                            .is_a(m_entity_prefab);
        }

        if (props.is_valid()) {
            deserialize_entity_target(prefab, std::move(props));
        }
    }

    bool prefabs_module_t::has_prefab(std::string_view name) const {
        if (name.empty()) return false;
        flecs::entity prefab = m_prefabs_root.lookup(std::string(name).c_str());
        return is_prefab(prefab);
    }

    bool prefabs_module_t::is_prefab(flecs::entity entity) const {
        return entity.is_valid() && entity.has(flecs::IsA, m_entity_prefab);
    }

    flecs::entity prefabs_module_t::find_prefab(std::string_view name) {
        if (name.empty()) return flecs::entity::null();
        return m_prefabs_root.lookup(std::string(name).c_str());
    }

    
    static void run_on_create_scripts(flecs::entity entity, flecs::world& world) {
        if (!entity.is_valid()) return;
        
        // Run on_create for children first
        entity.children([&](flecs::entity child) {
            run_on_create_scripts(child, world);
        });

        auto* scripts_mod = const_cast<script_module_t*>(&world.get_mut<script_module_t>());
        if (scripts_mod) {
            int index = 0;
            while (flecs::entity script_ent = entity.target<spectre_use_script_on_create_relation_t>(index++)) {
                const auto* rel_ptr = entity.try_get<spectre_use_script_on_create_relation_t>(script_ent);
                if (!rel_ptr) continue;
                const auto& rel = *rel_ptr;
                
                std::vector<spectre_script_argument_t> args;
                if (rel.argument_count > 0 && rel.arguments) {
                    args.assign(rel.arguments, rel.arguments + rel.argument_count);
                }
                scripts_mod->execute_script(script_ent.name().c_str(), args);
            }
        }
    }

    flecs::entity prefabs_module_t::create_entity(sandbox::properties props) {
        flecs::entity e = deserialize_entity(std::move(props));
        run_on_create_scripts(e, m_world);
        return e;
    }

    flecs::entity prefabs_module_t::create_entity(flecs::entity prefab) {
        if (!prefab.is_valid()) return flecs::entity::null();
        flecs::entity e = m_world.entity().is_a(prefab);
        run_on_create_scripts(e, m_world);
        return e;
    }

    flecs::entity prefabs_module_t::create_entity(std::string_view name) {
        flecs::entity prefab = find_prefab(name);
        if (!prefab.is_valid()) return flecs::entity::null();
        flecs::entity e = m_world.entity().is_a(prefab);
        run_on_create_scripts(e, m_world);
        return e;
    }

}
