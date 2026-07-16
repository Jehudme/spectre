#include <flecs.h>
#include <sandbox/sdk/engine.hpp>
#include <sandbox/sdk/runtime.hpp>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <spectre/components/renderer_component.h>
#include <sandbox/sdk/logs.hpp>

#ifndef APP_RESOURCES_DIR
#error "APP_RESOURCES_DIR must be defined"
#endif

int main(int argc, char* argv[]) {
    try {
        sandbox::engine engine;
        sandbox::properties props;

        // Simulate launcher loading the app configuration
        std::string config_path = std::string(APP_RESOURCES_DIR) + "/configuration.json";
        if (FILE* f = fopen(config_path.c_str(), "rb")) {
            fseek(f, 0, SEEK_END);
            size_t size = ftell(f);
            fseek(f, 0, SEEK_SET);
            std::string content(size, '\0');
            fread(&content[0], 1, size, f);
            fclose(f);
            
            sandbox::properties app_props(content, sandbox::properties::Format::JSON);
            props.merge("", app_props);
        }

        // Mount the application resources folder as app://
        props.set("filesystem/mounts/app/physical", APP_RESOURCES_DIR);
        props.set("filesystem/mounts/app/readonly", true);

        // Ensure mandatory modules are present
        std::vector<std::string> modules;
        props.get_array<std::string>("engine/sandbox", modules);
        std::vector<std::string> mandatory = {
            "sandbox-application@1.0.0"
        };
        for (const auto& m : mandatory) {
            if (std::find(modules.begin(), modules.end(), m) == modules.end()) {
                modules.push_back(m);
            }
        }
        props.set_array("engine/sandbox", modules);

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
