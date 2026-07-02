#include <catch2/catch_test_macros.hpp>
#include <flecs.h>
#include <sandbox/sdk/engine.hpp>
#include <spectre/abi/prefabs_service.h>
#include <sandbox/sdk/properties.hpp>

// A dummy component for testing
struct DummyComponent {
    int value;
};

static void factory_Dummy(ecs_world_t* ecs, ecs_entity_t e, sandbox_properties_handle_t props_h) {
    sandbox::properties props(props_h, false);
    DummyComponent comp;
    comp.value = (int)props.get<int64_t>("value").value_or(0);
    flecs::world(ecs).entity(e).set<DummyComponent>(comp);
}

TEST_CASE("Prefabs Module Initialization and Core Functions", "[prefabs]") {
    sandbox::engine engine;
    sandbox::properties props;

    // Define the APP_RESOURCES_DIR in CMake or we can use the relative path directly
    props.set("filesystem/mounts/app/physical", APP_RESOURCES_DIR);
    props.set("filesystem/mounts/app/readonly", true);

    std::vector<std::string> modules = {
        "sandbox-configuration@1.0.0", 
        "sandbox-logs@1.0.0", 
        "sandbox-filesystem@1.0.0",
        "spectre-prefabs@1.0.0"
    };
    props.set_array("engine/sandbox", modules);

    REQUIRE(engine.initialize(props) == true);
    flecs::world ecs(static_cast<ecs_world_t*>(engine.get_ecs()));
    
    const auto* prefabs_api = SANDBOX_GET_SERVICE(ecs, spectre_prefabs_service_t);
    REQUIRE(prefabs_api != nullptr);
    REQUIRE(prefabs_api->api != nullptr);
    
    // We register the dummy component in flecs
    ecs.component<DummyComponent>("DummyComponent");

    SECTION("Register and Check Factory") {
        prefabs_api->api->register_component_factory(ecs.c_ptr(), "DummyComponent", factory_Dummy);
        REQUIRE(prefabs_api->api->has_component_factory(ecs.c_ptr(), "DummyComponent") == true);
        REQUIRE(prefabs_api->api->has_component_factory(ecs.c_ptr(), "NonExistent") == false);
    }

    SECTION("Create Prefab") {
        sandbox_properties_handle_t empty = {0};
        ecs_entity_t square = prefabs_api->api->create_prefab(ecs.c_ptr(), "square", empty);
        REQUIRE(square != 0);
        REQUIRE(ecs.entity(square).is_valid());
    }
}
