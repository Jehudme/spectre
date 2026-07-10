#include <catch2/catch_all.hpp>
#include <flecs.h>
#include "modules/window/window_module.h"
#include "modules/serializer/serializer_module.h"
#include "spectre/spectre.h"
#include <string>

using namespace spectre::modules;

TEST_CASE("Window Module: Serialization and Configuration", "[window module test]") {
    flecs::world world;
    world.import<spectre::modules::serializer_module>();
    world.import<window_module_t>();
    
    auto* window_mod = &world.get_mut<window_module_t>();
    
    SECTION("Deserialize creates entity with correct properties") {
        sandbox::properties props;
        props.set("width", 1024u);
        props.set("height", 768u);
        props.set("title", std::string("Test Window"));
        props.set("vsync", true);
        
        flecs::entity window_ent = window_mod->deserialize_window(props);
        REQUIRE(window_ent.is_valid());
        REQUIRE(window_ent.has<spectre_window_component_t>());
        
        const auto* comp = window_ent.try_get<spectre_window_component_t>();
        REQUIRE(comp != nullptr);
        REQUIRE(comp->width == 1024);
        REQUIRE(comp->height == 768);
        REQUIRE(std::string(comp->title) == "Test Window");
        REQUIRE(comp->vsync == true);
    }
    
    SECTION("Serialize generates properties correctly") {
        sandbox::properties props;
        props.set("width", 1280u);
        props.set("height", 720u);
        
        flecs::entity window_ent = window_mod->deserialize_window(props);
        
        sandbox::properties serialized = window_mod->serialize_window(window_ent);
        REQUIRE(serialized.is_valid());
        
        REQUIRE(serialized.get<uint32_t>("width").value_or(0) == 1280);
        REQUIRE(serialized.get<uint32_t>("height").value_or(0) == 720);
        REQUIRE(serialized.get<std::string>("title").value_or("") == "Spectre Engine");
        REQUIRE(serialized.get<bool>("vsync").value_or(true) == false);
    }

    // Since register_window initializes Raylib and pops up a physical window, we want to make sure it functions properly.
    // However, headless CI environments might crash if xvfb is not available. 
    // We execute it here safely as a simple test case.
    SECTION("Register Window and modify parameters") {
        sandbox::properties props;
        props.set("width", 800u);
        props.set("height", 600u);
        props.set("title", std::string("Unit Test Window"));
        props.set("visible", false); // Keep hidden during tests if possible
        
        window_mod->register_window(props);
        
        REQUIRE(window_mod->get_width() == 800);
        REQUIRE(window_mod->get_height() == 600);
        REQUIRE(std::string(window_mod->get_title()) == "Unit Test Window");
        
        // Test Setters
        window_mod->set_size(1024, 768);
        REQUIRE(window_mod->get_width() == 1024);
        REQUIRE(window_mod->get_height() == 768);
    }
}
