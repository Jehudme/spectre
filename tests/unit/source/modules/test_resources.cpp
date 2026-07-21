#include "flecs.h"
#include "modules/resources/resources_module.h"
#include "modules/serializer/serializer_module.h"
#include "sandbox/sdk/properties.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string>

static void dummy_load(ecs_world_t* world, spectre_resource_component_t* resource) {
    resource->instance = (void*)0x1234;
}

static void dummy_free(ecs_world_t* world, spectre_resource_component_t* resource) {
    resource->instance = nullptr;
}

TEST_CASE("Resources Module: Registration and Lifecycle", "[resources]") {
    flecs::world world;
    world.import <spectre::modules::serializer_module>();
    world.import <spectre::modules::resource_module_t>();
    auto* resources_mod = &world.get_mut<spectre::modules::resource_module_t>();
    REQUIRE(resources_mod != nullptr);

    SECTION("register and find loader") {
        spectre_resource_loader_component_t loader = {.load_fn = dummy_load, .free_fn = dummy_free};
        resources_mod->register_resource_loader("dummy", loader);

        REQUIRE(resources_mod->has_resource_loader("dummy") == true);
        REQUIRE(resources_mod->has_resource_loader("nonexistent") == false);

        flecs::entity loader_ent = resources_mod->find_resource_loader("dummy");
        REQUIRE(loader_ent.is_valid() == true);
        const auto* l = &loader_ent.get<spectre_resource_loader_component_t>();
        REQUIRE(l != nullptr);
        REQUIRE(l->load_fn == dummy_load);
    }

    SECTION("register and find resource") {
        spectre_resource_loader_component_t loader = {.load_fn = dummy_load, .free_fn = dummy_free};
        resources_mod->register_resource_loader("dummy", loader);

        sandbox::properties props;
        sandbox::properties res_node;
        res_node.set<std::string>("type", "dummy");
        res_node.set<std::string>("path", "/virtual/path/to/dummy");
        props.merge("my_resource", res_node);

        resources_mod->register_resource(props);

        REQUIRE(resources_mod->has_resource("my_resource") == true);
        flecs::entity res_ent = resources_mod->find_resource("my_resource");
        REQUIRE(res_ent.is_valid() == true);

        const auto* res_comp = &res_ent.get<spectre_resource_component_t>();
        REQUIRE(res_comp != nullptr);
        REQUIRE(std::string(res_comp->path) == "/virtual/path/to/dummy");
        REQUIRE(res_comp->instance == nullptr);

        REQUIRE(resources_mod->is_resource_loaded(res_ent) == false);

        void* instance = resources_mod->get_resource(res_ent);
        REQUIRE(instance == (void*)0x1234);
        REQUIRE(resources_mod->is_resource_loaded(res_ent) == true);

        resources_mod->free_resource(res_ent);
        REQUIRE(resources_mod->is_resource_loaded(res_ent) == false);
        REQUIRE(res_ent.get<spectre_resource_component_t>().instance == nullptr);
    }

    SECTION("deserialize resource directly") {
        spectre_resource_loader_component_t loader = {.load_fn = dummy_load, .free_fn = dummy_free};
        resources_mod->register_resource_loader("dummy", loader);

        sandbox::properties res_node;
        res_node.set<std::string>("type", "dummy");
        res_node.set<std::string>("path", "/path/to/second");

        flecs::entity res_ent = world.entity();
        resources_mod->deserialize_resource(res_ent, res_node);
        REQUIRE(res_ent.is_valid() == true);

        const auto* res_comp = &res_ent.get<spectre_resource_component_t>();
        REQUIRE(res_comp != nullptr);
        REQUIRE(std::string(res_comp->path) == "/path/to/second");

        void* instance = resources_mod->get_resource(res_ent);
        REQUIRE(instance == (void*)0x1234);
        REQUIRE(resources_mod->is_resource_loaded(res_ent) == true);
    }
}
