#include "renderer_module.h"
#include <spectre/abi/renderer_service.h>
#include <spectre/abi/renderer_components.h>
#include <spectre/abi/prefabs_service.h>
#include <sandbox/sdk/logs.hpp>
#include <sandbox/sdk/properties.hpp>

#include <raylib.h>
#include <rlgl.h>

#include <vector>
#include <algorithm>

namespace spectre::module {

    // ------------------------------------------------------------------------
    // Helper Declarations
    // ------------------------------------------------------------------------
    inline Color to_raylib_color(const spectre_color_t& c);
    inline void draw_polygon(const spectre_polygon_renderer_t* poly);
    inline void draw_line(const spectre_line_renderer_t* line);
    inline void draw_rectangle(const spectre_rectangle_renderer_t* rect);
    inline void draw_custom_polygon(const spectre_custom_polygon_renderer_t* poly);

    // ------------------------------------------------------------------------
    // FACTORY FUNCTIONS
    // ------------------------------------------------------------------------
    static void factory_Transform2D(ecs_world_t* ecs, ecs_entity_t e, sandbox_properties_handle_t props_h) {
        sandbox::properties props(props_h, false);
        spectre_transform_2d_t t = {0};
        
        auto pos_node = props.sub("position");
        if (pos_node.is_valid() && pos_node.has("x") && pos_node.has("y")) {
            t.position[0] = (float)pos_node.get<double>("x").value_or(0.0);
            t.position[1] = (float)pos_node.get<double>("y").value_or(0.0);
        }
        t.rotation = (float)props.get<double>("rotation").value_or(0.0);
        auto scale_node = props.sub("scale");
        if (scale_node.is_valid() && scale_node.has("x") && scale_node.has("y")) {
            t.scale[0] = (float)scale_node.get<double>("x").value_or(1.0);
            t.scale[1] = (float)scale_node.get<double>("y").value_or(1.0);
        } else {
            t.scale[0] = 1.0f;
            t.scale[1] = 1.0f;
        }
        flecs::world(ecs).entity(e).set<spectre_transform_2d_t>(t);
    }

    static void factory_Renderable2D(ecs_world_t* ecs, ecs_entity_t e, sandbox_properties_handle_t props_h) {
        sandbox::properties props(props_h, false);
        spectre_renderable_2d_t r;
        r.type = SPECTRE_RENDER_2D_POLYGON;
        r.is_visible = false;
        r.z_order = 0;
        
        std::string type_str = props.get<std::string>("type").value_or("SPECTRE_RENDER_2D_POLYGON");
        if (type_str == "SPECTRE_RENDER_2D_POLYGON") r.type = SPECTRE_RENDER_2D_POLYGON;
        else if (type_str == "SPECTRE_RENDER_2D_LINE") r.type = SPECTRE_RENDER_2D_LINE;
        else if (type_str == "SPECTRE_RENDER_2D_RECTANGLE") r.type = SPECTRE_RENDER_2D_RECTANGLE;
        else if (type_str == "SPECTRE_RENDER_2D_CUSTOM_POLYGON") r.type = SPECTRE_RENDER_2D_CUSTOM_POLYGON;
        
        r.is_visible = props.get<bool>("is_visible").value_or(true);
        r.z_order = (uint32_t)props.get<int64_t>("z_order").value_or(0);
        flecs::world(ecs).entity(e).set<spectre_renderable_2d_t>(r);
    }

    static spectre_color_t read_color(sandbox::properties& props, const std::string& key) {
        auto color_node = props.sub(key);
        if (color_node.is_valid()) {
            // Try object first (r, g, b, a)
            if (color_node.has("r") && color_node.has("g") && color_node.has("b") && color_node.has("a")) {
                float r = color_node.get<double>("r").value_or(1.0);
                float g = color_node.get<double>("g").value_or(1.0);
                float b = color_node.get<double>("b").value_or(1.0);
                float a = color_node.get<double>("a").value_or(1.0);
                return spectre_color_t{r, g, b, a};
            }
        }
        
        // Fallback
        return spectre_color_t{1.0f, 1.0f, 1.0f, 1.0f};
    }

    static void factory_PolygonRenderer(ecs_world_t* ecs, ecs_entity_t e, sandbox_properties_handle_t props_h) {
        sandbox::properties props(props_h, false);
        spectre_polygon_renderer_t r = {0};
        r.radius = (float)props.get<double>("radius").value_or(10.0);
        r.point_count = (uint32_t)props.get<int64_t>("point_count").value_or(4);
        r.fill_color = read_color(props, "fill_color");
        r.outline_color = read_color(props, "outline_color");
        r.outline_thickness = (float)props.get<double>("outline_thickness").value_or(1.0);
        flecs::world(ecs).entity(e).set<spectre_polygon_renderer_t>(r);
    }

