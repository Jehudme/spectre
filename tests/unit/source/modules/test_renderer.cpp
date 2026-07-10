#include <catch2/catch_test_macros.hpp>
#include <flecs.h>
#include "spectre/components/renderer_component.h"
#include "spectre/services/renderer_service.h"
#include "modules/renderer/renderer_module.h"
#include "modules/scenes/scenes_module.h"
#include "spectre/sdk/scenes.hpp"
#include "sandbox/sdk/logs.hpp"

#include <iostream>

sandbox_logs_api_t mock_logs_api = {
    [](ecs_world_t*, const char* msg) { std::cout << "[TRACE] " << msg << std::endl; },
    [](ecs_world_t*, const char* msg) { std::cout << "[DEBUG] " << msg << std::endl; },
    [](ecs_world_t*, const char* msg) { std::cout << "[INFO] "  << msg << std::endl; },
    [](ecs_world_t*, const char* msg) { std::cout << "[WARN] "  << msg << std::endl; },
    [](ecs_world_t*, const char* msg) { std::cout << "[ERROR] " << msg << std::endl; }
};

TEST_CASE("Renderer Module: Rendering and Logging", "[renderer]") {
    flecs::world world;
    
    sandbox_logs_service_t logs_svc = { &mock_logs_api };
    world.set<sandbox_logs_service_t>(logs_svc);
    
    // Import required modules
    world.import<spectre::modules::renderer_module_t>();

    SECTION("Entity with renderable component logs its rendering type") {
        sandbox::modules::logs::info(world, "Starting Renderer Test...");
        
        spectre_rectange_renderable_t rect_comp;
        rect_comp.width = 100.0f;
        rect_comp.height = 50.0f;
        rect_comp.fill_color = {1,0,0,1};
        rect_comp.outline_color = {0,1,0,1};
        rect_comp.outline_thickness = 2.0f;

        // Create an entity with a rectangle renderable
        flecs::entity rect_entity = world.entity("MyRectangle")
            .add<spectre_renderable_t>()
            .set<spectre_rectange_renderable_t>(rect_comp);
            
        spectre_polygone_renderable_t poly_comp;
        poly_comp.radius = 50.0f;
        poly_comp.point_count = 6;
        poly_comp.fill_color = {1,1,0,1};
        poly_comp.outline_color = {0,0,1,1};
        poly_comp.outline_thickness = 1.0f;

        // Create an entity with a polygon renderable
        flecs::entity poly_entity = world.entity("MyPolygon")
            .add<spectre_renderable_t>()
            .set<spectre_polygone_renderable_t>(poly_comp);
            
        spectre_ligne_renderable_t line_comp;
        line_comp.position_x1 = 0.0;
        line_comp.position_y1 = 0.0;
        line_comp.position_x2 = 10.0;
        line_comp.position_y2 = 10.0;
        line_comp.color = {0,0,0,1};
        line_comp.thickness = 2.0f;

        // Create an entity with a line renderable
        flecs::entity line_entity = world.entity("MyLine")
            .add<spectre_renderable_t>()
            .set<spectre_ligne_renderable_t>(line_comp);

        // Run ECS pipeline (triggers OnUpdate -> on_renderer)
        world.progress();
        
        sandbox::modules::logs::info(world, "Renderer Test Finished.");

        REQUIRE(rect_entity.is_valid());
        REQUIRE(poly_entity.is_valid());
        REQUIRE(line_entity.is_valid());
    }

    SECTION("Renderables only draw when their state is active") {
        world.import<spectre::modules::scenes_module_t>();
        
        spectre_scenes_api_t mock_scenes_api = {};
        mock_scenes_api.find_current_state = [](ecs_world_t* w) -> ecs_entity_t {
            flecs::world fw(w);
            auto* mod = fw.try_get_mut<spectre::modules::scenes_module_t>();
            return mod ? mod->find_current_state().id() : 0;
        };
        spectre_scenes_service_t scenes_svc = { &mock_scenes_api };
        world.set<spectre_scenes_service_t>(scenes_svc);

        auto* scenes_module = const_cast<spectre::modules::scenes_module_t*>(&world.get_mut<spectre::modules::scenes_module_t>());
        
        sandbox::modules::logs::info(world, "Starting State Push Test...");
        
        flecs::entity state1 = world.entity("State1").add(flecs::Prefab).add<spectre_state_t>();
        flecs::entity state2 = world.entity("State2").add(flecs::Prefab).add<spectre_state_t>();
        
        flecs::entity scene1 = world.entity("Scene1")
            .child_of(state1)
            .add<spectre_scene_t>()
            .set<spectre_state_use_scene_relation_t>({0});
            
        flecs::entity scene2 = world.entity("Scene2")
            .child_of(state2)
            .add<spectre_scene_t>()
            .set<spectre_state_use_scene_relation_t>({0});

        spectre_rectange_renderable_t r;
        r.width = 10.0f; r.height = 10.0f;
        r.fill_color = {1,1,1,1};
        r.outline_color = {1,1,1,1};
        r.outline_thickness = 1.0f;
        
        flecs::entity r1 = world.entity("RectInState1")
            .child_of(scene1)
            .add<spectre_renderable_t>()
            .set<spectre_rectange_renderable_t>(r);
            
        flecs::entity r2 = world.entity("RectInState2")
            .child_of(scene2)
            .add<spectre_renderable_t>()
            .set<spectre_rectange_renderable_t>(r);
            
        sandbox::modules::logs::info(world, "Pushing State 1...");
        scenes_module->push_state(state1);
        world.progress();
        
        sandbox::modules::logs::info(world, "Pushing State 2...");
        scenes_module->push_state(state2);
        world.progress();
        
        REQUIRE(r1.is_valid());
        REQUIRE(r2.is_valid());
    }
}
