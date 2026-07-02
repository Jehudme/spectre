#include "renderer_module.h"
#include <spectre/abi/renderer_service.h>
#include <spectre/abi/renderer_components.h>
#include <sandbox/sdk/logs.hpp>

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
    inline void draw_complex_polygon(const spectre_complex_polygon_renderer_t* poly);

    // ------------------------------------------------------------------------
    // MODULE LIFECYCLE
    // ------------------------------------------------------------------------
    RendererModule::RendererModule(flecs::world& world) : m_entity_world(world) {
        sandbox::modules::logs::trace(m_entity_world, "Renderer Module: Initializing...");

        // Register components
        m_entity_world.component<spectre_transform_2d_t>("Transform2D");
        m_entity_world.component<spectre_renderable_2d_t>("Renderable2D");
        m_entity_world.component<spectre_polygon_renderer_t>("PolygonRenderer");
        m_entity_world.component<spectre_line_renderer_t>("LineRenderer");
        m_entity_world.component<spectre_rectangle_renderer_t>("RectangleRenderer");
        m_entity_world.component<spectre_complex_polygon_renderer_t>("ComplexPolygonRenderer");

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
                    } else if (data.renderable->type == SPECTRE_RENDER_2D_COMPLEX_POLYGON) {
                        if (const auto* cpoly = data.entity.try_get<spectre_complex_polygon_renderer_t>()) draw_complex_polygon(cpoly);
                    }

                    rlPopMatrix();
                }

                EndDrawing();
            });

        // --------------------------------------------------------------------
        // TEST ENTITIES (Bootstrapping visual verification)
        // --------------------------------------------------------------------

        // 1. Test Triangle
        m_entity_world.entity("TestTriangle")
            .set<spectre_transform_2d_t>({
                .position = {400.0f, 360.0f},
                .rotation = 0.0f,
                .scale = {1.0f, 1.0f}
            })
            .set<spectre_renderable_2d_t>({
                .type = SPECTRE_RENDER_2D_POLYGON,
                .is_visible = true,
                .z_order = 1
            })
            .set<spectre_polygon_renderer_t>({
                .radius = 100.0f,
                .point_count = 3,
                .fill_color = {1.0f, 0.0f, 0.0f, 1.0f}, // Red
                .outline_color = {1.0f, 1.0f, 1.0f, 1.0f}, // White
                .outline_thickness = 5.0f
            });

        // 2. Test Square/Rectangle (Rotated)
        m_entity_world.entity("TestSquare")
            .set<spectre_transform_2d_t>({
                .position = {800.0f, 360.0f},
                .rotation = 45.0f * (PI / 180.0f),
                .scale = {1.5f, 1.0f}
            })
            .set<spectre_renderable_2d_t>({
                .type = SPECTRE_RENDER_2D_POLYGON,
                .is_visible = true,
                .z_order = 2
            })
            .set<spectre_polygon_renderer_t>({
                .radius = 100.0f,
                .point_count = 4,
                .fill_color = {0.0f, 0.0f, 1.0f, 1.0f}, // Blue
                .outline_color = {0.0f, 1.0f, 0.0f, 1.0f}, // Green
                .outline_thickness = 3.0f
            });

        // 3. Test Line
        m_entity_world.entity("TestLine")
            .set<spectre_transform_2d_t>({
                .position = {200.0f, 100.0f},
                .rotation = 0.0f,
                .scale = {1.0f, 1.0f}
            })
            .set<spectre_renderable_2d_t>({
                .type = SPECTRE_RENDER_2D_LINE,
                .is_visible = true,
                .z_order = 3
            })
            .set<spectre_line_renderer_t>({
                .end_point_local = {800.0f, 100.0f},
                .color = {1.0f, 1.0f, 0.0f, 1.0f}, // Yellow
                .thickness = 10.0f
            });

        // 4. Test Rectangle
        m_entity_world.entity("TestRect")
            .set<spectre_transform_2d_t>({
                .position = {200.0f, 600.0f},
                .rotation = 15.0f * (PI / 180.0f),
                .scale = {1.0f, 1.0f}
            })
            .set<spectre_renderable_2d_t>({
                .type = SPECTRE_RENDER_2D_RECTANGLE,
                .is_visible = true,
                .z_order = 4
            })
            .set<spectre_rectangle_renderer_t>({
                .width = 150.0f,
                .height = 80.0f,
                .fill_color = {1.0f, 0.5f, 0.0f, 1.0f}, // Orange
                .outline_color = {1.0f, 1.0f, 1.0f, 1.0f}, // White
                .outline_thickness = 4.0f
            });

        // 5. Test Complex Polygon (Star shape)
        static float star_points[] = {
            0.0f, -50.0f,
            15.0f, -15.0f,
            50.0f, -15.0f,
            20.0f, 5.0f,
            30.0f, 40.0f,
            0.0f, 20.0f,
            -30.0f, 40.0f,
            -20.0f, 5.0f,
            -50.0f, -15.0f,
            -15.0f, -15.0f
        };
        m_entity_world.entity("TestStar")
            .set<spectre_transform_2d_t>({
                .position = {600.0f, 600.0f},
                .rotation = 0.0f,
                .scale = {1.5f, 1.5f}
            })
            .set<spectre_renderable_2d_t>({
                .type = SPECTRE_RENDER_2D_COMPLEX_POLYGON,
                .is_visible = true,
                .z_order = 5
            })
            .set<spectre_complex_polygon_renderer_t>({
                .vertices = star_points,
                .vertex_count = 10,
                .fill_color = {0.8f, 0.2f, 0.8f, 1.0f}, // Purple
                .outline_color = {1.0f, 1.0f, 0.0f, 1.0f}, // Yellow
                .outline_thickness = 2.0f
            });

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

    inline void draw_complex_polygon(const spectre_complex_polygon_renderer_t* poly) {
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
        .requirement_count = 2
    })
}