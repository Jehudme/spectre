#include "renderer_module.h"
#include "spectre/services/renderer_service.h"
#include "spectre/components/renderer_component.h"
#include "spectre/services/scenes_service.h"
#include "spectre/components/scenes_component.h"
#include "sandbox/sdk/logs.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(renderer_module_t, {
        .name = "renderer",
        .description = "Renderer Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_renderer_service_t_info,
        .requirements = nullptr,
        .requirement_count = 0
    })

    static ecs_entity_t deserialize_renderer_cb(ecs_world_t* world, sandbox_properties_handle_t props_handle) {
        flecs::world w(world);
        auto* mod = const_cast<renderer_module_t*>(w.try_get_mut<renderer_module_t>());
        if (mod) {
            sandbox::properties props(props_handle, false);
            return mod->deserialize_renderer(props).id();
        }
        return 0;
    }

    static sandbox_properties_handle_t serialize_renderer_cb(ecs_world_t* world, ecs_entity_t entity) {
        flecs::world w(world);
        auto* mod = const_cast<renderer_module_t*>(w.try_get_mut<renderer_module_t>());
        if (mod) {
            sandbox::properties props = mod->serialize_renderer(flecs::entity(w, entity));
            sandbox_properties_handle_t handle = props.get_raw();
            props.release();
            return handle;
        }
        return {0};
    }

    struct spectre_renderer_update_marker_t { char dummy; };

    renderer_module_t::renderer_module_t(flecs::world& world) : m_world(world) {
        spectre_serializer_component ser = {};
        ser.deserialize = deserialize_renderer_cb;
        ser.serialize = serialize_renderer_cb;
        m_renderer_serializer = m_world.entity("::spectre::serializers::renderer")
            .set<spectre_serializer_component>(ser);

        m_renderer = m_world.entity("::spectre::modules::renderer")
            .add<spectre_renderer_update_marker_t>();

        flecs::entity on_renderer = m_world.entity("on_renderer").add(flecs::Phase).depends_on(flecs::OnUpdate);

        m_world.system<spectre_renderer_update_marker_t>("RendererSystem")
            .kind(on_renderer)
            .each([this](flecs::entity e, spectre_renderer_update_marker_t&) {
                this->render_frame();
            });
    }
    
    renderer_module_t::~renderer_module_t() = default;

    flecs::entity renderer_module_t::deserialize_renderer(const sandbox::properties& props) {
        return m_world.entity();
    }

    sandbox::properties renderer_module_t::serialize_renderer(flecs::entity renderer) {
        return sandbox::properties{};
    }

    void renderer_module_t::register_renderer(const sandbox::properties& props) {
        m_renderer = deserialize_renderer(props);
    }

    bool renderer_module_t::is_renderer() const {
        return m_renderer.is_valid();
    }

    void renderer_module_t::render_frame() {
        flecs::entity current_state;
        auto* scenes_service = SANDBOX_GET_SERVICE(m_world, spectre_scenes_service_t);
        if (scenes_service && scenes_service->api && scenes_service->api->find_current_state) {
            ecs_entity_t state_id = scenes_service->api->find_current_state(m_world.c_ptr());
            if (state_id != 0) {
                current_state = m_world.entity(state_id);
            }
        }

        if (!current_state.is_valid()) {
            sandbox::modules::logs::debug(m_world, "No valid current state, rendering all.");
            auto q = m_world.query<spectre_renderable_t>();
            q.each([this](flecs::entity e, spectre_renderable_t& r) {
                this->render(e);
            });
            return;
        }

        sandbox::modules::logs::debug(m_world, "Current state is '{}'", current_state.name().c_str() ? current_state.name().c_str() : "unknown");

        struct RenderableEntity {
            flecs::entity entity;
            int layer;
        };
        std::vector<RenderableEntity> to_render;
        
        auto q = m_world.query<spectre_renderable_t>();
        q.each([&](flecs::entity e, spectre_renderable_t&) {
            flecs::entity parent = e.target(flecs::ChildOf);
            int layer = 0;
            bool found_scene = false;
            while (parent.is_valid() && parent.id() != 0) {
                if (parent.has<spectre_state_use_scene_relation_t>() && parent.target(flecs::ChildOf) == current_state) {
                    layer = parent.get<spectre_state_use_scene_relation_t>().layer_index;
                    found_scene = true;
                    break;
                }
                parent = parent.target(flecs::ChildOf);
            }
            
            if (found_scene) {
                to_render.push_back({e, layer});
            }
        });

        std::sort(to_render.begin(), to_render.end(), [](const RenderableEntity& a, const RenderableEntity& b) {
            return a.layer < b.layer;
        });

        for (const auto& re : to_render) {
            this->render(re.entity);
        }
    }

    void renderer_module_t::render(flecs::entity entity) {
        sandbox::modules::logs::debug(m_world, "Rendering entity '{}'", entity.name().c_str() ? entity.name().c_str() : "unknown");

        if (entity.has<spectre_rectange_renderable_t>()) {
            sandbox::modules::logs::debug(m_world, "  - Drawing Rectangle");
        }
        if (entity.has<spectre_polygone_renderable_t>()) {
            sandbox::modules::logs::debug(m_world, "  - Drawing Polygon");
        }
        if (entity.has<spectre_custom_polygone_renderable_t>()) {
            sandbox::modules::logs::debug(m_world, "  - Drawing Custom Polygon");
        }
        if (entity.has<spectre_ligne_renderable_t>()) {
            sandbox::modules::logs::debug(m_world, "  - Drawing Line");
        }
    }

    flecs::query<> renderer_module_t::find_renderable() const {
        return m_world.query<spectre_renderable_t>();
    }

}
