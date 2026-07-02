#include <catch2/catch_test_macros.hpp>
#include <flecs.h>
#include <sandbox/sdk/engine.hpp>
#include <spectre/abi/prefabs_service.h>
#include <sandbox/sdk/properties.hpp>
#include <sandbox/sdk/filesystem.hpp>
#include <sandbox/sdk/logs.hpp>

struct DummyComponent {
    int value;
};

static void factory_Dummy(ecs_world_t* ecs, ecs_entity_t e, sandbox_properties_handle_t props_h) {
    sandbox::properties props(props_h, false);
    DummyComponent comp;
    comp.value = (int)props.get<int64_t>("value").value_or(0);
    flecs::world(ecs).entity(e).set<DummyComponent>(comp);
}

TEST_CASE("Prefabs Module Initialization and Overrides", "[prefabs]") {
    sandbox::engine engine;
    sandbox::properties props;

    // We mount a temporary directory for testing
    props.set("filesystem/mounts/app/physical", "/tmp/spectre_tests/app");
    props.set("filesystem/mounts/app/readonly", false);

    std::vector<std::string> modules = {
        "sandbox-configuration@1.0.0", 
        "sandbox-logs@1.0.0", 
        "sandbox-filesystem@1.0.0",
        "spectre-prefabs@1.0.0"
    };
    props.set_array("engine/sandbox", modules);

    // Create the dummy prefabs.json
    std::system("mkdir -p /tmp/spectre_tests/app/state");
    std::system("cat << 'EOF' > /tmp/spectre_tests/app/state/prefabs.json\n"
                "{\n"
                "  \"prefabs\": {\n"
                "    \"parent_prefab\": {\n"
                "      \"components\": { \"DummyComponent\": { \"value\": 10 } },\n"
                "      \"auto_overrides\": [\"DummyComponent\"],\n"
                "      \"children\": {\n"
                "        \"child_prefab\": {\n"
                "          \"components\": { \"DummyComponent\": { \"value\": 20 } },\n"
                "          \"auto_overrides\": []\n"
                "        }\n"
                "      }\n"
                "    }\n"
                "  }\n"
                "}\n"
                "EOF");

    REQUIRE(engine.initialize(props) == true);
    flecs::world ecs(static_cast<ecs_world_t*>(engine.get_ecs()));
    
    const auto* prefabs_api = SANDBOX_GET_SERVICE(ecs, spectre_prefabs_service_t);
    REQUIRE(prefabs_api != nullptr);
    
    ecs.component<DummyComponent>("DummyComponent");
    prefabs_api->api->register_component_factory(ecs.c_ptr(), "DummyComponent", factory_Dummy);

    SECTION("Create Prefab Without Overrides") {
        sandbox_properties_handle_t empty = {0};
        ecs_entity_t parent = prefabs_api->api->create_prefab(ecs.c_ptr(), "parent_prefab", empty);
        REQUIRE(parent != 0);

        flecs::entity p = ecs.entity(parent);
        REQUIRE(p.get<DummyComponent>().value == 10);
        
        flecs::entity c = p.lookup("child_prefab");
        REQUIRE(c.is_valid());
        REQUIRE(c.get<DummyComponent>().value == 20);
    }

    SECTION("Create Prefab With Child Overrides") {
        std::string override_json = R"({
            "children": {
                "child_prefab": {
                    "components": {
                        "DummyComponent": {
                            "value": 99
                        }
                    }
                }
            }
        })";
        sandbox::properties overrides(override_json, sandbox::properties::Format::JSON);

        // create_prefab should apply the overrides
        ecs_entity_t parent = prefabs_api->api->create_prefab(ecs.c_ptr(), "parent_prefab", overrides.get_raw());
        REQUIRE(parent != 0);

        flecs::entity p = ecs.entity(parent);
        REQUIRE(p.get<DummyComponent>().value == 10); // Parent unchanged
        
        flecs::entity c = p.lookup("child_prefab");
        REQUIRE(c.is_valid());
        REQUIRE(c.get<DummyComponent>().value == 99); // Child overridden
    }
}
