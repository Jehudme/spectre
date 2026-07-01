#pragma once

#include <flecs.h>

namespace spectre::module {
    class RendererModule {
    public:
        RendererModule(flecs::world& world);
        ~RendererModule();

    private:
        flecs::world m_entity_world;
        flecs::system m_render_system;
    };
}
