#include <catch2/catch_all.hpp>
#include <flecs.h>
#include "modules/serializer/serializer_module.h"
#include "modules/prefabs/prefabs_module.h"
#include "spectre/spectre.h"
#include "sandbox/sdk/properties.hpp"
#include "modules/scripts/scripts_module.h"
#include <fstream>
#include <filesystem>
#include <iostream>


using namespace spectre::modules;

// Register a fake transform component for testing components serialization
ecs_entity_t test_transform_registration(ecs_world_t* world) {
    flecs::world fw(world);
    auto c = fw.component<spectre_2D_transform_component_t>("spectre_2D_transform_component_t");
    return c.id();
}

TEST_CASE("Prefabs Module: Serialization and Deserialization", "[prefabs module test]") {
    flecs::world world;
    world.import<spectre::modules::serializer_module>();
    world.import<spectre::modules::script_module_t>();
    auto* scripts_mod = &world.get_mut<spectre::modules::script_module_t>();
    world.import<spectre::modules::prefabs_module_t>();


    auto* prefabs_mod = &world.get_mut<spectre::modules::prefabs_module_t>();

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
    
    transform_serializer.deserialize = [](ecs_world_t* w, ecs_entity_t target, sandbox_properties_handle_t props_handle) {
        flecs::world fw(w);
        sandbox::properties p(props_handle, false);
        
        spectre_2D_transform_component_t transform{};
        double x = 0, y = 0;
        p.get<double>("x", x);
        p.get<double>("y", y);
        transform.position_x = static_cast<float>(x);
        transform.position_y = static_cast<float>(y);
        
        fw.entity(target).set<spectre_2D_transform_component_t>(transform);
    };

    test_transform_registration(world.c_ptr());
    auto* serializer_mod = &world.get_mut<spectre::modules::serializer_module>();
    serializer_mod->register_serializer("spectre_2D_transform_component_t", transform_serializer);

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

    SECTION("serialize and deserialize entity with scripts") {
        std::string lua_code = R"(
--- @param number
function on_create_test(value)
    print("on_create_test called with value " .. tostring(value))
end

--- @param number
function on_destroy_test(value)
    print("on_destroy_test called with value " .. tostring(value))
end
        )";

        std::string temp_file = "temp_prefab_script.lua";
        std::ofstream out(temp_file);
        out << lua_code;
        out.close();

        scripts_mod->include_code(temp_file);

        sandbox::properties prefab_props;
        prefab_props.set<std::string>("name", "ScriptedPrefab");
        
        sandbox::properties scripts_node;
        
        sandbox::properties on_create_arr;
        sandbox::properties create_script;
        create_script.set<std::string>("function", "on_create_test");
        sandbox::properties create_args;
        create_args.set<double>("value", 42.0);
        create_script.merge("arguments", create_args);
        on_create_arr.merge("0", create_script);
        
        sandbox::properties on_destroy_arr;
        sandbox::properties destroy_script;
        destroy_script.set<std::string>("function", "on_destroy_test");
        sandbox::properties destroy_args;
        destroy_args.set<double>("value", 99.0);
        destroy_script.merge("arguments", destroy_args);
        on_destroy_arr.merge("0", destroy_script);

        scripts_node.merge("on_create", on_create_arr);
        scripts_node.merge("on_destroy", on_destroy_arr);
        
        prefab_props.merge("scripts", scripts_node);

        prefabs_mod->register_prefab("ScriptedPrefab", std::move(prefab_props));
        REQUIRE(prefabs_mod->has_prefab("ScriptedPrefab") == true);
        
        flecs::entity instance = prefabs_mod->create_entity("ScriptedPrefab");
        REQUIRE(instance.is_valid() == true);
        
        // At this point on_create_test should have been called and logged via lua print (which isn't caught by the test, but we can verify relation existence)
        bool has_on_create = false;
        int index = 0;
        
        while (flecs::entity s_ent = instance.target<spectre_use_script_on_create_relation_t>(index++)) {
            const auto* rel_ptr = instance.try_get<spectre_use_script_on_create_relation_t>(s_ent);
            if (!rel_ptr) continue;
            const auto& rel = *rel_ptr;
            has_on_create = true;
            REQUIRE(s_ent.name() == "on_create_test");
            REQUIRE(rel.argument_count == 1);
            REQUIRE(rel.arguments[0].value.number_value == Catch::Approx(42.0));
        }
        REQUIRE(has_on_create == true);

        // Serialize the instance
        sandbox::properties serialized = prefabs_mod->serialize_entity(instance);
        REQUIRE(serialized.is_valid() == true);
        
        REQUIRE(serialized.has("scripts") == true);
        sandbox::properties s_scripts = serialized.sub("scripts");
        REQUIRE(s_scripts.has("on_create") == true);
        sandbox::properties s_on_create = s_scripts.sub("on_create");
        REQUIRE(s_on_create.has("0") == true);
        
        std::string func_name;
        s_on_create.sub("0").get<std::string>("function", func_name);
        REQUIRE(func_name == "on_create_test");

        // Destroy instance (should trigger on_destroy)
        instance.destruct();

        std::filesystem::remove(temp_file);
    }

}
