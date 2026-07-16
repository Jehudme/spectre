#include "runtime_module.h"
#include <sandbox/sdk/runtime.hpp>
#include <sandbox/sdk/logs.hpp>
#include <sandbox/services/runtime_service.h>
#include <chrono>

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

    void runtime_module_t::run(flecs::world& entity_world) {
        if (m_thread) {
            sandbox::modules::logs::warn(entity_world, "[Runtime Module] Runtime is already running in a thread.");
            return;
        }
        m_running->store(true);
        main_loop(entity_world);
    }

    void runtime_module_t::start(flecs::world& entity_world) {
        if (m_thread) {
            sandbox::modules::logs::warn(entity_world, "[Runtime Module] Runtime is already running.");
            return;
        }
        
        m_running->store(true);
        m_thread = std::make_shared<std::thread>([this, entity_world]() mutable {
            main_loop(entity_world);
        });
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

}

// ==========================================
static sandbox_requirement_info_t runtime_requirements[] = {
    {
        .kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
        .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
        .name = "runtime",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0
    }
};

namespace spectre::modules {
    struct runtime_module_initializer_t {
        runtime_module_initializer_t(flecs::world& entity_world) {
            entity_world.component<runtime_module_t>();
            entity_world.set<runtime_module_t>(runtime_module_t());
        }
    };

    SANDBOX_DECLARE_MODULE(runtime_module_initializer_t, {
        .name = "runtime",
        .description = "Spectre Global runtime module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &sandbox_runtime_service_t_info,
        .requirements = runtime_requirements,
        .requirement_count = 1
    })
}
