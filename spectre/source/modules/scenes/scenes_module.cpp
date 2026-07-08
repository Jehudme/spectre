#include "scenes_module.h"
#include "spectre/services/scenes_service.h"
#include <iostream>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(scenes_module_t, {
        .name = "scenes",
        .description = "Scenes Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_scenes_service_t_info_decl,
        .requirements = nullptr,
        .requirement_count = 0
    })

    scenes_module_t::scenes_module_t(flecs::world& world) : m_world(world) {
        std::cout << "[SPECTRE] Initializing " << "scenes" << " module" << std::endl;
    }
    
    scenes_module_t::~scenes_module_t() = default;

    sandbox::properties scenes_module_t::serialize_state(flecs::entity state) {
        return sandbox::properties{};
    }

    flecs::entity scenes_module_t::deserialize_state(sandbox::properties props) {
        return flecs::entity::null();
    }

    sandbox::properties scenes_module_t::serialize_scene(flecs::entity scene) {
        return sandbox::properties{};
    }

    flecs::entity scenes_module_t::deserialize_scene(sandbox::properties props) {
        return flecs::entity::null();
    }

    void scenes_module_t::register_state(sandbox::properties props) {
    }

    void scenes_module_t::register_scene(sandbox::properties props) {
    }

    flecs::entity scenes_module_t::find_state(std::string_view name) {
        return flecs::entity::null();
    }

    flecs::entity scenes_module_t::find_scene(std::string_view name) {
        return flecs::entity::null();
    }

    bool scenes_module_t::has_state(std::string_view name) const {
        return false;
    }

    bool scenes_module_t::has_scene(std::string_view name) const {
        return false;
    }

    bool scenes_module_t::is_state(flecs::entity entity) const {
        return false;
    }

    bool scenes_module_t::is_scene(flecs::entity entity) const {
        return false;
    }

    flecs::entity scenes_module_t::find_current_state() {
        return flecs::entity::null();
    }

    flecs::query<> scenes_module_t::find_current_scenes() {
        return {};
    }

    void scenes_module_t::push_state(flecs::entity state) {
    }

    void scenes_module_t::pop_state() {
    }

}
