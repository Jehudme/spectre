#include <catch2/catch_test_macros.hpp>
#include <flecs.h>
#include <sandbox/sdk/engine.hpp>
#include <spectre/abi/resources_service.h>
#include <sandbox/sdk/filesystem.hpp>
#include "../../../spectre/source/modules/resources/resource_module.h"

// We need a mock load and free function.
struct MockResourceInstance {
    std::string data;
};

static void* mock_load(spectre_resource_content_t* content) {
    if (!content || !content->data) return nullptr;
    auto* inst = new MockResourceInstance();
    inst->data = std::string((char*)content->data, content->size);
    return inst;
}

static void mock_free(void* instance) {
    if (instance) {
        delete static_cast<MockResourceInstance*>(instance);
    }
}

TEST_CASE("ResourceModule Initialization and Core Functions", "[resources]") {
    sandbox::engine engine;
    sandbox::properties props;

    // Define the APP_RESOURCES_DIR in CMake or we can use the relative path directly
    props.set("filesystem/mounts/app/physical", APP_RESOURCES_DIR);
    props.set("filesystem/mounts/app/readonly", true);

    std::vector<std::string> modules = {
        "sandbox-configuration@1.0.0", 
        "sandbox-logs@1.0.0", 
        "sandbox-filesystem@1.0.0"
    };
    props.set_array("engine/sandbox", modules);

    REQUIRE(engine.initialize(props) == true);
    flecs::world ecs(static_cast<ecs_world_t*>(engine.get_ecs()));

    // Create the resource module directly for testing
    spectre::module::ResourceModule res_module(ecs);

    SECTION("Create Resource and Instance") {
        res_module.create_resource("mock_type", mock_load, mock_free);
        
        // This should not throw
        REQUIRE_NOTHROW(res_module.create_resource_instance("mock_type", "app://resources/mock.txt"));

        // Get handle
        uint64_t handle = res_module.handle_for_path("app://resources/mock.txt");
        REQUIRE(handle != 0);

        SECTION("Load and Free") {
            void* raw_inst = res_module.load(handle);
            REQUIRE(raw_inst != nullptr);

            auto* inst = static_cast<MockResourceInstance*>(raw_inst);
            REQUIRE(inst->data == "Hello, this is a mock resource!\n");

            // Test caching
            void* raw_inst2 = res_module.load(handle);
            REQUIRE(raw_inst == raw_inst2);

            res_module.free_instance(handle);
            // It should still be alive because ref count was 2
            res_module.free_instance(handle);
            // Now it's freed
        }
    }
}
