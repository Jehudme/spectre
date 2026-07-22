#include "runtime_module.h"
#include <chrono>
#include <sandbox/sdk/filesystem.hpp>
#include <sandbox/sdk/logs.hpp>
#include <sandbox/sdk/properties.hpp>
#include <sandbox/sdk/runtime.hpp>
#include <sandbox/services/runtime_service.h>
#include <spectre/sdk/prefabs.hpp>
#include <spectre/sdk/renderer.hpp>
#include <spectre/sdk/scenes.hpp>
#include <spectre/sdk/scripts.hpp>
#include <spectre/sdk/window.hpp>

namespace spectre::modules {

runtime_module_t::~runtime_module_t() {
    m_running->store(false);
    resume();
    if (m_thread && m_thread->joinable()) {
        m_thread->join();
    }
}

void runtime_module_t::main_loop(flecs::world& entity_world) {
    sandbox::modules::logs::info(entity_world, "[Runtime Module] Runtime loop started");

    while (m_running->load() && entity_world.progress()) {
        if (m_paused->load()) {
            std::unique_lock<std::mutex> lock(*m_mutex);
            sandbox::modules::logs::info(entity_world, "[Runtime Module] Runtime paused");
            m_cv->wait(lock, [this]() { return !m_paused->load() || !m_running->load(); });
            sandbox::modules::logs::info(entity_world, "[Runtime Module] Runtime resumed");
        }
    }

    sandbox::modules::logs::info(entity_world, "[Runtime Module] Runtime loop stopped");
}

static void register_scripts_from_directory(flecs::world& world, const char* virtual_path) {
    if (!sandbox::modules::filesystem::exists(world, virtual_path))
        return;
    auto files = sandbox::modules::filesystem::list_files(world, virtual_path, true);
    for (const auto& file : files) {
        sandbox::modules::logs::trace(world, "[Runtime Module] Registering script: {}", file);
        spectre::modules::scripts::include_code(world, file);
    }
}

static void register_prefabs_from_directory(flecs::world& world, const char* virtual_path) {
    if (!sandbox::modules::filesystem::exists(world, virtual_path))
        return;
    auto files = sandbox::modules::filesystem::list_files(world, virtual_path, true);
    for (const auto& file : files) {
        sandbox::modules::logs::trace(world, "[Runtime Module] Registering prefab: {}", file);
        std::string content = sandbox::modules::filesystem::read_all_text(world, file.c_str());
        if (!content.empty()) {
            sandbox::properties props(content, sandbox::properties::Format::JSON);
            std::string prefab_name = props.get<std::string>("name").value_or(file);
            spectre::modules::prefabs::register_prefab(world, prefab_name.c_str(), props.get_raw());
        }
    }
}

static void register_scenes_from_directory(flecs::world& world, const char* virtual_path) {
    if (!sandbox::modules::filesystem::exists(world, virtual_path))
        return;
    auto files = sandbox::modules::filesystem::list_files(world, virtual_path, true);
    for (const auto& file : files) {
        sandbox::modules::logs::trace(world, "[Runtime Module] Registering scene: {}", file);
        std::string content = sandbox::modules::filesystem::read_all_text(world, file.c_str());
        if (!content.empty()) {
            sandbox::properties props(content, sandbox::properties::Format::JSON);
            spectre::modules::scenes::register_scene(world, props.get_raw());
        }
    }
}

static void register_states_from_directory(flecs::world& world, const char* virtual_path) {
    if (!sandbox::modules::filesystem::exists(world, virtual_path))
        return;
    auto files = sandbox::modules::filesystem::list_files(world, virtual_path, true);
    for (const auto& file : files) {
        sandbox::modules::logs::trace(world, "[Runtime Module] Registering state: {}", file);
        std::string content = sandbox::modules::filesystem::read_all_text(world, file.c_str());
        if (!content.empty()) {
            sandbox::properties props(content, sandbox::properties::Format::JSON);
            spectre::modules::scenes::register_state(world, props.get_raw());
        }
    }
}

bool runtime_module_t::initialize(flecs::world& entity_world) {
    sandbox::modules::logs::info(entity_world, "[Runtime Module] Initializing runtime resources...");

    register_scripts_from_directory(entity_world, "app://resources/assets/scripts");
    register_prefabs_from_directory(entity_world, "app://resources/scenes/prefabs");
    register_scenes_from_directory(entity_world, "app://resources/scenes/scenes");
    register_states_from_directory(entity_world, "app://resources/scenes/states");

    // Window Registration
    if (sandbox::modules::filesystem::exists(entity_world, "app://resources/configs/window.json")) {
        std::string content = sandbox::modules::filesystem::read_all_text(entity_world,
                                                                          "app://resources/configs/window.json");
        sandbox::properties props(content, sandbox::properties::Format::JSON);
        spectre::modules::window::register_window(entity_world, props.get_raw());
        sandbox::modules::logs::trace(entity_world, "[Runtime Module] Registered window config.");
    } else {
        sandbox::modules::logs::warn(entity_world, "[Runtime Module] Window configuration missing at "
                                                   "app://resources/configs/window.json");
    }

    // Renderer Registration
    if (sandbox::modules::filesystem::exists(entity_world, "app://resources/configs/renderer.json")) {
        std::string content = sandbox::modules::filesystem::read_all_text(entity_world,
                                                                          "app://resources/configs/renderer.json");
        sandbox::properties props(content, sandbox::properties::Format::JSON);
        spectre::modules::renderer::register_renderer(entity_world, props.get_raw());
        sandbox::modules::logs::trace(entity_world, "[Runtime Module] Registered renderer config.");
    } else {
        sandbox::modules::logs::warn(entity_world, "[Runtime Module] Renderer configuration missing at "
                                                   "app://resources/configs/renderer.json");
    }

    // Default State Stack
    if (sandbox::modules::filesystem::exists(entity_world, "app://resources/configs/startup.json")) {
        std::string content = sandbox::modules::filesystem::read_all_text(entity_world,
                                                                          "app://resources/configs/startup.json");
        sandbox::properties props(content, sandbox::properties::Format::JSON);
        std::vector<std::string> startup_states;
        if (props.get_array<std::string>("states", startup_states)) {
            for (const auto& state_name : startup_states) {
                ecs_entity_t state_id = spectre::modules::scenes::find_state(entity_world, state_name.c_str());
                if (state_id) {
                    spectre::modules::scenes::push_state(entity_world, state_id);
                }
            }
            sandbox::modules::logs::trace(entity_world, "[Runtime Module] Initialized state stack with {} states.",
                                          startup_states.size());
        }
    }

    sandbox::modules::logs::info(entity_world, "[Runtime Module] Runtime initialization complete.");
    return true;
}

void runtime_module_t::run(flecs::world& entity_world) {
    if (m_thread) {
        sandbox::modules::logs::warn(entity_world, "[Runtime Module] Runtime is already running in a thread.");
        return;
    }
    initialize(entity_world);
    m_running->store(true);
    main_loop(entity_world);
}

void runtime_module_t::start(flecs::world& entity_world) {
    if (m_thread) {
        sandbox::modules::logs::warn(entity_world, "[Runtime Module] Runtime is already running.");
        return;
    }

    initialize(entity_world);
    m_running->store(true);
    m_thread = std::make_unique<std::thread>(&runtime_module_t::main_loop, this, std::ref(entity_world));
}

void runtime_module_t::stop(flecs::world& entity_world) {
    m_running->store(false);
    entity_world.quit();
    resume(); // This notifies the condition variable in case it was paused
    if (m_thread && m_thread->joinable()) {
        m_thread->join();
        m_thread.reset();
    }
}

void runtime_module_t::pause() {
    m_paused->store(true);
}

void runtime_module_t::resume() {
    m_paused->store(false);
    m_cv->notify_all();
}

} // namespace spectre::modules

