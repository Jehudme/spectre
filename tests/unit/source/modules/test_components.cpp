#include <catch2/catch_all.hpp>
#include <flecs.h>
#include "modules/components/components_module.h"
#include "modules/serializer/serializer_module.h"
#include "spectre/spectre.h"
#include "sandbox/sdk/properties.hpp"

using namespace spectre::modules;

TEST_CASE("Components Module: Registration and Queries", "[components module test]") {
    flecs::world world;
    world.import<spectre::modules::components_module_t>();
    world.import<spectre::modules::serializer_module>();

    auto* comp_mod = &world.get_mut<spectre::modules::components_module_t>();
    REQUIRE(comp_mod != nullptr);

    SECTION("has_component returns false for empty or unregistered type") {
        REQUIRE(comp_mod->has_component("") == false);
        REQUIRE(comp_mod->has_component("non_existent_component") == false);
    }

    SECTION("register_component correctly registers and finds component") {
        spectre_serializer_component valid_serializer{};
        valid_serializer.serialize = [](ecs_world_t*, ecs_entity_t) -> sandbox_properties_handle_t { return {0}; };
        valid_serializer.deserialize = [](ecs_world_t*, sandbox_properties_handle_t) -> ecs_entity_t { return 0; };

        component_registration_fn registration = [](ecs_world_t* w) -> ecs_entity_t {
            flecs::world fw(w);
            return fw.component<spectre_2D_transform_component_t>("spectre_2D_transform_component_t")
                .member<float>("position_x")
                .member<float>("position_y")
                .id();
        };

        REQUIRE(comp_mod->has_component("spectre_2D_transform_component_t") == false);

        comp_mod->register_component("spectre_2D_transform_component_t", registration, valid_serializer);

        REQUIRE(comp_mod->has_component("spectre_2D_transform_component_t") == true);
        
        flecs::entity comp = world.lookup("::components::spectre_2D_transform_component_t");
        REQUIRE(comp.is_valid() == true);
        REQUIRE(comp_mod->is_component(comp) == true);
    }
}

TEST_CASE("Components Module: Serialization and Deserialization with Real Component", "[components module test]") {
    flecs::world world;
    world.import<spectre::modules::serializer_module>();
    world.import<spectre::modules::components_module_t>();
    auto* comp_mod = &world.get_mut<spectre::modules::components_module_t>();

    spectre_serializer_component transform_serializer{};
    transform_serializer.serialize = [](ecs_world_t* w, ecs_entity_t entity) -> sandbox_properties_handle_t {
        flecs::world fw(w);
        flecs::entity ent = fw.entity(entity);
        const auto* transform = ent.try_get<spectre_2D_transform_component_t>();
        
        sandbox::properties p;
        if (transform) {
            p.set<double>("position_x", transform->position_x);
            p.set<double>("position_y", transform->position_y);
        }
        
        sandbox_properties_handle_t handle = p.get_raw();
        p.release();
        return handle;
    };
    
    transform_serializer.deserialize = [](ecs_world_t* w, sandbox_properties_handle_t props_handle) -> ecs_entity_t {
        flecs::world fw(w);
        sandbox::properties p(props_handle, false);
        
        spectre_2D_transform_component_t transform = {0};
        double px = 0.0, py = 0.0;
        
        if (p.get<double>("position_x", px)) transform.position_x = static_cast<float>(px);
        if (p.get<double>("position_y", py)) transform.position_y = static_cast<float>(py);
        
        flecs::entity new_ent = fw.entity();
        new_ent.set<spectre_2D_transform_component_t>(transform);
        
        return new_ent.id();
    };

    component_registration_fn registration = [](ecs_world_t* w) -> ecs_entity_t {
        flecs::world fw(w);
        return fw.component<spectre_2D_transform_component_t>("spectre_2D_transform_component_t")
            .member<float>("position_x")
            .member<float>("position_y")
            .member<float>("position_z")
            .id();
    };

    comp_mod->register_component("spectre_2D_transform_component_t", registration, transform_serializer);

    REQUIRE(comp_mod->has_component("spectre_2D_transform_component_t") == true);
    
    flecs::entity target_entity = world.entity();
    target_entity.set<spectre_2D_transform_component_t>({10.5f, 20.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f});

    SECTION("serialize_component successfully serializes actual component data") {
        sandbox::properties serialized_props = comp_mod->serialize_component(target_entity);
        
        REQUIRE(serialized_props.is_valid() == true);
        
        sandbox::properties comp_props = serialized_props.sub("spectre_2D_transform_component_t");
        REQUIRE(comp_props.is_valid() == true);
        
        double px = 0, py = 0;
        REQUIRE(comp_props.get<double>("position_x", px) == true);
        REQUIRE(comp_props.get<double>("position_y", py) == true);
        
        REQUIRE(px == Catch::Approx(10.5));
        REQUIRE(py == Catch::Approx(20.5));
    }

    SECTION("deserialize_component successfully deserializes to an entity with the component") {
        sandbox::properties valid_props;
        valid_props.set<double>("position_x", 42.0);
        valid_props.set<double>("position_y", -15.5);
        
        flecs::entity deserialized = comp_mod->deserialize_component("spectre_2D_transform_component_t", std::move(valid_props));
        
        REQUIRE(deserialized.is_valid() == true);
        const auto* transform = deserialized.try_get<spectre_2D_transform_component_t>();
        REQUIRE(transform != nullptr);
        REQUIRE(transform->position_x == Catch::Approx(42.0f));
        REQUIRE(transform->position_y == Catch::Approx(-15.5f));
    }
}
