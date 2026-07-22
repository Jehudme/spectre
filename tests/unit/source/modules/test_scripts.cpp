#include "../../../../spectre/source/modules/scripts/scripts_module.h"
#include "modules/serializer/serializer_module.h"
#include "spectre/spectre.h"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <flecs.h>
#include <fstream>

using namespace spectre::modules;

TEST_CASE("Scripts Module: New Architecture", "[scripts module test]") {
    flecs::world world;
    world.import<spectre::modules::serializer_module>();
    world.import<script_module_t>();

    auto* scripts_mod = const_cast<script_module_t*>(world.try_get<script_module_t>());
    REQUIRE(scripts_mod != nullptr);

    SECTION("Include and Execute Lua Function with ecs.Script.define") {
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
function Test.on_update(entity_id, a, b)
    print("Test.on_update: entity=" .. tostring(entity_id) .. ", a=" .. tostring(a) .. ", b=" .. tostring(b))
end

return {
    on_update = ecs.Script.define(Test.on_update, "a:number", "b:number")
}
        )";

        scripts_mod->eval_code(lua_code, "test_new_script");

        REQUIRE(scripts_mod->has_script("on_update", {}));

        flecs::entity on_update_ent = scripts_mod->find_script("on_update");
        REQUIRE(on_update_ent.is_valid());

        const auto* script_comp = on_update_ent.try_get<spectre_script_t>();
        REQUIRE(script_comp != nullptr);
        REQUIRE(script_comp->argument_count == 2);
        REQUIRE(std::string(script_comp->arguments_name[0]) == "a");
        REQUIRE(std::string(script_comp->arguments_name[1]) == "b");
        REQUIRE(script_comp->argument_types[0] == SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER);
        REQUIRE(script_comp->argument_types[1] == SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER);

        script_arguments_t args(2);
        args[0].type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER;
        args[0].value.number_value = 10.5;
        args[1].type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER;
        args[1].value.number_value = 5.0;

        scripts_mod->execute_script("on_update", args);
    }

    SECTION("Context Token Resolution via JSON Deserialization") {
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
function Test.check_tokens(entity_id, self_ent, scene_ent, state_ent)
    print("Test.check_tokens: entity_id=" .. tostring(entity_id) .. ", self=" .. tostring(self_ent) .. ", scene=" .. tostring(scene_ent) .. ", state=" .. tostring(state_ent))
end

return {
    check_tokens = ecs.Script.define(Test.check_tokens, "self_ent:integer", "scene_ent:integer", "state_ent:integer")
}
        )";

        scripts_mod->eval_code(lua_code, "temp_test_tokens");

        flecs::entity state_entity = world.entity("MyState");
        state_entity.set<spectre_state_context_t>({state_entity});

        flecs::entity scene_entity = world.entity("MyScene");
        scene_entity.child_of(state_entity);
        scene_entity.set<spectre_scene_context_t>({scene_entity});

        flecs::entity target_entity = world.entity("MyTarget");
        target_entity.child_of(scene_entity);

        std::string json_props = R"({
            "on_update": {
                "0": {
                    "function": "check_tokens",
                    "arguments": {
                        "self_ent": "%self%",
                        "scene_ent": "%scene%",
                        "state_ent": "%state%"
                    }
                }
            }
        })";
        
        sandbox::properties props(json_props, sandbox::properties::Format::JSON);
        scripts_mod->deserialize_scripts(target_entity, std::move(props));
        
        const auto* rel = target_entity.try_get<spectre_use_script_on_update_relation_t>(scripts_mod->find_script("check_tokens"));
        REQUIRE(rel != nullptr);
        REQUIRE(rel->argument_count == 3);
        REQUIRE(rel->arguments[0].type == SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER);
        REQUIRE(rel->arguments[1].type == SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER);
        REQUIRE(rel->arguments[2].type == SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER);

        REQUIRE(rel->arguments[0].value.integer_value == target_entity.id());
        REQUIRE(rel->arguments[1].value.integer_value == scene_entity.id());
        REQUIRE(rel->arguments[2].value.integer_value == state_entity.id());

        scripts_mod->execute_on_update(target_entity);
    }
}
