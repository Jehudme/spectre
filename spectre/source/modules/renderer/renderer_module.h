#pragma once
#include "flecs.h"
#include "sandbox/sdk/properties.hpp"

namespace spectre::modules {

    class renderer_module_t {
    public:
        explicit renderer_module_t(flecs::world& world);
        ~renderer_module_t();

        renderer_module_t(const renderer_module_t&) = delete;
        renderer_module_t& operator=(const renderer_module_t&) = delete;
        renderer_module_t(renderer_module_t&&) = delete;
        renderer_module_t& operator=(renderer_module_t&&) = delete;

        void deserialize_renderer(flecs::entity target, const sandbox::properties& props);
        sandbox::properties serialize_renderer(flecs::entity renderer);

        void register_renderer(const sandbox::properties& props);
        bool is_renderer() const;

    private:
        void render_frame();
        void render(flecs::entity entity);

        flecs::query<> find_renderable() const;

        flecs::world m_world;
        flecs::entity m_renderer;
        flecs::entity m_renderer_prefab;
        flecs::entity m_renderer_serializer;
    };

} // namespace spectre::module