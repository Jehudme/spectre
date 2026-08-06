#include <algorithm>
#include <flecs.h>
#include <fstream>
#include <iostream>
#include <sandbox/sdk/engine.hpp>
#include <sandbox/sdk/logs.hpp>
#include <sandbox/sdk/runtime.hpp>
#include <spectre/components/renderer_component.h>

#ifndef APP_RESOURCES_DIR
#    error "APP_RESOURCES_DIR must be defined"
#endif

int main(int argc, char* argv[]) {
    try {
        sandbox::engine engine;
        sandbox::properties props;

        // Mount the application resources folder as app://
        props.set("booting-configuration/mount-path", APP_RESOURCES_DIR);

        // Ensure mandatory modules are present
        std::vector<std::string> modules;
        props.get_array<std::string>("engine/sandbox", modules);
        std::vector<std::string> mandatory = {"sandbox-application@1.0.0"};
        for (const auto& m : mandatory) {
            if (std::find(modules.begin(), modules.end(), m) == modules.end()) {
                modules.push_back(m);
            }
        }

        if (engine.initialize(props)) {
            flecs::world ecs(static_cast<ecs_world_t*>(engine.get_ecs()));

            sandbox::modules::runtime::run(ecs);
            return 0;
        }

        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[Test App] Fatal error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "[Test App] Fatal error: Unknown exception.\n";
        return 1;
    }
}
