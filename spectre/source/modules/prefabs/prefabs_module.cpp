#include "prefabs_module.h"
#include "spectre/services/prefabs_service.h"
#include <iostream>
#include <string>
#include <vector>

#include "spectre/sdk/scripts.hpp"
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

    static ecs_entity_t deserialize_entity_cb(ecs_world_t* world, sandbox_properties_handle_t props_handle) {
        flecs::world w(world);
        auto* mod = const_cast<prefabs_module_t*>(w.try_get_mut<prefabs_module_t>());
        if (mod) {
            sandbox::properties props(props_handle, false);
            return mod->deserialize_entity(std::move(props)).id();
        }
        return 0;
    }

    static sandbox_properties_handle_t serialize_entity_cb(ecs_world_t* world, ecs_entity_t entity) {
        flecs::world w(world);
        auto* mod = const_cast<prefabs_module_t*>(w.try_get_mut<prefabs_module_t>());
        if (mod) {
            sandbox::properties props = mod->serialize_entity(flecs::entity(w, entity));
            sandbox_properties_handle_t handle = props.get_raw();
            props.release();
            return handle;
        }
        return {0};
    }

    prefabs_module_t::prefabs_module_t(flecs::world& world) : m_world(world) {
        // make an argument serializer using the serializer module but in the  script module. but you can fetch and store this serializer in m_script_args_serializer and use it in the deserialize_entity function to deserialize scripts props.
        sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "Initializing prefabs module...");
        
        m_prefabs_root = m_world.entity("::prefabs");
        m_entity_prefab = m_world.prefab("::prefabs::prefab");

        auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
        if (serializer_mod) {
            spectre_serializer_component ser_comp;
            ser_comp.deserialize = deserialize_entity_cb;
            ser_comp.serialize = serialize_entity_cb;
            serializer_mod->register_serializer("entity", ser_comp);
            
            m_script_args_serializer = serializer_mod->find_serializer("scripts");
            m_entity_serializer = serializer_mod->find_serializer("entity");
        }
        
        // Register observer for entity destruction to run on_destroy scripts
        m_world.observer<spectre_use_script_on_destroy_relation_t>()
            .event(flecs::OnRemove)
            .each([](flecs::iter& it, size_t i, spectre_use_script_on_destroy_relation_t& rel) {
                flecs::entity entity = it.entity(i);
                flecs::entity target = entity.parent();
                if (!target.is_valid() || target.name() != "scripts") target = entity;
                else target = target.parent();
                
                if (target.is_valid()) {
                    ::spectre::sdk::scripts::execute_on_destroy(target);
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

        auto* components_mod = const_cast<components_module_t*>(m_world.try_get_mut<components_module_t>());
        if (components_mod) {
            sandbox::properties comps = components_mod->serialize_component(entity);
            if (comps.is_valid()) {
                result.merge("components", comps);
            }
        }

        
        if (m_script_args_serializer.is_valid()) {
            auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
            if (serializer_mod) {
                // Find the child that represents scripts
                flecs::entity scripts_child = entity.lookup("scripts");
                if (scripts_child.is_valid()) {
                    sandbox::properties scripts_props = serializer_mod->serialize_entity(m_script_args_serializer, scripts_child);
                    if (scripts_props.is_valid()) {
                        result.merge("scripts", scripts_props);
                    }
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
                    prefab = m_world.prefab(p_name.c_str())
                                    .child_of(m_prefabs_root)
                                    .is_a(m_entity_prefab);
                }
                entity.is_a(prefab);
            }
        }

        if (props.has("components")) {
            auto* components_mod = const_cast<components_module_t*>(m_world.try_get_mut<components_module_t>());
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
            auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
            if (serializer_mod) {
                sandbox::properties scripts_node = props.sub("scripts");
                flecs::entity temp_scripts_ent = serializer_mod->deserialize_entity(m_script_args_serializer, scripts_node);
                if (temp_scripts_ent.is_valid()) {
                    temp_scripts_ent.child_of(entity);
                    temp_scripts_ent.set_name("scripts");
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
            prefab = m_world.prefab(std::string(name).c_str())
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

        ::spectre::sdk::scripts::execute_on_create(entity);
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
