#include <catch2/catch_test_macros.hpp>
#include <flecs.h>
#include <sandbox/sdk/engine.hpp>
#include "../../spectre/source/modules/scripts/scripts_module.hpp"
#include <spectre/abi/spectre_window_input_abi.h>
#include <spectre/abi/renderer_components.h>
#include <sandbox/sdk/filesystem.hpp>

using namespace spectre::module;

TEST_CASE("Scripts Module Integration", "[scripts]") {
    sandbox::engine engine;
    sandbox::properties props;

    // We mount a temporary directory for testing
    props.set("filesystem/mounts/app/physical", "/tmp/spectre_tests/app");
    props.set("filesystem/mounts/app/readonly", false);

    // Provide the modules needed to run the ScriptsModule
    std::vector<std::string> modules = {
        "sandbox-configuration@1.0.0", 
        "sandbox-logs@1.0.0", 
        "sandbox-filesystem@1.0.0"
    };
    props.set_array("engine/sandbox", modules);

    // Setup the mock script file on the filesystem
    std::system("mkdir -p /tmp/spectre_tests/app/state/scripts");
    std::system("cp /home/jehud/CLionProjects/spectre/tests/application/resources/app/state/scripts/player_movement.lua /tmp/spectre_tests/app/state/scripts/");

    REQUIRE(engine.initialize(props) == true);
    flecs::world ecs(static_cast<ecs_world_t*>(engine.get_ecs()));

    // Register ABI structs in Flecs manually for this test since we are not booting all spectre modules
    ecs.component<spectre_input_state_t>("spectre_input_state_t");
    ecs.component<spectre_transform_2d_t>("spectre_transform_2d_t");

    // Initialize mock input state
    spectre_input_state_t input_state = {0};
    // Simulate 'W' key down (Key 87)
    input_state.keys_down[87 / 64] |= (1ULL << (87 % 64));
    ecs.set<spectre_input_state_t>(input_state);

    // Create player entity
    flecs::entity player = ecs.entity("Player")
        .set<spectre_transform_2d_t>({ {100.0f, 200.0f}, 0.0f, {1.0f, 1.0f} });

    // Initialize ScriptsModule
    ScriptsModule scripts_module(ecs);

    // Load the lua script
    scripts_module.load_script("app://state/scripts/player_movement.lua");

    // Execute the hook
    scripts_module.execute_script("on_update", player);

    // Assert that the player's position was mutated!
    const spectre_transform_2d_t* transform = player.get<spectre_transform_2d_t>();
    REQUIRE(transform != nullptr);
    REQUIRE(transform->position[1] == Catch::Approx(190.0f)); // 200.0 - 10.0
}
