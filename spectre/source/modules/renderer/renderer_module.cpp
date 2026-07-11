#include "renderer_module.h"
#include "spectre/services/renderer_service.h"
#include "spectre/components/renderer_component.h"
#include "spectre/sdk/scenes.hpp"
#include "spectre/sdk/components.hpp"
#include "spectre/sdk/serializer.hpp"
#include "sandbox/sdk/logs.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

#include "spectre/sdk/renderer.hpp"

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

    static ecs_entity_t deserialize_renderer_cb(ecs_world_t* world, sandbox_properties_handle_t properties_handle) {
        return spectre::modules::renderer::deserialize_renderer(flecs::world(world), properties_handle);
    }

    static sandbox_properties_handle_t serialize_renderer_cb(ecs_world_t* world, ecs_entity_t entity_id) {
        return spectre::modules::renderer::serialize_renderer(flecs::world(world), entity_id);
    }
    
    // Component Registration Callbacks
    // TODO: register components member
    static ecs_entity_t register_renderable_comp(ecs_world_t* world) { return flecs::world(world).component<spectre_renderable_t>().id(); }
    static ecs_entity_t register_rectangle_comp(ecs_world_t* world) { return flecs::world(world).component<spectre_rectange_renderable_t>().id(); }
    static ecs_entity_t register_polygon_comp(ecs_world_t* world) { return flecs::world(world).component<spectre_polygone_renderable_t>().id(); }
    static ecs_entity_t register_custom_polygon_comp(ecs_world_t* world) { return flecs::world(world).component<spectre_custom_polygone_renderable_t>().id(); }
    static ecs_entity_t register_line_comp(ecs_world_t* world) { return flecs::world(world).component<spectre_ligne_renderable_t>().id(); }

    struct spectre_renderer_update_marker_t { char dummy; };

    renderer_module_t::renderer_module_t(flecs::world& world) : m_world(world) {
        sandbox::modules::logs::trace(m_world, "[Renderer Module] Initializing...");

        spectre_serializer_component serializer_callbacks = {};
        serializer_callbacks.deserialize = deserialize_renderer_cb;
        serializer_callbacks.serialize = serialize_renderer_cb;
        
        spectre::modules::serializer::register_serializer(m_world, "renderer", &serializer_callbacks);

        m_renderer = m_world.entity("::renderer")
            .add<spectre_renderer_update_marker_t>();

        // Register Renderer Components via SDK
        // TODO: create serializers for these components
        spectre_serializer_component empty_serializer = {nullptr, nullptr};
        spectre::modules::components::register_component(m_world, "spectre_renderable_t", register_renderable_comp, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_rectange_renderable_t", register_rectangle_comp, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_polygone_renderable_t", register_polygon_comp, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_custom_polygone_renderable_t", register_custom_polygon_comp, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_ligne_renderable_t", register_line_comp, empty_serializer);

        flecs::entity on_renderer_phase = m_world.entity("on_renderer").add(flecs::Phase).depends_on(flecs::OnUpdate);

        m_world.system<spectre_renderer_update_marker_t>("RendererSystem")
            .kind(on_renderer_phase)
            .each([this](flecs::entity entity, spectre_renderer_update_marker_t&) {
                this->render_frame();
            });
            
        sandbox::modules::logs::info(m_world, "[Renderer Module] Initialized successfully.");
    }
    
    renderer_module_t::~renderer_module_t() = default;

    flecs::entity renderer_module_t::deserialize_renderer(const sandbox::properties& properties) {
        sandbox::modules::logs::debug(const_cast<flecs::world&>(m_world), "[Renderer Module] Deserializing renderer entity.");
        return m_world.entity();
    }

    sandbox::properties renderer_module_t::serialize_renderer(flecs::entity renderer_entity) {
        sandbox::modules::logs::debug(const_cast<flecs::world&>(m_world), "[Renderer Module] Serializing renderer entity '{}'.", renderer_entity.name().c_str() ? renderer_entity.name().c_str() : "unknown");
        return sandbox::properties{};
    }

    void renderer_module_t::register_renderer(const sandbox::properties& properties) {
        m_renderer = deserialize_renderer(properties);
    }

    bool renderer_module_t::is_renderer() const {
        return m_renderer.is_valid();
    }

    void renderer_module_t::render_frame() {
        ecs_entity_t current_state_id = spectre::modules::scenes::find_current_state(m_world);
        flecs::entity current_state = m_world.entity(current_state_id);

        if (!current_state.is_valid()) {
            sandbox::modules::logs::warn(m_world, "[Renderer Module] No valid current state found, falling back to rendering all renderables.");
            auto renderable_query = m_world.query<spectre_renderable_t>();
            renderable_query.each([this](flecs::entity entity, spectre_renderable_t& renderable) {
                this->render(entity);
            });
            return;
        }

        struct renderable_entity_t {
            flecs::entity entity;
            int layer;
        };
        std::vector<renderable_entity_t> entities_to_render;
        
        auto renderable_query = m_world.query<spectre_renderable_t>();
        renderable_query.each([&](flecs::entity entity, spectre_renderable_t&) {
            flecs::entity parent_entity = entity.target(flecs::ChildOf);
            int layer_index = 0;
            bool found_scene_ancestor = false;
            
            while (parent_entity.is_valid() && parent_entity.id() != 0) {
                if (parent_entity.has<spectre_state_use_scene_relation_t>() && parent_entity.target(flecs::ChildOf) == current_state) {
                    layer_index = parent_entity.get<spectre_state_use_scene_relation_t>().layer_index;
                    found_scene_ancestor = true;
                    break;
                }
                parent_entity = parent_entity.target(flecs::ChildOf);
            }
            
            if (found_scene_ancestor) {
                entities_to_render.push_back({entity, layer_index});
            }
        });

        std::sort(entities_to_render.begin(), entities_to_render.end(), [](const renderable_entity_t& entity_a, const renderable_entity_t& entity_b) {
            return entity_a.layer < entity_b.layer;
        });

        for (const auto& renderable_entity : entities_to_render) {
            this->render(renderable_entity.entity);
        }
    }

    // TODO: Make the code to use raylib to render these
    void renderer_module_t::render(flecs::entity entity_to_render) {
        if (entity_to_render.has<spectre_rectange_renderable_t>()) {
            sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Renderer Module] Drawing Rectangle for entity '{}'", entity_to_render.name().c_str() ? entity_to_render.name().c_str() : "unknown");
        }
        if (entity_to_render.has<spectre_polygone_renderable_t>()) {
            sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Renderer Module] Drawing Polygon for entity '{}'", entity_to_render.name().c_str() ? entity_to_render.name().c_str() : "unknown");
        }
        if (entity_to_render.has<spectre_custom_polygone_renderable_t>()) {
            sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Renderer Module] Drawing Custom Polygon for entity '{}'", entity_to_render.name().c_str() ? entity_to_render.name().c_str() : "unknown");
        }
        if (entity_to_render.has<spectre_ligne_renderable_t>()) {
            sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Renderer Module] Drawing Line for entity '{}'", entity_to_render.name().c_str() ? entity_to_render.name().c_str() : "unknown");
        }
    }

    flecs::query<> renderer_module_t::find_renderable() const {
        return m_world.query<spectre_renderable_t>();
    }

}
