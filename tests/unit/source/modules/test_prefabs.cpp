#include <catch2/catch_all.hpp>
#include <flecs.h>
#include "modules/components/components_module.h"
#include "modules/serializer/serializer_module.h"
#include "modules/prefabs/prefabs_module.h"
#include "spectre/components.h"
#include "sandbox/sdk/properties.hpp"

using namespace spectre::modules;

// Register a fake transform component for testing components serialization
ecs_entity_t test_transform_registration(ecs_world_t* world) {
    flecs::world fw(world);
    return fw.component<spectre_2D_transform_component_t>("spectre_2D_transform_component_t").id();
}

TEST_CASE("Prefabs Module: Serialization and Deserialization", "[prefabs module test]") {
    flecs::world world;
    world.import<spectre::modules::serializer_module>();
    world.import<spectre::modules::components_module_t>();
    world.import<spectre::modules::prefabs_module_t>();

    auto* prefabs_mod = &world.get_mut<spectre::modules::prefabs_module_t>();
    auto* components_mod = &world.get_mut<spectre::modules::components_module_t>();

    // Setup transform serializer
    spectre_serializer_component transform_serializer{};
    transform_serializer.serialize = [](ecs_world_t* w, ecs_entity_t entity) -> sandbox_properties_handle_t {
        flecs::world fw(w);
        flecs::entity ent = fw.entity(entity);
        const auto* transform = ent.try_get<spectre_2D_transform_component_t>();
        
        sandbox::properties p;
        if (transform) {
            p.set<double>("x", transform->position_x);
            p.set<double>("y", transform->position_y);
        }
        sandbox_properties_handle_t handle = p.get_raw();
        p.release();
        return handle;
    };
    
    transform_serializer.deserialize = [](ecs_world_t* w, sandbox_properties_handle_t props_handle) -> ecs_entity_t {
        flecs::world fw(w);
        sandbox::properties p(props_handle, false);
        
        spectre_2D_transform_component_t transform{};
        double x = 0, y = 0;
        p.get<double>("x", x);
        p.get<double>("y", y);
        transform.position_x = static_cast<float>(x);
        transform.position_y = static_cast<float>(y);
        
        flecs::entity e = fw.entity().set<spectre_2D_transform_component_t>(transform);
        return e.id();
    };

    components_mod->register_component("spectre_2D_transform_component_t", test_transform_registration, transform_serializer);

    SECTION("register_prefab and create_entity_from_prefab") {
        sandbox::properties prefab_props;
        prefab_props.set<std::string>("name", "BasePrefab");
        
        sandbox::properties comp_props;
        comp_props.set<double>("x", 100.0);
        comp_props.set<double>("y", 50.0);
        
        sandbox::properties comps_obj;
        comps_obj.merge("spectre_2D_transform_component_t", comp_props);
        prefab_props.merge("components", comps_obj);

        prefabs_mod->register_prefab("BasePrefab", std::move(prefab_props));
        
        REQUIRE(prefabs_mod->has_prefab("BasePrefab") == true);
        
        flecs::entity prefab_ent = prefabs_mod->find_prefab("BasePrefab");
        REQUIRE(prefab_ent.is_valid() == true);
        REQUIRE(prefabs_mod->is_prefab(prefab_ent) == true);
        
        const auto* transform = prefab_ent.try_get<spectre_2D_transform_component_t>();
        REQUIRE(transform != nullptr);
        REQUIRE(transform->position_x == Catch::Approx(100.0f));
        REQUIRE(transform->position_y == Catch::Approx(50.0f));
        
        flecs::entity instance = prefabs_mod->create_entity("BasePrefab");
        REQUIRE(instance.is_valid() == true);
        REQUIRE(instance.has(flecs::IsA, prefab_ent) == true);
        
        const auto* inst_transform = instance.try_get<spectre_2D_transform_component_t>();
        REQUIRE(inst_transform != nullptr);
        REQUIRE(inst_transform->position_x == Catch::Approx(100.0f));
        REQUIRE(inst_transform->position_y == Catch::Approx(50.0f));
    }

    SECTION("serialize_entity recursively serializes prefabs, components, and children") {
        sandbox::properties base_prefab_props;
        base_prefab_props.set<std::string>("name", "BasePrefab2");
        prefabs_mod->register_prefab("BasePrefab2", std::move(base_prefab_props));

        flecs::entity parent = world.entity("ParentEntity");
        parent.is_a(prefabs_mod->find_prefab("BasePrefab2"));
        parent.set<spectre_2D_transform_component_t>({10.0f, 20.0f, 0.0f, 1.0f, 1.0f});

        flecs::entity child1 = world.entity("Child1").child_of(parent);
        child1.set<spectre_2D_transform_component_t>({5.0f, 5.0f, 0.0f, 1.0f, 1.0f});

        sandbox::properties serialized = prefabs_mod->serialize_entity(parent);
        REQUIRE(serialized.is_valid() == true);

        std::string name;
        REQUIRE(serialized.get<std::string>("name", name) == true);
        REQUIRE(name == "ParentEntity");

        std::vector<std::string> prefabs_list;
        REQUIRE(serialized.get_array<std::string>("prefabs", prefabs_list) == true);
        REQUIRE(prefabs_list.size() == 1);
        REQUIRE(prefabs_list[0] == "BasePrefab2");

        REQUIRE(serialized.has("components") == true);
        sandbox::properties comps = serialized.sub("components");
        REQUIRE(comps.has("spectre_2D_transform_component_t") == true);
        sandbox::properties comp_props_sub = comps.sub("spectre_2D_transform_component_t");
        double px = 0, py = 0;
        comp_props_sub.get<double>("x", px);
        comp_props_sub.get<double>("y", py);
        REQUIRE(px == Catch::Approx(10.0));
        REQUIRE(py == Catch::Approx(20.0));

        REQUIRE(serialized.has("children") == true);
        sandbox::properties children_node = serialized.sub("children");
        REQUIRE(children_node.has("Child1") == true);
        sandbox::properties child_props = children_node.sub("Child1");
        REQUIRE(child_props.has("components") == true);
        sandbox::properties child_comps = child_props.sub("components");
        REQUIRE(child_comps.has("spectre_2D_transform_component_t") == true);
    }
}
