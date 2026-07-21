#pragma once

#include "flecs.h"
#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

namespace spectre::modules {
struct runtime_module_t {
    runtime_module_t() = default;
    ~runtime_module_t();

    void run(flecs::world& entity_world);
    void start(flecs::world& entity_world);
    void stop(flecs::world& entity_world);
    void pause();
    void resume();

  private:
    void main_loop(flecs::world& entity_world);
    bool initialize(flecs::world& entity_world);

    std::shared_ptr<std::thread> m_thread;
    std::shared_ptr<std::mutex> m_mutex = std::make_shared<std::mutex>();
    std::shared_ptr<std::condition_variable> m_cv = std::make_shared<std::condition_variable>();
    std::shared_ptr<std::atomic<bool>> m_paused = std::make_shared<std::atomic<bool>>(false);
    std::shared_ptr<std::atomic<bool>> m_running = std::make_shared<std::atomic<bool>>(false);
};
} // namespace spectre::modules
