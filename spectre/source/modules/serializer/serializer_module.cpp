#include "serializer_module.h"

#include <format>
#include <iostream>

#include "sandbox/sdk/logs.hpp"
#include "spectre/services/serializer_service.h"


namespace spectre::modules {

    static sandbox_requirement_info_t serializer_requirements[] = {
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

    SANDBOX_DECLARE_MODULE(serializer_module, {
        .name = "serializer",
        .description = "Serializer Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_serializer_service_t_info,
        .requirements = serializer_requirements,
        .requirement_count = 1
    })

    // Component Registration Callbacks
    static ecs_entity_t register_serializer_comp(ecs_world_t* world) { return flecs::world(world).component<spectre_serializer_component>().id(); }

    serializer_module::serializer_module(flecs::world& world) : m_world(world) {
        sandbox::modules::logs::trace(m_world, "[Serializer Module] Initializing...");

        m_serializer = m_world.entity("::serializers");
        
        m_serializable_prefab = m_world.prefab("::serializers::prefab")
            .set<serializer_t>({});

        auto deserialize_empty = [](ecs_world_t*, ecs_entity_t, sandbox_properties_handle_t) {};
        auto serialize_empty = [](ecs_world_t*, ecs_entity_t) -> sandbox_properties_handle_t { return {0}; };
        spectre_serializer_component empty_serializer = {deserialize_empty, serialize_empty};
        register_serializer_comp(m_world.c_ptr());
        register_serializer("spectre_serializer_component", empty_serializer);
            
        sandbox::modules::logs::info(m_world, "[Serializer Module] Initialized successfully.");
    }
    
    serializer_module::~serializer_module() {
        sandbox::modules::logs::trace(m_world, "[Serializer Module] Destroyed.");
    }

    void serializer_module::register_serializer(std::string_view serializer_type, const serializer_t& serializer_component) {
        if (serializer_type.empty()) {
            sandbox::modules::logs::error(m_world, "Failed to register serializer: serializer type name is empty.");
            return;
        }

        if (serializer_component.serialize == nullptr || serializer_component.deserialize == nullptr) {
            sandbox::modules::logs::error(m_world, "Failed to register serializer '{}': missing serialize or deserialize function pointers.", serializer_type);
            return;
        }

        flecs::entity serializer_entity = m_world.entity(std::string(serializer_type).c_str())
                                                .child_of(m_serializer)
                                                .is_a(m_serializable_prefab)
                                                .set<serializer_t>(serializer_component);

        sandbox::modules::logs::info(m_world, "Registered serializer for type: '{}'.", serializer_type);
    }

    bool serializer_module::has_serializer(std::string_view serializer_type) const {
        if (serializer_type.empty()) {
            return false;
        }

        flecs::entity serializer_entity = find_serializer(serializer_type);
        return is_serializer(serializer_entity);
    }

    bool serializer_module::is_serializer(flecs::entity entity) const {
        return entity.is_valid() && entity.has(flecs::IsA, m_serializable_prefab);
    }

    flecs::entity serializer_module::find_serializer(std::string_view serializer_type) const {
        if (serializer_type.empty()) {
            sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "Attempted to find serializer with an empty type name.");
            return flecs::entity::null();
        }

        flecs::entity serializer_entity = m_serializer.lookup(std::string(serializer_type).c_str());
        
        if (!serializer_entity.is_valid()) {
            sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "Serializer for type '{}' not found.", serializer_type);
        }
        
        return serializer_entity;
    }

    sandbox::properties serializer_module::serialize_entity(flecs::entity serializer_entity, flecs::entity target_entity) {
        if (!serializer_entity.is_valid()) {
            sandbox::modules::logs::error(m_world, "Failed to serialize entity: serializer entity is invalid.");
            return sandbox::properties({0}, false);
        }

        if (!target_entity.is_valid()) {
            sandbox::modules::logs::error(m_world, "Failed to serialize entity: target entity is invalid.");
            return sandbox::properties({0}, false);
        }

        if (!serializer_entity.has<serializer_t>()) {
            sandbox::modules::logs::error(m_world, "Failed to serialize entity: serializer entity {} does not have a serializer_t component.", serializer_entity.id());
            return sandbox::properties({0}, false);
        }

        const serializer_t* serializer_component = &serializer_entity.get<serializer_t>();
        
        if (serializer_component == nullptr || serializer_component->serialize == nullptr) {
            sandbox::modules::logs::error(m_world, "Failed to serialize entity: serializer component or serialize function is null.");
            return sandbox::properties({0}, false);
        }

        sandbox::properties serialized_properties{serializer_component->serialize(m_world.c_ptr(), target_entity.id())};
        
        if (!serialized_properties.is_valid()) {
            sandbox::modules::logs::warn(m_world, "Serialization of entity {} returned invalid properties.", target_entity.id());
        } else {
            sandbox::modules::logs::trace(m_world, "Successfully serialized entity {}.", target_entity.id());
        }

        return serialized_properties;
    }

    void serializer_module::deserialize_entity(flecs::entity serializer_entity, flecs::entity target_entity, const sandbox::properties& properties) {
        if (!serializer_entity.is_valid()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Failed to deserialize entity: serializer entity is invalid.");
            return;
        }

        if (!target_entity.is_valid()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Failed to deserialize entity: target entity is invalid.");
            return;
        }

        if (!properties.is_valid()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Failed to deserialize entity: input properties are invalid.");
            return;
        }

        if (!serializer_entity.has<serializer_t>()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Failed to deserialize entity: serializer entity {} does not have a serializer_t component.", serializer_entity.id());
            return;
        }

        const serializer_t* serializer_component = serializer_entity.try_get<serializer_t>();
        
        if (serializer_component == nullptr || serializer_component->deserialize == nullptr) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Failed to deserialize entity: serializer component or deserialize function is null.");
            return;
        }

        serializer_component->deserialize(m_world.c_ptr(), target_entity.id(), properties.get_raw());
        
        sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "Successfully deserialized entity {}.", target_entity.id());
    }

}
