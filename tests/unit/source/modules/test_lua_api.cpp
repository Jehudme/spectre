#include "sandbox/sandbox.h"
#include "spectre/spectre.h"
#include "../../../../spectre/source/modules/scripts/scripts_module.h"
#include "../../../../spectre/source/modules/components/components_module.h"
#include <catch2/catch_all.hpp>
#include <flecs.h>
#include <filesystem>

using namespace spectre::modules;

TEST_CASE("Lua API wrappers", "[lua api]") {
    flecs::world world;
    
    world.import<spectre::modules::components_module_t>();
    world.import<spectre::modules::script_module_t>();
    
    auto* scripts_mod = const_cast<script_module_t*>(world.try_get<script_module_t>());
    REQUIRE(scripts_mod != nullptr);

    SECTION("API tests script") {
        std::string raw_lua_code = std::string("g_world = g_world or nil\n") +
                                   "package.path = package.path .. ';" + std::string(APP_RESOURCES_DIR) + "/resources/scripts/?.lua'\n" +
                                   "require('api_tests')\n";

        std::filesystem::remove("api_test_success.txt");
        scripts_mod->eval_code(raw_lua_code, "api_tests");
        REQUIRE(std::filesystem::exists("api_test_success.txt"));
    }
}
