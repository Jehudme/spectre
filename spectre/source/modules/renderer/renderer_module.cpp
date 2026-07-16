#include "renderer_module.h"
#include "spectre/services/renderer_service.h"
#include "spectre/components/renderer_component.h"
#include "spectre/sdk/scenes.hpp"

#include "spectre/sdk/serializer.hpp"
#include "sandbox/sdk/logs.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

#include "spectre/sdk/renderer.hpp"
#include <raylib.h>
#include <rlgl.h>

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

    static void deserialize_renderer_cb(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t properties_handle) {
        spectre::modules::renderer::deserialize_renderer(flecs::world(world), entity, properties_handle);
    }

    static sandbox_properties_handle_t serialize_renderer_cb(ecs_world_t* world, ecs_entity_t entity_id) {
        return spectre::modules::renderer::serialize_renderer(flecs::world(world), entity_id);
    }

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
        spectre_color_t color = {255.0f, 255.0f, 255.0f, 255.0f};
        if (props.is_valid()) {
            auto get_val = [&](const std::string& key) -> float {
                if (auto f = props.get<float>(key)) return *f;
                if (auto d = props.get<double>(key)) return static_cast<float>(*d);
                if (auto i = props.get<int>(key)) return static_cast<float>(*i);
                return 255.0f;
            };
            color.r = get_val("r");
            color.g = get_val("g");
            color.b = get_val("b");
            color.a = get_val("a");
        }
        std::cout << "deserialize_color parsed: r=" << color.r << ", g=" << color.g << ", b=" << color.b << ", a=" << color.a << std::endl;
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
    static void deserialize_rectangle_renderable(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t handle) {
        if (!world) return;
        sandbox::properties props(handle, false);
        flecs::world flecs_world(world);
        flecs::entity e(flecs_world, entity);
        spectre_rectange_renderable_t comp = {};
        comp.width = props.get<float>("width").value_or(10.0f);
        comp.height = props.get<float>("height").value_or(10.0f);
        comp.fill_color = deserialize_color(props.sub("fill_color"));
        comp.outline_color = deserialize_color(props.sub("outline_color"));
        comp.outline_thickness = props.get<float>("outline_thickness").value_or(0.0f);
        e.set<spectre_rectange_renderable_t>(comp);
    }


    static ecs_entity_t register_circle_comp(ecs_world_t* world) { 
        auto w = flecs::world(world);
        if (!w.lookup("spectre_color_t").is_valid()) {
            w.component<spectre_color_t>()
                .member<float>("r").member<float>("g").member<float>("b").member<float>("a");
        }
        return w.component<spectre_circle_renderable_t>()
            .member<float>("radius")
            .member<spectre_color_t>("fill_color")
            .member<spectre_color_t>("outline_color")
            .member<float>("outline_thickness")
            .id(); 
    }

    static sandbox_properties_handle_t serialize_circle_renderable(ecs_world_t* world, ecs_entity_t entity) {
        if (!world || !entity) return {0};
        flecs::world flecs_world(world);
        const auto* comp = flecs_world.entity(entity).try_get<spectre_circle_renderable_t>();
        if (!comp) return {0};
        sandbox::properties props;
        props.set("radius", comp->radius);
        props.merge("fill_color", serialize_color(comp->fill_color));
        props.merge("outline_color", serialize_color(comp->outline_color));
        props.set("outline_thickness", comp->outline_thickness);
        sandbox_properties_handle_t handle = props.get_raw();
        props.release();
        return handle;
    }

    static void deserialize_circle_renderable(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t handle) {
        if (!world) return;
        sandbox::properties props(handle, false);
        flecs::world flecs_world(world);
        flecs::entity e(flecs_world, entity);
        spectre_circle_renderable_t comp = {};
        comp.radius = props.get<float>("radius").value_or(10.0f);
        comp.fill_color = deserialize_color(props.sub("fill_color"));
        comp.outline_color = deserialize_color(props.sub("outline_color"));
        comp.outline_thickness = props.get<float>("outline_thickness").value_or(0.0f);
        e.set<spectre_circle_renderable_t>(comp);
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
    static void deserialize_polygon_renderable(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t handle) {
        if (!world) return;
        sandbox::properties props(handle, false);
        flecs::world flecs_world(world);
        flecs::entity e(flecs_world, entity);
        spectre_polygone_renderable_t comp = {};
        comp.radius = props.get<float>("radius").value_or(10.0f);
        comp.point_count = props.get<uint32_t>("point_count").value_or(3);
        comp.fill_color = deserialize_color(props.sub("fill_color"));
        comp.outline_color = deserialize_color(props.sub("outline_color"));
        comp.outline_thickness = props.get<float>("outline_thickness").value_or(0.0f);
        e.set<spectre_polygone_renderable_t>(comp);
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
    static void deserialize_line_renderable(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t handle) {
        if (!world) return;
        sandbox::properties props(handle, false);
        flecs::world flecs_world(world);
        flecs::entity e(flecs_world, entity);
        spectre_ligne_renderable_t comp = {};
        comp.position_x1 = props.get<double>("position_x1").value_or(0.0);
        comp.position_y1 = props.get<double>("position_y1").value_or(0.0);
        comp.position_x2 = props.get<double>("position_x2").value_or(10.0);
        comp.position_y2 = props.get<double>("position_y2").value_or(10.0);
        comp.color = deserialize_color(props.sub("color"));
        comp.thickness = props.get<float>("thickness").value_or(1.0f);
        e.set<spectre_ligne_renderable_t>(comp);
    }

    struct spectre_renderer_update_marker_t { char dummy; };

    static sandbox_properties_handle_t serialize_empty(ecs_world_t*, ecs_entity_t) { return {0}; }
    static void deserialize_empty(ecs_world_t*, ecs_entity_t, sandbox_properties_handle_t) {}
    
    static sandbox_properties_handle_t serialize_renderable(ecs_world_t* world, ecs_entity_t entity) {
        return {0}; 
    }
    static void deserialize_renderable(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t) {
        if (world && entity) {
            flecs::world flecs_world(world);
            flecs_world.entity(entity).add<spectre_renderable_t>();
        }
    }
    
    static sandbox_properties_handle_t serialize_2D_transform_component(ecs_world_t* world, ecs_entity_t entity) {
        if (!world || !entity) return {0};
        flecs::world flecs_world(world);
        const auto* comp = flecs_world.entity(entity).try_get<spectre_2D_transform_component_t>();
        if (!comp) return {0};
        sandbox::properties props;
        props.set("position_x", comp->position_x);
        props.set("position_y", comp->position_y);
        props.set("position_z", comp->position_z);
        props.set("scale_x", comp->scale_x);
        props.set("scale_y", comp->scale_y);
        props.set("rotation", comp->rotation);
        sandbox_properties_handle_t handle = props.get_raw();
        props.release();
        return handle;
    }
    static void deserialize_2D_transform_component(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t handle) {
        if (!world) return;
        sandbox::properties props(handle, false);
        flecs::world flecs_world(world);
        flecs::entity e(flecs_world, entity);
        spectre_2D_transform_component_t comp = {};
        comp.position_x = props.get<float>("position_x").value_or(0.0f);
        comp.position_y = props.get<float>("position_y").value_or(0.0f);
        comp.position_z = props.get<float>("position_z").value_or(0.0f);
        comp.scale_x = props.get<float>("scale_x").value_or(1.0f);
        comp.scale_y = props.get<float>("scale_y").value_or(1.0f);
        comp.rotation = props.get<float>("rotation").value_or(0.0f);
        e.set<spectre_2D_transform_component_t>(comp);
    }

    renderer_module_t::renderer_module_t(flecs::world& world) : m_world(world) {
        sandbox::modules::logs::trace(m_world, "[Renderer Module] Initializing...");

        spectre_serializer_component serializer_callbacks = {};
        serializer_callbacks.deserialize = deserialize_renderer_cb;
        serializer_callbacks.serialize = serialize_renderer_cb;
        
        spectre::modules::serializer::register_serializer(m_world, "renderer", &serializer_callbacks);


        // Register Renderer Components via SDK
        spectre_serializer_component empty_serializer = {deserialize_empty, serialize_empty};
        spectre_serializer_component renderable_serializer = {deserialize_renderable, serialize_renderable};
        
        spectre_serializer_component rect_serializer = {deserialize_rectangle_renderable, serialize_rectangle_renderable};
        spectre_serializer_component poly_serializer = {deserialize_polygon_renderable, serialize_polygon_renderable};
        spectre_serializer_component line_serializer = {deserialize_line_renderable, serialize_line_renderable};
        spectre_serializer_component transform_serializer = {deserialize_2D_transform_component, serialize_2D_transform_component};

        register_renderable_comp(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_renderable_t", &renderable_serializer);
        
        register_rectangle_comp(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_rectange_renderable_t", &rect_serializer);
        

        register_circle_comp(m_world.c_ptr());
        spectre_serializer_component circle_serializer = {deserialize_circle_renderable, serialize_circle_renderable};
        spectre::modules::serializer::register_serializer(m_world, "spectre_circle_renderable_t", &circle_serializer);

        register_polygon_comp(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_polygone_renderable_t", &poly_serializer);
        
        register_custom_polygon_comp(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_custom_polygone_renderable_t", &empty_serializer);
        
        register_line_comp(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_ligne_renderable_t", &line_serializer);
        
        m_world.component<spectre_2D_transform_component_t>()
            .member<float>("position_x")
            .member<float>("position_y")
            .member<float>("position_z")
            .member<float>("scale_x")
            .member<float>("scale_y")
            .member<float>("rotation");
        spectre::modules::serializer::register_serializer(m_world, "spectre_2D_transform_component_t", &transform_serializer);

        flecs::entity on_renderer_phase = m_world.entity("on_renderer").add(flecs::Phase).depends_on(flecs::OnUpdate);


            
        sandbox::modules::logs::info(m_world, "[Renderer Module] Initialized successfully.");
    }
    
    renderer_module_t::~renderer_module_t() = default;

    void renderer_module_t::deserialize_renderer(flecs::entity target, const sandbox::properties& properties) {
        sandbox::modules::logs::debug(const_cast<flecs::world&>(m_world), "[Renderer Module] Deserializing renderer entity.");
    }

    sandbox::properties renderer_module_t::serialize_renderer(flecs::entity renderer_entity) {
        return sandbox::properties{};
    }

    void renderer_module_t::register_renderer(const sandbox::properties& properties) {
        m_renderer = m_world.entity("::renderer").add<spectre_renderer_update_marker_t>();
        
        m_world.system<spectre_renderer_update_marker_t>("RendererSystem")
            .kind(flecs::OnUpdate)
            .each([this](flecs::entity entity, spectre_renderer_update_marker_t&) {
                this->render_frame();
            });

        deserialize_renderer(m_renderer, properties);
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
        
        static int frame_count = 0;
        if (frame_count++ == 0) {
        }

        EndDrawing();
    }

    void renderer_module_t::render(flecs::entity entity_to_render) {
        // Setup transform
        float pos_x = 0.0f, pos_y = 0.0f;
        float scale_x = 1.0f, scale_y = 1.0f;
        float origin_x = 0.0f, origin_y = 0.0f;
        float rotation = 0.0f;

        if (entity_to_render.has<spectre_2D_transform_component_t>()) {
            const auto* transform = entity_to_render.try_get<spectre_2D_transform_component_t>();
            if (transform) {
                pos_x = transform->position_x;
                pos_y = transform->position_y;
                scale_x = transform->scale_x;
                scale_y = transform->scale_y;
                origin_x = transform->origin_x;
                origin_y = transform->origin_y;
                rotation = transform->rotation;
            }
        }

        rlPushMatrix();
        rlTranslatef(pos_x, pos_y, 0.0f);
        rlRotatef(rotation, 0.0f, 0.0f, 1.0f);
        rlScalef(scale_x, scale_y, 1.0f);
        rlTranslatef(-origin_x, -origin_y, 0.0f);

        auto to_raylib_color = [](const spectre_color_t& c) {
            return Color{ (unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b, (unsigned char)c.a };
        };

        if (entity_to_render.has<spectre_rectange_renderable_t>()) {
            const auto* rect = entity_to_render.try_get<spectre_rectange_renderable_t>();
            if (rect) {
                std::cout << "Rendering rect: w=" << rect->width << ", h=" << rect->height 
                          << ", fill=" << rect->fill_color.r << "," << rect->fill_color.g << "," << rect->fill_color.b
                          << ", outline=" << rect->outline_color.r << "," << rect->outline_color.g << "," << rect->outline_color.b
                          << std::endl;
                DrawRectangle(0, 0, (int)rect->width, (int)rect->height, to_raylib_color(rect->fill_color));
                if (rect->outline_thickness > 0) {
                    DrawRectangleLinesEx(Rectangle{0, 0, rect->width, rect->height}, rect->outline_thickness, to_raylib_color(rect->outline_color));
                }
            }
        }

        if (entity_to_render.has<spectre_circle_renderable_t>()) {
            const auto* circle = entity_to_render.try_get<spectre_circle_renderable_t>();
            if (circle) {
                DrawCircleV(Vector2{0.0f, 0.0f}, circle->radius, to_raylib_color(circle->fill_color));
                if (circle->outline_thickness > 0) {
                    // raylib DrawCircleLines lacks thickness, but we can draw a ring
                    DrawRing(Vector2{0.0f, 0.0f}, circle->radius - circle->outline_thickness, circle->radius, 0, 360, 36, to_raylib_color(circle->outline_color));
                }
            }
        }

        if (entity_to_render.has<spectre_polygone_renderable_t>()) {
            const auto* poly = entity_to_render.try_get<spectre_polygone_renderable_t>();
            if (poly) {
                DrawPoly(Vector2{0.0f, 0.0f}, poly->point_count, poly->radius, 0.0f, to_raylib_color(poly->fill_color));
                if (poly->outline_thickness > 0) {
                    DrawPolyLinesEx(Vector2{0.0f, 0.0f}, poly->point_count, poly->radius, 0.0f, poly->outline_thickness, to_raylib_color(poly->outline_color));
                }
            }
        }

        if (entity_to_render.has<spectre_custom_polygone_renderable_t>()) {
            const auto* cpoly = entity_to_render.try_get<spectre_custom_polygone_renderable_t>();
            if (cpoly && cpoly->vertices && cpoly->vertex_count >= 3) {
                // Raylib's DrawTriangleFan can draw a fan of triangles. 
                // We'll construct Vector2 array and use it.
                std::vector<Vector2> points;
                for (uint32_t i = 0; i < cpoly->vertex_count; ++i) {
                    points.push_back({cpoly->vertices[i * 2], cpoly->vertices[i * 2 + 1]});
                }
                DrawTriangleFan(points.data(), points.size(), to_raylib_color(cpoly->fill_color));
                
                if (cpoly->outline_thickness > 0) {
                    for (uint32_t i = 0; i < cpoly->vertex_count; ++i) {
                        Vector2 p1 = points[i];
                        Vector2 p2 = points[(i + 1) % cpoly->vertex_count];
                        DrawLineEx(p1, p2, cpoly->outline_thickness, to_raylib_color(cpoly->outline_color));
                    }
                }
            }
        }

        if (entity_to_render.has<spectre_ligne_renderable_t>()) {
            const auto* line = entity_to_render.try_get<spectre_ligne_renderable_t>();
            if (line) {
                Vector2 startPos = {(float)line->position_x1, (float)line->position_y1};
                Vector2 endPos = {(float)line->position_x2, (float)line->position_y2};
                DrawLineEx(startPos, endPos, line->thickness, to_raylib_color(line->color));
            }
        }

        rlPopMatrix();
    }

    flecs::query<> renderer_module_t::find_renderable() const {
        return m_world.query<spectre_renderable_t>();
    }

}
