#include <catch2/catch_all.hpp>
#include <flecs.h>
#include "modules/components/components_module.h"
#include "modules/serializer/serializer_module.h"
#include "modules/prefabs/prefabs_module.h"
#include "modules/scripts/scripts_module.h"
#include "modules/scenes/scenes_module.h"
#include "spectre/spectre.h"
#include "sandbox/sdk/properties.hpp"

using namespace spectre::modules;

TEST_CASE("Scenes Module: Serialization and Deserialization", "[modules][scenes]") {
    flecs::world world;
    
    world.import<components_module_t>();
    world.import<serializer_module>();
    world.import<script_module_t>();
    world.import<prefabs_module_t>();
    world.import<scenes_module_t>();
    
    auto* scenes_module = const_cast<scenes_module_t*>(&world.get_mut<scenes_module_t>());
    
    SECTION("state and scene serialization") {
        sandbox::properties scene_props;
        scene_props.set<std::string>("name", "Level1");
        
        sandbox::properties scene_scripts;
        sandbox::properties on_create;
        on_create.set<std::string>("test_scene_create", "test_scene_create");
        scene_scripts.merge("on_create", on_create);
        scene_props.merge("scripts", scene_scripts);
        
        scenes_module->register_scene(std::move(scene_props));
        REQUIRE(scenes_module->has_scene("Level1"));
        
        sandbox::properties state_props;
        state_props.set<std::string>("name", "GameState");
        
        sandbox::properties state_scripts;
        sandbox::properties on_enter;
        on_enter.set<std::string>("test_state_enter", "test_state_enter");
        state_scripts.merge("on_enter", on_enter);
        state_props.merge("scripts", state_scripts);
        
        sandbox::properties scenes_arr;
        scenes_arr.set<std::string>("0", "Level1");
        state_props.merge("scenes", scenes_arr);
        
        scenes_module->register_state(std::move(state_props));
        REQUIRE(scenes_module->has_state("GameState"));
        
        flecs::entity state_prefab = scenes_module->find_state("GameState");
        REQUIRE(state_prefab.is_valid());
        
        scenes_module->push_state(state_prefab);
        
        flecs::entity current = scenes_module->find_current_state();
        REQUIRE(current.is_valid());
        REQUIRE(scenes_module->is_state(current));
        REQUIRE(current.target(flecs::IsA).name() == flecs::string_view("GameState"));
        
        flecs::query<> q_scenes = scenes_module->find_current_scenes();
        int scene_count = 0;
        q_scenes.each([&](flecs::entity e) {
            scene_count++;
            REQUIRE(e.has<spectre_state_use_scene_relation_t>());
        });
        REQUIRE(scene_count == 1);
        q_scenes.destruct();
        
        scenes_module->pop_state();
        REQUIRE(!scenes_module->find_current_state().is_valid());
    }
}