    static void factory_LineRenderer(ecs_world_t* ecs, ecs_entity_t e, sandbox_properties_handle_t props_h) {
        sandbox::properties props(props_h, false);
        spectre_line_renderer_t r = {0};
        auto ep_node = props.sub("end_point_local");
        if (ep_node.is_valid() && ep_node.has("x") && ep_node.has("y")) {
            r.end_point_local[0] = (float)ep_node.get<double>("x").value_or(0.0);
            r.end_point_local[1] = (float)ep_node.get<double>("y").value_or(0.0);
        }
        r.color = read_color(props, "color");
        r.thickness = (float)props.get<double>("thickness").value_or(1.0);
        flecs::world(ecs).entity(e).set<spectre_line_renderer_t>(r);
    }

    static void factory_RectangleRenderer(ecs_world_t* ecs, ecs_entity_t e, sandbox_properties_handle_t props_h) {
        sandbox::properties props(props_h, false);
        spectre_rectangle_renderer_t r = {0};
        r.width = (float)props.get<double>("width").value_or(50.0);
        r.height = (float)props.get<double>("height").value_or(50.0);
        r.fill_color = read_color(props, "fill_color");
        r.outline_color = read_color(props, "outline_color");
        r.outline_thickness = (float)props.get<double>("outline_thickness").value_or(1.0);
        flecs::world(ecs).entity(e).set<spectre_rectangle_renderer_t>(r);
    }

    static void factory_CustomPolygonRenderer(ecs_world_t* ecs, ecs_entity_t e, sandbox_properties_handle_t props_h) {
        sandbox::properties props(props_h, false);
        spectre_custom_polygon_renderer_t r = {0};
        
        int count = (int)props.get<int64_t>("vertex_count").value_or(0);
        if (count > 0) {
            r.vertices = new float[count * 2];
            r.vertex_count = count;
            auto vertices_node = props.sub("vertices");
            if (vertices_node.is_valid()) {
                for (int i = 0; i < count; ++i) {
                    std::string key = std::to_string(i);
                    auto v_node = vertices_node.sub(key);
                    r.vertices[i*2] = (float)v_node.get<double>("x").value_or(0.0);
                    r.vertices[i*2+1] = (float)v_node.get<double>("y").value_or(0.0);
                }
            }
        }
        r.fill_color = read_color(props, "fill_color");
        r.outline_color = read_color(props, "outline_color");
        r.outline_thickness = (float)props.get<double>("outline_thickness").value_or(1.0);
        flecs::world(ecs).entity(e).set<spectre_custom_polygon_renderer_t>(r);
    }

