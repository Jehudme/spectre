#include "serializer_module.h"

#include "sandbox/abi/bootstrapper.h"

namespace spectre::modules {

    serializer_module::serializer_module(flecs::world &world) : m_world(world) {
        m_serializer = m_world.entity("spectre::serializer");

    }

    serializer_module::~serializer_module() {

    }

    void serializer_module::register_serializer(std::string_view type, const serializer_t &serializer) {
        // log an override for this type if alredy register
        // instantiate an entity with the serializer_t component with the type as name
        // add this entity to m_serializer entity
    }

    bool serializer_module::has_serializer(std::string_view type) const {
        // return true if m_serializer already have an entity with the name type
    }

    flecs::entity serializer_module::find_serializer(std::string_view type) const {
        // return an null entity if it do not exist and log an warning
        // return the entity with the variable type as name from m_serializer entity
    }

    sandbox::properties serializer_module::serialize_entity(flecs::entity serializer_entity, flecs::entity target_entity) {
        // log an error and return an null properties if serializer_entity(must have an serializer_t component) or target_entity are invalid
        // instantiate an sandbox::properties from serialize function inside of the serialize_t component from serializer_entity
        // return the properties instance, log an error if the instance is null that mean it failed
    }

    flecs::entity serializer_module::deserialize_entity(flecs::entity serializer_entity, const sandbox::properties &props) {
        // log an error and return an null properties if serializer_entity(must have an serializer_t component) or target_entity are invalid
        // instantiate an entity from deserialize function inside of the serialize_t component from serializer_entity
        // return the properties instance, log an error if the instance is null that mean it failed
    }
}

