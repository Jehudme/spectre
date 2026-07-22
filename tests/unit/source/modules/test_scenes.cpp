#include "../../../../spectre/source/modules/scripts/scripts_module.h"
#include "modules/prefabs/prefabs_module.h"
#include "modules/scenes/scenes_module.h"
#include "modules/serializer/serializer_module.h"
#include "sandbox/sdk/properties.hpp"
#include "spectre/spectre.h"
#include <catch2/catch_all.hpp>
#include <flecs.h>
#include <fstream>
#include <iostream>

using namespace spectre::modules;

TEST_CASE("Scenes Module: Serialization and Deserialization", "[modules][scenes]") {
    flecs::world world;

    world.import <serializer_module>();
    world.import <script_module_t>();
    world.import <prefabs_module_t>();
    world.import <scenes_module_t>();
    auto* scripts_mod = const_cast<script_module_t*>(world.try_get<script_module_t>());
    auto* scenes_module = const_cast<scenes_module_t*>(&world.get_mut<scenes_module_t>());

    SECTION("state and scene serialization") {
        std::string lua_code = R"(
g_world = g_world or nil
local ecs = { Script = {} }
function ecs.Script.define(func, ...)
    local args = {}
    for i, arg in ipairs({...}) do
        local name, arg_type = string.match(arg, "^([%w_]+):([%w_]+)$")
        table.insert(args, { name = name, type = arg_type })
    end
    return { func = func, args = args }
end

local Test = {}
function Test.test_scene_create() end
function Test.test_state_enter() end

return {
    test_scene_create = ecs.Script.define(Test.test_scene_create),
    test_state_enter = ecs.Script.define(Test.test_state_enter)
}
        )";

        scripts_mod->eval_code(lua_code, "test_scene_script");

        std::string scene_json = R"({
            "name": "Level1",
            "scripts": {
                "on_create": {
                    "0": { "function": "test_scene_create" }
                }
            }
        })";
        sandbox::properties scene_props(scene_json, sandbox::properties::Format::JSON);

        scenes_module->register_scene(std::move(scene_props));
        REQUIRE(scenes_module->has_scene("Level1"));

        std::string state_json = R"({
            "name": "GameState",
            "scripts": {
                "on_enter": {
                    "0": { "function": "test_state_enter" }
                }
            },
            "scenes": ["Level1"]
        })";
        sandbox::properties state_props(state_json, sandbox::properties::Format::JSON);

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