    // ------------------------------------------------------------------------
    // MODULE LIFECYCLE
    // ------------------------------------------------------------------------
    RendererModule::RendererModule(flecs::world& world) : m_entity_world(world) {
        sandbox::modules::logs::trace(m_entity_world, "Renderer Module: Initializing...");

        // Register components with reflection members
        m_entity_world.component<spectre_color_t>("spectre_color_t")
            .member<float>("r")
            .member<float>("g")
            .member<float>("b")
            .member<float>("a");

        m_entity_world.component<spectre_transform_2d_t>("Transform2D")
            .member<float>("position", 2)
            .member<float>("rotation")
            .member<float>("scale", 2);
            
        m_entity_world.component<spectre_render_type_2d_t>("spectre_render_type_2d_t")
            .constant("SPECTRE_RENDER_2D_POLYGON", SPECTRE_RENDER_2D_POLYGON)
            .constant("SPECTRE_RENDER_2D_LINE", SPECTRE_RENDER_2D_LINE)
            .constant("SPECTRE_RENDER_2D_RECTANGLE", SPECTRE_RENDER_2D_RECTANGLE)
            .constant("SPECTRE_RENDER_2D_CUSTOM_POLYGON", SPECTRE_RENDER_2D_CUSTOM_POLYGON);

        m_entity_world.component<spectre_renderable_2d_t>("Renderable2D")
            .member<spectre_render_type_2d_t>("type")
            .member<bool>("is_visible")
            .member<uint32_t>("z_order");

        m_entity_world.component<spectre_polygon_renderer_t>("PolygonRenderer")
            .member<float>("radius")
            .member<uint32_t>("point_count")
            .member<spectre_color_t>("fill_color")
            .member<spectre_color_t>("outline_color")
            .member<float>("outline_thickness");

        m_entity_world.component<spectre_line_renderer_t>("LineRenderer")
            .member<float>("end_point_local", 2)
            .member<spectre_color_t>("color")
            .member<float>("thickness");

        m_entity_world.component<spectre_rectangle_renderer_t>("RectangleRenderer");
        m_entity_world.component<spectre_custom_polygon_renderer_t>("CustomPolygonRenderer")
            .on_remove([](flecs::entity e, spectre_custom_polygon_renderer_t& cpoly) {
                if (cpoly.vertices) {
                    delete[] cpoly.vertices;
                    cpoly.vertices = nullptr;
                }
            });

        // 2. Define the main rendering system
        m_render_system = m_entity_world.system("RenderSystem")
            .kind(flecs::OnUpdate)
            .run([this](flecs::iter& iterator) {

                BeginDrawing();
                ClearBackground(BLACK);

                // --- Phase A: Data Collection ---
                struct RenderData {
                    flecs::entity entity;
                    spectre_transform_2d_t* transform;
                    spectre_renderable_2d_t* renderable;
                };

                std::vector<RenderData> sorted_renderables;

                // Query all entities that have both a Transform and the Renderable Flag
                auto renderable_query = m_entity_world.query<spectre_transform_2d_t, spectre_renderable_2d_t>();

                renderable_query.each([&](flecs::entity current_entity, spectre_transform_2d_t& transform, spectre_renderable_2d_t& renderable_flag) {
                    if (renderable_flag.is_visible) {
                        sorted_renderables.push_back({current_entity, &transform, &renderable_flag});
                    }
                });

                // --- Phase B: Depth Sorting (Z-Order) ---
                std::sort(sorted_renderables.begin(), sorted_renderables.end(), [](const RenderData& a, const RenderData& b) {
                    return a.renderable->z_order < b.renderable->z_order;
                });

                // --- Phase C: Batch Rendering ---
                for (const auto& data : sorted_renderables) {
                    rlPushMatrix();

                    // Apply Spatial Transform
                    rlTranslatef(data.transform->position[0], data.transform->position[1], 0.0f);
                    rlRotatef(data.transform->rotation * RAD2DEG, 0.0f, 0.0f, 1.0f);
                    rlScalef(data.transform->scale[0], data.transform->scale[1], 1.0f);

                    // Route based on Shape Type
                    if (data.renderable->type == SPECTRE_RENDER_2D_POLYGON) {
                        if (const auto* poly = data.entity.try_get<spectre_polygon_renderer_t>()) draw_polygon(poly);
                    } else if (data.renderable->type == SPECTRE_RENDER_2D_LINE) {
                        if (const auto* line = data.entity.try_get<spectre_line_renderer_t>()) draw_line(line);
                    } else if (data.renderable->type == SPECTRE_RENDER_2D_RECTANGLE) {
                        if (const auto* rect = data.entity.try_get<spectre_rectangle_renderer_t>()) draw_rectangle(rect);
                    } else if (data.renderable->type == SPECTRE_RENDER_2D_CUSTOM_POLYGON) {
                        if (const auto* cpoly = data.entity.try_get<spectre_custom_polygon_renderer_t>()) draw_custom_polygon(cpoly);
                    }

                    rlPopMatrix();
                }

                EndDrawing();
            });

        // --------------------------------------------------------------------
        // Register factories to prefabs module
        const auto* prefabs = SANDBOX_GET_SERVICE(m_entity_world, spectre_prefabs_service_t);
        if (prefabs && prefabs->api) {
            prefabs->api->register_component_factory(m_entity_world.c_ptr(), "Transform2D", factory_Transform2D);
            prefabs->api->register_component_factory(m_entity_world.c_ptr(), "Renderable2D", factory_Renderable2D);
            prefabs->api->register_component_factory(m_entity_world.c_ptr(), "PolygonRenderer", factory_PolygonRenderer);
            prefabs->api->register_component_factory(m_entity_world.c_ptr(), "LineRenderer", factory_LineRenderer);
            prefabs->api->register_component_factory(m_entity_world.c_ptr(), "RectangleRenderer", factory_RectangleRenderer);
            prefabs->api->register_component_factory(m_entity_world.c_ptr(), "CustomPolygonRenderer", factory_CustomPolygonRenderer);
            
            // Create test prefabs from JSON
            sandbox_properties_handle_t empty = {0};
            
            auto asteroid = prefabs->api->create_prefab(m_entity_world.c_ptr(), "asteroid", empty);
            if (asteroid) {
                auto comp_id = m_entity_world.component<spectre_transform_2d_t>().id();
                const auto* t = (const spectre_transform_2d_t*)ecs_get_id(m_entity_world.c_ptr(), asteroid, comp_id);
                if (t) {
                    spectre_transform_2d_t nt = *t;
                    nt.position[0] = 300.0f;
                    nt.position[1] = 200.0f;
                    ecs_set_id(m_entity_world.c_ptr(), asteroid, comp_id, sizeof(spectre_transform_2d_t), &nt);
                }
            }
            
            auto ship = prefabs->api->create_prefab(m_entity_world.c_ptr(), "spaceship_striker", empty);
            if (ship) {
                auto comp_id = m_entity_world.component<spectre_transform_2d_t>().id();
                const auto* t = (const spectre_transform_2d_t*)ecs_get_id(m_entity_world.c_ptr(), ship, comp_id);
                if (t) {
                    spectre_transform_2d_t nt = *t;
                    nt.position[0] = 640.0f;
                    nt.position[1] = 400.0f;
                    ecs_set_id(m_entity_world.c_ptr(), ship, comp_id, sizeof(spectre_transform_2d_t), &nt);
                }
            }
            
            auto station = prefabs->api->create_prefab(m_entity_world.c_ptr(), "space_station", empty);
            if (station) {
                auto comp_id = m_entity_world.component<spectre_transform_2d_t>().id();
                const auto* t = (const spectre_transform_2d_t*)ecs_get_id(m_entity_world.c_ptr(), station, comp_id);
                if (t) {
                    spectre_transform_2d_t nt = *t;
                    nt.position[0] = 1000.0f;
                    nt.position[1] = 500.0f;
                    ecs_set_id(m_entity_world.c_ptr(), station, comp_id, sizeof(spectre_transform_2d_t), &nt);
                }
            }
        }

        sandbox::modules::logs::info(m_entity_world, "Renderer Module: Initialized with test entities.");
    }