// ==========================================
static sandbox_requirement_info_t runtime_requirements[] = {{.kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
                                                             .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
                                                             .name = "logs",
                                                             .architecture = "sandbox",
                                                             .version_major = 1,
                                                             .version_minor = 0,
                                                             .version_patch = -1}};

namespace spectre::modules {
static void spectre_runtime_run(ecs_world_t* entity_world) {
    if (!entity_world)
        return;
    flecs::world flecs_world(entity_world);
    auto* runtime = flecs_world.try_get_mut<spectre::modules::runtime_module_t>();
    if (runtime)
        runtime->run(flecs_world);
}

static void spectre_runtime_start(ecs_world_t* entity_world) {
    if (!entity_world)
        return;
    flecs::world flecs_world(entity_world);
    auto* runtime = flecs_world.try_get_mut<spectre::modules::runtime_module_t>();
    if (runtime)
        runtime->start(flecs_world);
}

static void spectre_runtime_stop(ecs_world_t* entity_world) {
    if (!entity_world)
        return;
    flecs::world flecs_world(entity_world);
    auto* runtime = flecs_world.try_get_mut<spectre::modules::runtime_module_t>();
    if (runtime)
        runtime->stop(flecs_world);
}

static void spectre_runtime_pause(ecs_world_t* entity_world) {
    if (!entity_world)
        return;
    flecs::world flecs_world(entity_world);
    auto* runtime = flecs_world.try_get_mut<spectre::modules::runtime_module_t>();
    if (runtime)
        runtime->pause();
}

static void spectre_runtime_resume(ecs_world_t* entity_world) {
    if (!entity_world)
        return;
    flecs::world flecs_world(entity_world);
    auto* runtime = flecs_world.try_get_mut<spectre::modules::runtime_module_t>();
    if (runtime)
        runtime->resume();
}

static sandbox_runtime_api_t g_spectre_runtime_api = {.run = spectre_runtime_run,
                                                      .start = spectre_runtime_start,
                                                      .stop = spectre_runtime_stop,
                                                      .pause = spectre_runtime_pause,
                                                      .resume = spectre_runtime_resume};

struct runtime_module_initializer_t {
    runtime_module_initializer_t(flecs::world& entity_world) {
        entity_world.component<runtime_module_t>("RuntimeModule");
        entity_world.set<runtime_module_t>(runtime_module_t());

        // Override the default API with spectre's implementation
        sandbox_runtime_service_t* service = entity_world.try_get_mut<sandbox_runtime_service_t>();
        if (service) {
            service->api = &g_spectre_runtime_api;
        }
    }
};

SANDBOX_DECLARE_MODULE(runtime_module_initializer_t, {.name = "runtime",
                                                      .description = "Global runtime module",
                                                      .architecture = "spectre",
                                                      .version_major = 1,
                                                      .version_minor = 0,
                                                      .version_patch = 0,
                                                      .service = &sandbox_runtime_service_t_info,
                                                      .requirements = runtime_requirements,
                                                      .requirement_count = 1})
} // namespace spectre::modules
