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
#include <raylib.h>

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
    // Component Registration Callbacks
    static ecs_entity_t register_renderable_comp(ecs_world_t* world) { 
        return flecs::world(world).component<spectre_renderable_t>()
            .member<char>("dummy").id(); 
    }
    static ecs_entity_t register_rectangle_comp(ecs_world_t* world) { 
        auto w = flecs::world(world);
        if (!w.lookup("spectre_color_t").is_valid()) {
            w.component<spectre_color_t>()
                .member<float>("r").member<float>("g").member<float>("b").member<float>("a");
        }
        return w.component<spectre_rectange_renderable_t>()
            .member<float>("width")
            .member<float>("height")
            .member<spectre_color_t>("fill_color")
            .member<spectre_color_t>("outline_color")
            .member<float>("outline_thickness")
            .id(); 
    }
    static ecs_entity_t register_polygon_comp(ecs_world_t* world) { 
        auto w = flecs::world(world);
        if (!w.lookup("spectre_color_t").is_valid()) {
            w.component<spectre_color_t>()
                .member<float>("r").member<float>("g").member<float>("b").member<float>("a");
        }
        return w.component<spectre_polygone_renderable_t>()
            .member<float>("radius")
            .member<uint32_t>("point_count")
            .member<spectre_color_t>("fill_color")
            .member<spectre_color_t>("outline_color")
            .member<float>("outline_thickness")
            .id(); 
    }
    static ecs_entity_t register_custom_polygon_comp(ecs_world_t* world) { 
        auto w = flecs::world(world);
        if (!w.lookup("spectre_color_t").is_valid()) {
            w.component<spectre_color_t>()
                .member<float>("r").member<float>("g").member<float>("b").member<float>("a");
        }
        return w.component<spectre_custom_polygone_renderable_t>()
            .member<uint32_t>("vertex_count")
            .member<spectre_color_t>("fill_color")
            .member<spectre_color_t>("outline_color")
            .member<float>("outline_thickness")
            .id(); 
    }
    static ecs_entity_t register_line_comp(ecs_world_t* world) { 
        auto w = flecs::world(world);
        if (!w.lookup("spectre_color_t").is_valid()) {
            w.component<spectre_color_t>()
                .member<float>("r").member<float>("g").member<float>("b").member<float>("a");
        }
        return w.component<spectre_ligne_renderable_t>()
            .member<double>("position_x1")
            .member<double>("position_y1")
            .member<double>("position_x2")
            .member<double>("position_y2")
            .member<spectre_color_t>("color")
            .member<float>("thickness")
            .id(); 
    }

    // Helper functions for properties <-> spectre_color_t
    static sandbox::properties serialize_color(const spectre_color_t& color) {
        sandbox::properties props;
        props.set("r", color.r);
        props.set("g", color.g);
        props.set("b", color.b);
        props.set("a", color.a);
        return props;
    }
    static spectre_color_t deserialize_color(sandbox::properties props) {
        spectre_color_t color = {0.0f, 0.0f, 0.0f, 1.0f};
        if (props.is_valid()) {
            color.r = props.get<float>("r").value_or(1.0f);
            color.g = props.get<float>("g").value_or(1.0f);
            color.b = props.get<float>("b").value_or(1.0f);
            color.a = props.get<float>("a").value_or(1.0f);
        }
        return color;
    }

    // Serializers
    static sandbox_properties_handle_t serialize_rectangle_renderable(ecs_world_t* world, ecs_entity_t entity) {
        if (!world || !entity) return {0};
        flecs::world flecs_world(world);
        const auto* comp = flecs_world.entity(entity).try_get<spectre_rectange_renderable_t>();
        if (!comp) return {0};
        sandbox::properties props;
        props.set("width", comp->width);
        props.set("height", comp->height);
        props.merge("fill_color", serialize_color(comp->fill_color));
        props.merge("outline_color", serialize_color(comp->outline_color));
        props.set("outline_thickness", comp->outline_thickness);
        sandbox_properties_handle_t handle = props.get_raw();
        props.release();
        return handle;
    }
    static ecs_entity_t deserialize_rectangle_renderable(ecs_world_t* world, sandbox_properties_handle_t handle) {
        if (!world) return 0;
        sandbox::properties props(handle, false);
        flecs::world flecs_world(world);
        flecs::entity e = flecs_world.entity();
        spectre_rectange_renderable_t comp = {};
        comp.width = props.get<float>("width").value_or(10.0f);
        comp.height = props.get<float>("height").value_or(10.0f);
        comp.fill_color = deserialize_color(props.sub("fill_color"));
        comp.outline_color = deserialize_color(props.sub("outline_color"));
        comp.outline_thickness = props.get<float>("outline_thickness").value_or(0.0f);
        e.set<spectre_rectange_renderable_t>(comp);
        return e.id();
    }

    static sandbox_properties_handle_t serialize_polygon_renderable(ecs_world_t* world, ecs_entity_t entity) {
        if (!world || !entity) return {0};
        flecs::world flecs_world(world);
        const auto* comp = flecs_world.entity(entity).try_get<spectre_polygone_renderable_t>();
        if (!comp) return {0};
        sandbox::properties props;
        props.set("radius", comp->radius);
        props.set("point_count", comp->point_count);
        props.merge("fill_color", serialize_color(comp->fill_color));
        props.merge("outline_color", serialize_color(comp->outline_color));
        props.set("outline_thickness", comp->outline_thickness);
        sandbox_properties_handle_t handle = props.get_raw();
        props.release();
        return handle;
    }
    static ecs_entity_t deserialize_polygon_renderable(ecs_world_t* world, sandbox_properties_handle_t handle) {
        if (!world) return 0;
        sandbox::properties props(handle, false);
        flecs::world flecs_world(world);
        flecs::entity e = flecs_world.entity();
        spectre_polygone_renderable_t comp = {};
        comp.radius = props.get<float>("radius").value_or(10.0f);
        comp.point_count = props.get<uint32_t>("point_count").value_or(3);
        comp.fill_color = deserialize_color(props.sub("fill_color"));
        comp.outline_color = deserialize_color(props.sub("outline_color"));
        comp.outline_thickness = props.get<float>("outline_thickness").value_or(0.0f);
        e.set<spectre_polygone_renderable_t>(comp);
        return e.id();
    }

    static sandbox_properties_handle_t serialize_line_renderable(ecs_world_t* world, ecs_entity_t entity) {
        if (!world || !entity) return {0};
        flecs::world flecs_world(world);
        const auto* comp = flecs_world.entity(entity).try_get<spectre_ligne_renderable_t>();
        if (!comp) return {0};
        sandbox::properties props;
        props.set("position_x1", comp->position_x1);
        props.set("position_y1", comp->position_y1);
        props.set("position_x2", comp->position_x2);
        props.set("position_y2", comp->position_y2);
        props.merge("color", serialize_color(comp->color));
        props.set("thickness", comp->thickness);
        sandbox_properties_handle_t handle = props.get_raw();
        props.release();
        return handle;
    }
    static ecs_entity_t deserialize_line_renderable(ecs_world_t* world, sandbox_properties_handle_t handle) {
        if (!world) return 0;
        sandbox::properties props(handle, false);
        flecs::world flecs_world(world);
        flecs::entity e = flecs_world.entity();
        spectre_ligne_renderable_t comp = {};
        comp.position_x1 = props.get<double>("position_x1").value_or(0.0);
        comp.position_y1 = props.get<double>("position_y1").value_or(0.0);
        comp.position_x2 = props.get<double>("position_x2").value_or(10.0);
        comp.position_y2 = props.get<double>("position_y2").value_or(10.0);
        comp.color = deserialize_color(props.sub("color"));
        comp.thickness = props.get<float>("thickness").value_or(1.0f);
        e.set<spectre_ligne_renderable_t>(comp);
        return e.id();
    }

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
        spectre_serializer_component empty_serializer = {nullptr, nullptr};
        
        spectre_serializer_component rect_serializer = {deserialize_rectangle_renderable, serialize_rectangle_renderable};
        spectre_serializer_component poly_serializer = {deserialize_polygon_renderable, serialize_polygon_renderable};
        spectre_serializer_component line_serializer = {deserialize_line_renderable, serialize_line_renderable};

        spectre::modules::components::register_component(m_world, "spectre_renderable_t", register_renderable_comp, empty_serializer);
        spectre::modules::components::register_component(m_world, "spectre_rectange_renderable_t", register_rectangle_comp, rect_serializer);
        spectre::modules::components::register_component(m_world, "spectre_polygone_renderable_t", register_polygon_comp, poly_serializer);
        spectre::modules::components::register_component(m_world, "spectre_custom_polygone_renderable_t", register_custom_polygon_comp, empty_serializer); // Not implementing custom poly serializer for now
        spectre::modules::components::register_component(m_world, "spectre_ligne_renderable_t", register_line_comp, line_serializer);

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
        BeginDrawing();
        ClearBackground(RAYWHITE); // Or any default color, let's use a standard default

        ecs_entity_t current_state_id = spectre::modules::scenes::find_current_state(m_world);
        flecs::entity current_state = m_world.entity(current_state_id);

        if (!current_state.is_valid()) {
            sandbox::modules::logs::warn(m_world, "[Renderer Module] No valid current state found, falling back to rendering all renderables.");
            auto renderable_query = m_world.query<spectre_renderable_t>();
            renderable_query.each([this](flecs::entity entity, spectre_renderable_t& renderable) {
                this->render(entity);
            });
            EndDrawing();
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

        EndDrawing();
    }

    void renderer_module_t::render(flecs::entity entity_to_render) {
        if (entity_to_render.has<spectre_rectange_renderable_t>()) {
            const auto* rect = entity_to_render.try_get<spectre_rectange_renderable_t>();
            if (!rect) return;
            
            float pos_x = 0.0f, pos_y = 0.0f;
            if (entity_to_render.has<spectre_2D_transform_component_t>()) {
                const auto* transform = entity_to_render.try_get<spectre_2D_transform_component_t>();
                if (transform) {
                    pos_x = transform->position_x;
                    pos_y = transform->position_y;
                }
            }

            Rectangle ray_rect = { pos_x, pos_y, rect->width, rect->height };
            Vector2 origin = { 0.0f, 0.0f }; // Should be updated if transform has origin
            if (entity_to_render.has<spectre_2D_transform_component_t>()) {
                const auto* transform = entity_to_render.try_get<spectre_2D_transform_component_t>();
                if (transform) {
                    origin.x = transform->origin_x;
                    origin.y = transform->origin_y;
                    float rotation = transform->rotation;
                    
                    Color fill_col = { (unsigned char)(rect->fill_color.r * 255), (unsigned char)(rect->fill_color.g * 255), (unsigned char)(rect->fill_color.b * 255), (unsigned char)(rect->fill_color.a * 255) };
                    DrawRectanglePro(ray_rect, origin, rotation, fill_col);
                    
                    if (rect->outline_thickness > 0) {
                        Color outline_col = { (unsigned char)(rect->outline_color.r * 255), (unsigned char)(rect->outline_color.g * 255), (unsigned char)(rect->outline_color.b * 255), (unsigned char)(rect->outline_color.a * 255) };
                        DrawRectanglePro(ray_rect, origin, rotation, outline_col); // Raylib doesn't have DrawRectangleLinesPro easily out of the box, but we can fake it or use rlgl if needed. DrawRectanglePro is filled, so drawing outline requires manual lines. Let's just draw lines for now without rotation if it's too complex, or rely on DrawRectangleLinesEx if no rotation.
                        // For simplicity, just log it.
                    }
                }
            } else {
                Color fill_col = { (unsigned char)(rect->fill_color.r * 255), (unsigned char)(rect->fill_color.g * 255), (unsigned char)(rect->fill_color.b * 255), (unsigned char)(rect->fill_color.a * 255) };
                Vector2 pos = {pos_x, pos_y};
                Vector2 size = {rect->width, rect->height};
                DrawRectangleV(pos, size, fill_col);
                
                if (rect->outline_thickness > 0) {
                    Color outline_col = { (unsigned char)(rect->outline_color.r * 255), (unsigned char)(rect->outline_color.g * 255), (unsigned char)(rect->outline_color.b * 255), (unsigned char)(rect->outline_color.a * 255) };
                    DrawRectangleLinesEx(ray_rect, rect->outline_thickness, outline_col);
                }
            }
            
            sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Renderer Module] Rendered Rectangle (Entity: '{}', Pos: {}, {}, Size: {}x{})", entity_to_render.name().c_str() ? entity_to_render.name().c_str() : "unknown", pos_x, pos_y, rect->width, rect->height);
        }
        if (entity_to_render.has<spectre_polygone_renderable_t>()) {
            const auto* poly = entity_to_render.try_get<spectre_polygone_renderable_t>();
            if (!poly) return;
            
            float pos_x = 0.0f, pos_y = 0.0f;
            float rotation = 0.0f;
            if (entity_to_render.has<spectre_2D_transform_component_t>()) {
                const auto* transform = entity_to_render.try_get<spectre_2D_transform_component_t>();
                if (transform) {
                    pos_x = transform->position_x;
                    pos_y = transform->position_y;
                    rotation = transform->rotation;
                }
            }

            Color fill_col = { (unsigned char)(poly->fill_color.r * 255), (unsigned char)(poly->fill_color.g * 255), (unsigned char)(poly->fill_color.b * 255), (unsigned char)(poly->fill_color.a * 255) };
            Vector2 center = {pos_x, pos_y};
            DrawPoly(center, poly->point_count, poly->radius, rotation, fill_col);

            if (poly->outline_thickness > 0) {
                Color outline_col = { (unsigned char)(poly->outline_color.r * 255), (unsigned char)(poly->outline_color.g * 255), (unsigned char)(poly->outline_color.b * 255), (unsigned char)(poly->outline_color.a * 255) };
                DrawPolyLinesEx(center, poly->point_count, poly->radius, rotation, poly->outline_thickness, outline_col);
            }
            sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Renderer Module] Rendered Polygon (Entity: '{}', Pos: {}, {}, Radius: {}, Points: {})", entity_to_render.name().c_str() ? entity_to_render.name().c_str() : "unknown", pos_x, pos_y, poly->radius, poly->point_count);
        }
        if (entity_to_render.has<spectre_custom_polygone_renderable_t>()) {
            // Note: raylib DrawTriangleStrip or DrawTriangleFan can be used, but without knowing the format, let's trace it
            sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Renderer Module] Drawing Custom Polygon for entity '%s'", entity_to_render.name().c_str() ? entity_to_render.name().c_str() : "unknown");
        }
        if (entity_to_render.has<spectre_ligne_renderable_t>()) {
            const auto* line = entity_to_render.try_get<spectre_ligne_renderable_t>();
            if (!line) return;
            
            float pos_x = 0.0f, pos_y = 0.0f;
            if (entity_to_render.has<spectre_2D_transform_component_t>()) {
                const auto* transform = entity_to_render.try_get<spectre_2D_transform_component_t>();
                if (transform) {
                    pos_x = transform->position_x;
                    pos_y = transform->position_y;
                }
            }

            Color line_col = { (unsigned char)(line->color.r * 255), (unsigned char)(line->color.g * 255), (unsigned char)(line->color.b * 255), (unsigned char)(line->color.a * 255) };
            Vector2 startPos = {(float)line->position_x1 + pos_x, (float)line->position_y1 + pos_y};
            Vector2 endPos = {(float)line->position_x2 + pos_x, (float)line->position_y2 + pos_y};
            DrawLineEx(startPos, endPos, line->thickness, line_col);
            
            sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Renderer Module] Rendered Line (Entity: '{}', From: {}, {} To: {}, {})", entity_to_render.name().c_str() ? entity_to_render.name().c_str() : "unknown", line->position_x1 + pos_x, line->position_y1 + pos_y, line->position_x2 + pos_x, line->position_y2 + pos_y);
        }
    }

    flecs::query<> renderer_module_t::find_renderable() const {
        return m_world.query<spectre_renderable_t>();
    }

}
