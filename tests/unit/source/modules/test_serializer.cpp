#include "modules/serializer/serializer_module.h"
#include "sandbox/sdk/properties.hpp"
#include "spectre/spectre.h"
#include <catch2/catch_all.hpp>
#include <flecs.h>

using namespace spectre::modules;

struct test_id_t {
    uint64_t id;
};

TEST_CASE("Serializer Module: Registration and Queries", "[serializers module test]") {
    flecs::world world;
    world.import <spectre::modules::serializer_module>();

    // Obtain the module
    auto* serializer_mod = world.try_get_mut<spectre::modules::serializer_module>();
    REQUIRE(serializer_mod != nullptr);

    SECTION("has_serializer returns false for empty or unregistered type") {
        REQUIRE(serializer_mod->has_serializer("") == false);
        REQUIRE(serializer_mod->has_serializer("non_existent_serializer") == false);
    }

    SECTION("find_serializer returns invalid entity for empty or unregistered type") {
        REQUIRE(serializer_mod->find_serializer("").is_valid() == false);
        REQUIRE(serializer_mod->find_serializer("non_existent_serializer").is_valid() == false);
    }

    SECTION("register_serializer rejects registration with missing function pointers") {
        spectre_serializer_component invalid_serializer{};
        invalid_serializer.serialize = nullptr;
        invalid_serializer.deserialize = nullptr;

        serializer_mod->register_serializer("invalid_serializer", invalid_serializer);

        REQUIRE(serializer_mod->has_serializer("invalid_serializer") == false);
        REQUIRE(serializer_mod->find_serializer("invalid_serializer").is_valid() == false);
    }

    SECTION("register_serializer correctly registers and finds serializer") {
        spectre_serializer_component valid_serializer{};
        valid_serializer.serialize = [](ecs_world_t*, ecs_entity_t) -> sandbox_properties_handle_t { return {0}; };
        valid_serializer.deserialize = [](ecs_world_t*, ecs_entity_t, sandbox_properties_handle_t) {};

        serializer_mod->register_serializer("test_serializer", valid_serializer);

        REQUIRE(serializer_mod->has_serializer("test_serializer") == true);

        flecs::entity found = serializer_mod->find_serializer("test_serializer");
        REQUIRE(found.is_valid() == true);
        REQUIRE(found.has<spectre_serializer_component>() == true);
        REQUIRE(serializer_mod->is_serializer(found) == true);
    }
}

TEST_CASE("Serializer Module: Serialization and Deserialization", "[serializers module test]") {
    flecs::world world;
    world.import <spectre::modules::serializer_module>();
    auto* serializer_mod = &world.get_mut<spectre::modules::serializer_module>();

    // Setup a valid dummy serializer
    spectre_serializer_component valid_serializer{};
    valid_serializer.serialize = [](ecs_world_t*, ecs_entity_t entity) -> sandbox_properties_handle_t {
        sandbox::properties p;
        p.set<uint64_t>("serialized_id", entity);
        sandbox_properties_handle_t handle = p.get_raw();
        p.release(); // Relinquish ownership so it doesn't get destroyed
        return handle;
    };
    valid_serializer.deserialize = [](ecs_world_t* w, ecs_entity_t target, sandbox_properties_handle_t props_handle) {
        sandbox::properties p(props_handle, false); // Don't own it, just read
        uint64_t id = 0;
        if (p.get<uint64_t>("serialized_id", id)) {
            flecs::world(w).entity(target).set<test_id_t>({id});
        }
    };

    serializer_mod->register_serializer("valid_serializer", valid_serializer);
    flecs::entity valid_serializer_entity = serializer_mod->find_serializer("valid_serializer");
    REQUIRE(valid_serializer_entity.is_valid());

    flecs::entity dummy_entity = world.entity();

    SECTION("serialize_entity returns invalid properties for invalid arguments") {
        flecs::entity invalid_entity = flecs::entity::null();

        sandbox::properties res1 = serializer_mod->serialize_entity(invalid_entity, dummy_entity);
        REQUIRE(res1.is_valid() == false);

        sandbox::properties res2 = serializer_mod->serialize_entity(valid_serializer_entity, invalid_entity);
        REQUIRE(res2.is_valid() == false);
    }

    SECTION("serialize_entity successfully serializes an entity") {
        sandbox::properties serialized_props = serializer_mod->serialize_entity(valid_serializer_entity, dummy_entity);

        REQUIRE(serialized_props.is_valid() == true);
        uint64_t id = 0;
        REQUIRE(serialized_props.get<uint64_t>("serialized_id", id) == true);
        REQUIRE(id == dummy_entity.id());
    }

    SECTION("deserialize_entity handles invalid arguments") {
        flecs::entity invalid_entity = flecs::entity::null();
        sandbox::properties valid_props;
        valid_props.set<uint64_t>("serialized_id", dummy_entity.id());
        sandbox::properties invalid_props;

        flecs::entity target = world.entity();
        serializer_mod->deserialize_entity(invalid_entity, target, valid_props);
        REQUIRE(target.has<test_id_t>() == false);

        serializer_mod->deserialize_entity(valid_serializer_entity, target, invalid_props);
        REQUIRE(target.has<test_id_t>() == false);
    }

    SECTION("deserialize_entity successfully deserializes an entity") {
        sandbox::properties valid_props;
        valid_props.set<uint64_t>("serialized_id", dummy_entity.id());

        flecs::entity target = world.entity();
        serializer_mod->deserialize_entity(valid_serializer_entity, target, valid_props);

        REQUIRE(target.has<test_id_t>() == true);
        const test_id_t* val = target.try_get<test_id_t>();
        REQUIRE(val != nullptr);
        REQUIRE(val->id == dummy_entity.id());
    }
}