    RendererModule::~RendererModule() {
        if (m_render_system.is_valid()) {
            m_render_system.destruct();
        }
    }

    // ------------------------------------------------------------------------
    // Helper Implementations
    // ------------------------------------------------------------------------

    inline Color to_raylib_color(const spectre_color_t& c) {
        return Color{
            static_cast<unsigned char>(c.r * 255.0f),
            static_cast<unsigned char>(c.g * 255.0f),
            static_cast<unsigned char>(c.b * 255.0f),
            static_cast<unsigned char>(c.a * 255.0f)
        };
    }

    inline void draw_polygon(const spectre_polygon_renderer_t* poly) {
        DrawPoly(Vector2{0.0f, 0.0f}, poly->point_count, poly->radius, 0.0f, to_raylib_color(poly->fill_color));
        if (poly->outline_thickness > 0.0f) {
            DrawPolyLinesEx(Vector2{0.0f, 0.0f}, poly->point_count, poly->radius, 0.0f, poly->outline_thickness, to_raylib_color(poly->outline_color));
        }
    }

    inline void draw_line(const spectre_line_renderer_t* line) {
        DrawLineEx(Vector2{0.0f, 0.0f}, Vector2{line->end_point_local[0], line->end_point_local[1]}, line->thickness, to_raylib_color(line->color));
    }

    inline void draw_rectangle(const spectre_rectangle_renderer_t* rect) {
        Rectangle r = { -rect->width / 2.0f, -rect->height / 2.0f, rect->width, rect->height };
        DrawRectangleRec(r, to_raylib_color(rect->fill_color));
        if (rect->outline_thickness > 0.0f) {
            DrawRectangleLinesEx(r, rect->outline_thickness, to_raylib_color(rect->outline_color));
        }
    }

    inline void draw_custom_polygon(const spectre_custom_polygon_renderer_t* poly) {
        if (!poly->vertices || poly->vertex_count < 3) return;

        Vector2 center = {0, 0};
        for (uint32_t i = 0; i < poly->vertex_count; ++i) {
            center.x += poly->vertices[i * 2];
            center.y += poly->vertices[i * 2 + 1];
        }
        center.x /= (float)poly->vertex_count;
        center.y /= (float)poly->vertex_count;
        
        for (uint32_t i = 0; i < poly->vertex_count; ++i) {
            Vector2 p1 = { poly->vertices[i * 2], poly->vertices[i * 2 + 1] };
            uint32_t next = (i + 1) % poly->vertex_count;
            Vector2 p2 = { poly->vertices[next * 2], poly->vertices[next * 2 + 1] };
            
            DrawTriangle(center, p1, p2, to_raylib_color(poly->fill_color));
            
            if (poly->outline_thickness > 0.0f) {
                DrawLineEx(p1, p2, poly->outline_thickness, to_raylib_color(poly->outline_color));
            }
        }
    }

}

// ----------------------------------------------------------------------------
// C-ABI EXPORT (Bridge to Engine Core)
// ----------------------------------------------------------------------------
using spectre::module::RendererModule;

// Left empty for now, waiting for global rendering state functions
static spectre_renderer_api_t g_renderer_api = {
};

SANDBOX_DEFINE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t, &g_renderer_api)

namespace spectre::module {

    static sandbox_requirement_info_t renderer_requirements[] = {
        {
            .kind = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name = "logs",
            .architecture = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name = "window",
            .architecture = "spectre",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name = "prefabs",
            .architecture = "spectre",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(RendererModule, {
        .name = "renderer",
        .description = "Simple Raylib Renderer Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_renderer_service_t_info,
        .requirements = renderer_requirements,
        .requirement_count = 3
    })
}