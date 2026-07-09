#include "components_module.h"

#include <iostream>

#include "sandbox/sdk/logs.hpp"
#include "spectre/services/serializer_service.h"
#include "../serializer/serializer_module.h"

namespace spectre::modules {

    components_module_t::components_module_t(flecs::world& world) : m_world(world) {
        sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "Initializing components module...");

        m_world.component<spectre_prefab_flag_t>("spectre_prefab_flag_t");
        m_world.component<spectre_serializer_component>("spectre_serializer_component");
        m_world.component<spectre_serializer_relation_t>("spectre_serializer_relation_t");
        m_world.component<spectre_resource_loader_relation>("spectre_resource_loader_relation");
        m_world.component<spectre_use_resource_relation>("spectre_use_resource_relation");
        m_world.component<spectre_resource_component_t>("spectre_resource_component_t");
        m_world.component<spectre_resource_loader_component_t>("spectre_resource_loader_component_t");
        m_world.component<spectre_scene_t>("spectre_scene_t");
        m_world.component<spectre_state_t>("spectre_state_t");
        m_world.component<spectre_state_use_scene_relation_t>("spectre_state_use_scene_relation_t");
        m_world.component<spectre_renderable_t>("spectre_renderable_t");
        m_world.component<spectre_2D_transform_component_t>("spectre_2D_transform_component_t");
        m_world.component<spectre_use_script_on_enter_relation_t>("spectre_use_script_on_enter_relation_t");
        m_world.component<spectre_use_script_on_exit_relation_t>("spectre_use_script_on_exit_relation_t");
        m_world.component<spectre_use_script_on_create_relation_t>("spectre_use_script_on_create_relation_t");
        m_world.component<spectre_use_script_on_destroy_relation_t>("spectre_use_script_on_destroy_relation_t");
        m_world.component<spectre_use_script_on_update_relation_t>("spectre_use_script_on_update_relation_t");
        m_world.component<spectre_use_script_on_render_relation_t>("spectre_use_script_on_render_relation_t");
        m_world.component<spectre_script_component_t>("spectre_script_component_t");
        m_world.component<spectre_input_state_t>("spectre_input_state_t");
        m_world.component<spectre_component_serializer_t>("spectre_component_serializer_t");

        m_components_root = m_world.entity("::components");
        
        m_component_prefab = m_world.prefab("::components::prefab")
            .set<spectre_component_serializer_t>({});

        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "Components module initialized successfully.");
    }

    components_module_t::~components_module_t() = default;

    sandbox::properties components_module_t::serialize_component(flecs::entity entity) {
        if (!entity.is_valid()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Cannot serialize components for an invalid entity.");
            return sandbox::properties({0}, false);
        }

        auto* serializer_mod = const_cast<serializer_module*>(&m_world.get_mut<serializer_module>());
        if (!serializer_mod) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Serializer module is not available in components_module serialize.");
            return sandbox::properties({0}, false);
        }

        sandbox::properties result;
        bool has_any = false;

        entity.each([&](flecs::id id) {
            if (id.is_entity()) {
                flecs::entity comp = id.entity();
                if (is_component(comp)) {
                    std::string comp_name = comp.name().c_str();
                    flecs::entity serializer_entity = serializer_mod->find_serializer(comp_name);
                    
                    if (serializer_entity.is_valid()) {
                        sandbox::properties comp_props = serializer_mod->serialize_entity(serializer_entity, entity);
                        if (comp_props.is_valid()) {
                            result.merge(comp_name, comp_props);
                            has_any = true;
                        }
                    }
                }
            }
        });

        if (!has_any) {
            result.release();
            return sandbox::properties({0}, false);
        }

        return result;
    }

    flecs::entity components_module_t::deserialize_component(std::string_view type, sandbox::properties props) {
        if (type.empty()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Cannot deserialize component: type is empty.");
            return flecs::entity::null();
        }

        if (!props.is_valid()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Cannot deserialize component: properties are invalid.");
            return flecs::entity::null();
        }

        auto* serializer_mod = const_cast<serializer_module*>(&m_world.get_mut<serializer_module>());
        if (!serializer_mod) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Serializer module is not available.");
            return flecs::entity::null();
        }

        flecs::entity comp = m_components_root.lookup(std::string(type).c_str());
        if (!comp.is_valid()) {
            sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "Cannot deserialize component: type '{}' not found.", type);
            return flecs::entity::null();
        }

        if (!is_component(comp)) {
            sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "Entity '{}' is not a registered component prefab.", type);
            return flecs::entity::null();
        }

        flecs::entity serializer_entity = serializer_mod->find_serializer(type);
        if (!serializer_entity.is_valid()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "No serializer found for component type '{}'.", type);
            return flecs::entity::null();
        }

        return serializer_mod->deserialize_entity(serializer_entity, props);
    }

    void components_module_t::register_component(std::string_view type, component_registration_fn registration, spectre_serializer_component serializer) {
        if (type.empty()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Cannot register component: type is empty.");
            return;
        }

        if (registration == nullptr) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Cannot register component '{}': registration function is null.", type);
            return;
        }

        auto* serializer_mod = const_cast<serializer_module*>(&m_world.get_mut<serializer_module>());
        if (!serializer_mod) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Serializer module is not available.");
            return;
        }

        ecs_entity_t comp_id = registration(m_world.c_ptr());
        if (comp_id == 0) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Failed to register component '{}': registration function returned 0.", type);
            return;
        }

        flecs::entity comp_entity(m_world, comp_id);
        
        comp_entity.set_name(std::string(type).c_str());
        comp_entity.child_of(m_components_root);
        comp_entity.is_a(m_component_prefab);

        serializer_mod->register_serializer(type, serializer);

        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "Successfully registered component '{}'.", type);
    }

    bool components_module_t::has_component(std::string_view type) const {
        if (type.empty()) return false;
        flecs::entity comp = m_components_root.lookup(std::string(type).c_str());
        return is_component(comp);
    }

    bool components_module_t::is_component(flecs::entity entity) const {
        return entity.is_valid() && entity.has(flecs::IsA, m_component_prefab);
    }
}
