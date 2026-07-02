#pragma once

#include <flecs.h>

namespace spectre::module {

    /**
     * @brief 2D batch renderer backed by raylib.
     *
     * On construction, registers ECS components, component factory functions with
     * the PrefabsModule, and starts the RenderSystem.
     *
     * The render system runs each frame and dispatches draw calls in z-order for
     * all entities that have both a Transform2D and a Renderable2D component.
     */
    class RendererModule {
    public:
        explicit RendererModule(flecs::world& world);
        ~RendererModule();

    private:
        /** Collects visible entities, sorts by z_order, and dispatches draw calls. */
        void render_frame();

        flecs::world  m_entity_world;
        flecs::system m_render_system;
    };

} // namespace spectre::module
