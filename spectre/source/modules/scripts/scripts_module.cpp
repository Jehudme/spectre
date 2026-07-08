#include "scripts_module.h"
#include "spectre/abi/scripts_service.h"
#include <iostream>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(script_module_t, {
        .name = "scripts",
        .description = "Scripts Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_scripts_service_t_info_decl,
        .requirements = nullptr,
        .requirement_count = 0
    })

    script_module_t::script_module_t(flecs::world& world) : m_world(world) {
        std::cout << "[SPECTRE] Initializing " << "scripts" << " module" << std::endl;
    }
    
    script_module_t::~script_module_t() = default;

    sandbox::properties script_module_t::serialize_script(flecs::entity script) {
        return sandbox::properties{};
    }

    flecs::entity script_module_t::deserialize_script(sandbox::properties props) {
        return flecs::entity::null();
    }

    void script_module_t::register_script(sandbox::properties props) {
    }

    bool script_module_t::has_script(std::string_view name) const {
        return false;
    }

    bool script_module_t::is_script(flecs::entity entity) const {
        return false;
    }

    flecs::entity script_module_t::find_script(std::string_view name) {
        return flecs::entity::null();
    }

    void script_module_t::include_code(std::string_view path) {
    }

    void script_module_t::execute_script(flecs::entity script_entity, script_arguments_t args) {
    }

}
