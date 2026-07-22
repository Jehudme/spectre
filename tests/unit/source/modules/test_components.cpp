#include <catch2/catch_test_macros.hpp>
#include <flecs.h>
#include <spectre/sdk/components.hpp>
#include <spectre/services/components_service.h>
#include "modules/components/components_module.h"
#include "modules/serializer/serializer_module.h"
#include <sandbox/abi/bootstrapper.h>
#include <string>

using namespace spectre::modules;

struct dummy_component_t {
    float x, y;
};

static ecs_entity_t register_dummy(ecs_world_t* world) {
    flecs::world w(world);
    return w.component<dummy_component_t>("DummyComponent").id();
}

TEST_CASE("Components Module: SDK and Registration", "[components module]") {
    flecs::world world;
    
    // Import components module first so it initializes properly
    world.import<spectre::modules::components_module_t>();

    // Serializer module because components registers there
    world.import<spectre::modules::serializer_module>();
    auto* components_mod = world.try_get_mut<spectre::modules::components_module_t>();
    REQUIRE(components_mod != nullptr);

    SECTION("Register a component and verify") {
        spectre_serializer_component serializer = {};
        serializer.serialize = [](ecs_world_t*, ecs_entity_t) -> sandbox_properties_handle_t { return {0}; };
        serializer.deserialize = [](ecs_world_t*, ecs_entity_t, sandbox_properties_handle_t) {};

        components_mod->register_component("DummyComponent", register_dummy, serializer);

        REQUIRE(components_mod->has_component("DummyComponent") == true);
        REQUIRE(components_mod->has_component("NonExistent") == false);

        flecs::entity comp = components_mod->find_component("DummyComponent");
        REQUIRE(comp.is_valid() == true);

        REQUIRE(components_mod->is_component(comp) == true);
        REQUIRE(components_mod->is_component(world.entity()) == false);
        
        // Also verify the component has a serializer
        REQUIRE(comp.has<spectre_serializer_component>());
    }

    SECTION("Register component with empty name") {
        spectre_serializer_component serializer = {};
        components_mod->register_component("", register_dummy, serializer);
        
        // Should have logged an error and not registered
        REQUIRE(components_mod->has_component("") == false);
    }
}
