#include "sandbox/sandbox.h"
#include "spectre/spectre.h"
#include "../../../../spectre/source/modules/scripts/scripts_module.h"
#include <catch2/catch_all.hpp>
#include <flecs.h>

using namespace spectre::modules;

TEST_CASE("Lua API wrappers", "[lua api]") {
    flecs::world world;
    
    // Import required modules to expose the APIs and C functions to the engine
    world.import<spectre::modules::script_module_t>();
    
    auto* scripts_mod = const_cast<script_module_t*>(world.try_get<script_module_t>());
    REQUIRE(scripts_mod != nullptr);

    SECTION("Properties wrapper") {
        std::string lua_code = R"(
g_world = g_world or nil
package.path = package.path .. ";]]" + std::string(APP_RESOURCES_DIR) + R"(/resources/assets/scripts/?.lua"
local sandbox = require("externals.sandbox")

local Test = {}
function Test.test_properties()
    local props = sandbox.Properties.new()
    local json = "{\"hello\": 42}"
    local loaded = props:load(json, 0)
    
    if loaded then
        print("Properties loaded successfully.")
    else
        print("Failed to load properties.")
    end
    
    local has_hello = props:has("hello")
    if has_hello then
        print("Has 'hello' property.")
    else
        print("Does not have 'hello' property.")
    end
    
    local dump_str = props:dump(0)
    print("Dump: " .. (dump_str or "nil"))
    
    props:destroy()
end

return {
    test_properties = ecs.Script.define(Test.test_properties)
}
        )";

        // Note: the test script doesn't have an ecs.Script.define if we don't declare it.
        // Let's just execute lua manually or via eval_code without defining a script.
        // The script_module allows eval_code.
        
        std::string raw_lua_code = R"(
g_world = g_world or nil
package.path = package.path .. ";)" + std::string(APP_RESOURCES_DIR) + R"(/resources/assets/scripts/?.lua"
local sandbox = require("externals.sandbox")
local spectre = require("externals.spectre")

local props = sandbox.Properties.new()
local json = "{\"hello\": 42}"
local loaded = props:load(json, 0)
assert(loaded, "Failed to load properties")
assert(props:has("hello"), "Property 'hello' not found")
local dump_str = props:dump(0)
assert(dump_str ~= nil, "Dump failed")
props:destroy()

-- Test logs (should output to console/logs)
sandbox.logs.info(g_world, "Testing sandbox_logs_info via wrapper!")

-- Test spectre component API
local ent = ffi.C.ecs_new_id(g_world)
local found = spectre.components.is_component(g_world, ent)
assert(found == false, "Empty entity should not be a component")
        )";

        scripts_mod->eval_code(raw_lua_code, "test_lua_api");
    }
}
