#include "components_module.h"

#include <iostream>

#include "sandbox/sdk/logs.hpp"
#include "spectre/services/serializer_service.h"
#include "spectre/sdk/serializer.hpp"
#include "spectre/sdk/components.hpp"

namespace spectre::modules {

    components_module_t::components_module_t(flecs::world& world) : m_world(world) {
        sandbox::modules::logs::trace(m_world, "[Components Module] Initializing...");

        m_components_root = m_world.entity("::components");
        
        m_component_prefab = m_world.prefab("::components::prefab")
            .set<spectre_component_serializer_t>({});

        sandbox::modules::logs::info(m_world, "[Components Module] Initialized successfully.");
    }

    components_module_t::~components_module_t() = default;

    sandbox::properties components_module_t::serialize_component(flecs::entity entity_to_serialize) {
        if (!entity_to_serialize.is_valid()) {
            sandbox::modules::logs::error(m_world, "[Components Module] Cannot serialize components for an invalid entity.");
            return sandbox::properties({0}, false);
        }

        sandbox::properties serialized_properties;
        bool has_any_components = false;

        entity_to_serialize.each([&](flecs::id component_id) {
            if (component_id.is_entity()) {
                flecs::entity component_entity = component_id.entity();
                sandbox::modules::logs::info(m_world, "[Components Module Debug] Checking component: {}", component_entity.name().c_str() ? component_entity.name().c_str() : "unnamed");

                if (is_component(component_entity)) {
                    std::string component_name = component_entity.name().c_str();
                    ecs_entity_t serializer_entity = spectre::modules::serializer::find_serializer(m_world, component_name.c_str());
                    
                    sandbox::modules::logs::info(m_world, "[Components Module Debug] Component is registered. Serializer: {}", serializer_entity);
                    
                    if (serializer_entity != 0) {
                        sandbox_properties_handle_t properties_handle = spectre::modules::serializer::serialize_entity(m_world, serializer_entity, entity_to_serialize.id());
                        sandbox::properties component_properties(properties_handle, true);
                        if (component_properties.is_valid()) {
                            sandbox::modules::logs::debug(m_world, "[Components Module] Successfully serialized component: {}", component_name);
                            serialized_properties.merge(component_name, component_properties);
                            has_any_components = true;
                        } else {
                            sandbox::modules::logs::warn(m_world, "[Components Module] serialize_entity returned invalid properties for '{}'", component_name);
                        }
                    } else {
                        sandbox::modules::logs::warn(m_world, "[Components Module] No serializer found for component type '{}' while serializing entity '{}'.", component_name, entity_to_serialize.name().c_str() ? entity_to_serialize.name().c_str() : "unnamed");
                    }
                } else {
                    sandbox::modules::logs::info(m_world, "[Components Module] Component '{}' is not registered as a module component.", component_entity.name().c_str() ? component_entity.name().c_str() : "unnamed");
                }
            }
        });

        if (!has_any_components) {
            serialized_properties.release();
            return sandbox::properties({0}, false);
        }

        return serialized_properties;
    }

    flecs::entity components_module_t::deserialize_component(std::string_view component_type, sandbox::properties properties) {
        if (component_type.empty()) {
            sandbox::modules::logs::error(m_world, "[Components Module] Cannot deserialize component: type is empty.");
            return flecs::entity::null();
        }

        if (!properties.is_valid()) {
            sandbox::modules::logs::error(m_world, "[Components Module] Cannot deserialize component '{}': properties are invalid.", component_type);
            return flecs::entity::null();
        }

        flecs::entity component_entity = m_components_root.lookup(std::string(component_type).c_str());
        if (!component_entity.is_valid()) {
            sandbox::modules::logs::warn(m_world, "[Components Module] Cannot deserialize component: type '{}' not found in component registry.", component_type);
            return flecs::entity::null();
        }

        if (!is_component(component_entity)) {
            sandbox::modules::logs::warn(m_world, "[Components Module] Entity '{}' is not a registered component prefab.", component_type);
            return flecs::entity::null();
        }

        ecs_entity_t serializer_entity = spectre::modules::serializer::find_serializer(m_world, std::string(component_type).c_str());
        if (serializer_entity == 0) {
            sandbox::modules::logs::error(m_world, "[Components Module] No serializer found for component type '{}'.", component_type);
            return flecs::entity::null();
        }

        ecs_entity_t deserialized_entity = spectre::modules::serializer::deserialize_entity(m_world, serializer_entity, properties.get_raw());
        
        return m_world.entity(deserialized_entity);
    }

    void components_module_t::register_component(std::string_view component_type, component_registration_fn registration_function, spectre_serializer_component serializer_callbacks) {
        if (component_type.empty()) {
            sandbox::modules::logs::error(m_world, "[Components Module] Cannot register component: type is empty.");
            return;
        }

        if (registration_function == nullptr) {
            sandbox::modules::logs::error(m_world, "[Components Module] Cannot register component '{}': registration function is null.", component_type);
            return;
        }

        ecs_entity_t component_id = registration_function(m_world.c_ptr());
        if (component_id == 0) {
            sandbox::modules::logs::error(m_world, "[Components Module] Failed to register component '{}': registration function returned 0.", component_type);
            return;
        }

        flecs::entity component_entity(m_world, component_id);
        
        component_entity.set_name(std::string(component_type).c_str());
        component_entity.child_of(m_components_root);
        component_entity.is_a(m_component_prefab);

        spectre::modules::serializer::register_serializer(m_world, std::string(component_type).c_str(), &serializer_callbacks);

        sandbox::modules::logs::info(m_world, "[Components Module] Successfully registered component '{}'.", component_type);
    }

    bool components_module_t::has_component(std::string_view component_type) const {
        if (component_type.empty()) return false;
        flecs::entity component_entity = m_components_root.lookup(std::string(component_type).c_str());
        return is_component(component_entity);
    }

    bool components_module_t::is_component(flecs::entity entity_to_check) const {
        return entity_to_check.is_valid() && entity_to_check.has(flecs::IsA, m_component_prefab);
    }
}
