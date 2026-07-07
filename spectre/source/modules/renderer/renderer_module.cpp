//
// Created by jehud on 2026-07-04.
//

#include "renderer_module.h"

namespace spectre::modules {
    renderer_module_t::renderer_module_t(flecs::world &world) {
    }

    renderer_module_t::~renderer_module_t() {
    }

    flecs::entity renderer_module_t::deserialize_renderer(sandbox::properties props) {
    }

    sandbox::properties renderer_module_t::serialize_renderer(flecs::entity renderer) {
    }

    void renderer_module_t::register_renderer(sandbox::properties props) {
    }

    bool renderer_module_t::is_renderer() const {
    }

    void renderer_module_t::render_frame() {
    }

    void renderer_module_t::render(flecs::entity entity) {
    }

    flecs::query<> renderer_module_t::find_renderable() const {
    }
}
