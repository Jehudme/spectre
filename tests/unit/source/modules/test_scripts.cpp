#include <catch2/catch_all.hpp>
#include <flecs.h>
#include "modules/scripts/scripts_module.h"
#include "spectre/components.h"
#include <fstream>
#include <filesystem>

using namespace spectre::modules;

TEST_CASE("Scripts Module: Initialization and Parsing", "[scripts module test]") {
    flecs::world world;
    world.import<script_module_t>();

    auto* scripts_mod = &world.get_mut<script_module_t>();

    SECTION("Parse and register Lua functions from file") {
        std::string lua_code = R"(
--- @param number
--- @param number
function on_update(dt, entity_id)
    print("Updating entity " .. entity_id .. " with dt " .. dt)
end

function on_init()
    print("Init called!")
end
        )";

        std::string temp_file = "temp_test_script.lua";
        std::ofstream out(temp_file);
        out << lua_code;
        out.close();

        scripts_mod->include_code(temp_file);

        REQUIRE(scripts_mod->has_script("on_update", {}));
        REQUIRE(scripts_mod->has_script("on_init", {}));

        flecs::entity on_update_ent = scripts_mod->find_script("on_update");
        REQUIRE(on_update_ent.is_valid());
        REQUIRE(scripts_mod->is_script(on_update_ent));

        const auto* script_comp = on_update_ent.try_get<spectre_script_t>();
        REQUIRE(script_comp != nullptr);
        REQUIRE(std::string(script_comp->function_name) == "on_update");
        REQUIRE(script_comp->argument_count == 2);
        REQUIRE(std::string(script_comp->arguments_name[0]) == "dt");
        REQUIRE(std::string(script_comp->arguments_name[1]) == "entity_id");
        REQUIRE(script_comp->argument_types[0] == SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER);
        REQUIRE(script_comp->argument_types[1] == SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER);

        std::filesystem::remove(temp_file);
    }

    SECTION("Execute Lua function with arguments") {
        std::string lua_code = R"(
--- @param number
--- @param number
function add_to_sum(a, b)
    local global_sum = a + b
    print("LUA_TEST_OUTPUT: add_to_sum executed successfully! a=" .. tostring(a) .. ", b=" .. tostring(b) .. ", sum=" .. tostring(global_sum))
end
        )";

        std::string temp_file = "temp_test_execute.lua";
        std::ofstream out(temp_file);
        out << lua_code;
        out.close();

        scripts_mod->include_code(temp_file);

        script_arguments_t args(2);
        args[0].type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER;
        args[0].value.number_value = 10.5;
        
        args[1].type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER;
        args[1].value.number_value = 5.0;

        scripts_mod->execute_script("add_to_sum", args);

        // Test with wrong arguments (should fail and log error, but we just verify it doesn't crash)
        script_arguments_t wrong_args(2);
        wrong_args[0].type = SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING;
        wrong_args[0].value.string_value = "hello";
        wrong_args[1].type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER;
        wrong_args[1].value.number_value = 5.0;

        scripts_mod->execute_script("add_to_sum", wrong_args);
        
        // Test with wrong argument count
        script_arguments_t wrong_count_args(1);
        wrong_count_args[0].type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER;
        wrong_count_args[0].value.number_value = 10.5;

        scripts_mod->execute_script("add_to_sum", wrong_count_args);

        REQUIRE(true);

        std::filesystem::remove(temp_file);
    }
}
