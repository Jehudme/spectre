#include "prefabs_module.h"
#include "spectre/services/prefabs_service.h"
#include <iostream>
#include <string>
#include <vector>

#include "sandbox/sdk/logs.hpp"
#include "../components/components_module.h"
#include "../serializer/serializer_module.h"

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
        sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "Initializing prefabs module...");
        
        m_prefabs_root = m_world.entity("::prefabs");
        m_entity_prefab = m_world.prefab("::prefabs::prefab");

        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "Prefabs module initialized successfully.");
    }
    
    prefabs_module_t::~prefabs_module_t() = default;

    sandbox::properties prefabs_module_t::serialize_entity(flecs::entity entity) {
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

    flecs::entity prefabs_module_t::create_entity(sandbox::properties props) {
        return deserialize_entity(std::move(props));
    }

    flecs::entity prefabs_module_t::create_entity(flecs::entity prefab) {
        if (!prefab.is_valid()) return flecs::entity::null();
        return m_world.entity().is_a(prefab);
    }

    flecs::entity prefabs_module_t::create_entity(std::string_view name) {
        flecs::entity prefab = find_prefab(name);
        if (!prefab.is_valid()) return flecs::entity::null();
        return m_world.entity().is_a(prefab);
    }

}
