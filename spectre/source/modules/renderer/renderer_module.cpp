#include "renderer_module.h"
#include "spectre/services/renderer_service.h"
#include <iostream>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(renderer_module_t, {
        .name = "renderer",
        .description = "Renderer Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_renderer_service_t_info_decl,
        .requirements = nullptr,
        .requirement_count = 0
    })

    renderer_module_t::renderer_module_t(flecs::world& world) : m_world(world) {
        std::cout << "[SPECTRE] Initializing " << "renderer" << " module" << std::endl;
    }
    
    renderer_module_t::~renderer_module_t() = default;

    flecs::entity renderer_module_t::deserialize_renderer(sandbox::properties props) {
        return flecs::entity::null();
    }

    sandbox::properties renderer_module_t::serialize_renderer(flecs::entity renderer) {
        return sandbox::properties{};
    }

    void renderer_module_t::register_renderer(sandbox::properties props) {
    }

    bool renderer_module_t::is_renderer() const {
        return false;
    }

    void renderer_module_t::render(flecs::entity entity) {
    }

    flecs::query<> renderer_module_t::find_renderable() const {
        return {};
    }

}
