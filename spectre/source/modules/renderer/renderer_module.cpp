#include "renderer_module.h"
#include <spectre/abi/renderer_service.h>
#include <spectre/abi/renderer_components.h>
#include <sandbox/sdk/logs.hpp>
#include <raylib.h>
#include <rlgl.h>
#include <vector>
#include <algorithm>

namespace spectre::module {

    static Color to_raylib_color(const spectre_color_t& c) {
        return Color{
            static_cast<unsigned char>(c.r * 255.0f),
            static_cast<unsigned char>(c.g * 255.0f),
            static_cast<unsigned char>(c.b * 255.0f),
            static_cast<unsigned char>(c.a * 255.0f)
        };
    }

    RendererModule::RendererModule(flecs::world& world) : m_entity_world(world) {
        sandbox::modules::logs::trace(m_entity_world, "Renderer Module: Initializing...");

        // Register components
        m_entity_world.component<spectre_transform_2d_t>("Transform2D");
        m_entity_world.component<spectre_renderable_2d_t>("Renderable2D");
        m_entity_world.component<spectre_polygon_renderer_t>("PolygonRenderer");
        m_entity_world.component<spectre_line_renderer_t>("LineRenderer");

        m_render_system = m_entity_world.system("RenderSystem")
            .kind(flecs::OnUpdate)
            .run([this](flecs::iter& it) {
                BeginDrawing();
                ClearBackground(BLACK);

                // Collect entities to render and sort them by z_order
                struct RenderData {
                    flecs::entity entity;
                    spectre_transform_2d_t* transform;
                    spectre_renderable_2d_t* renderable;
                };
                
                std::vector<RenderData> renderables;
                
                // Query all renderables
                auto q = m_entity_world.query<spectre_transform_2d_t, spectre_renderable_2d_t>();
                q.each([&](flecs::entity e, spectre_transform_2d_t& transform, spectre_renderable_2d_t& rend) {
                    if (rend.is_visible) {
                        renderables.push_back({e, &transform, &rend});
                    }
                });

                // Sort by z_order
                std::sort(renderables.begin(), renderables.end(), [](const RenderData& a, const RenderData& b) {
                    return a.renderable->z_order < b.renderable->z_order;
                });

                // Render each
                for (auto& data : renderables) {
                    rlPushMatrix();
                    rlTranslatef(data.transform->position[0], data.transform->position[1], 0.0f);
                    rlRotatef(data.transform->rotation * RAD2DEG, 0.0f, 0.0f, 1.0f);
                    rlScalef(data.transform->scale[0], data.transform->scale[1], 1.0f);

                    if (data.renderable->type == SPECTRE_RENDER_2D_POLYGON) {
                        const spectre_polygon_renderer_t* poly = data.entity.try_get<spectre_polygon_renderer_t>();
                        if (poly) {
                            DrawPoly(Vector2{0.0f, 0.0f}, poly->point_count, poly->radius, 0.0f, to_raylib_color(poly->fill_color));
                            if (poly->outline_thickness > 0.0f) {
                                DrawPolyLinesEx(Vector2{0.0f, 0.0f}, poly->point_count, poly->radius, 0.0f, poly->outline_thickness, to_raylib_color(poly->outline_color));
                            }
                        }
                    } else if (data.renderable->type == SPECTRE_RENDER_2D_LINE) {
                        const spectre_line_renderer_t* line = data.entity.try_get<spectre_line_renderer_t>();
                        if (line) {
                            DrawLineEx(Vector2{0.0f, 0.0f}, Vector2{line->end_point_local[0], line->end_point_local[1]}, line->thickness, to_raylib_color(line->color));
                        }
                    }

                    rlPopMatrix();
                }

                EndDrawing();
            });
            
        sandbox::modules::logs::info(m_entity_world, "Renderer Module: Initialized.");
    }

    RendererModule::~RendererModule() {
        if (m_render_system.is_valid()) {
            m_render_system.destruct();
        }
    }
}

// ----------------------------------------------------------------------------
// C-ABI Implementations (Bridge to C++)
// ----------------------------------------------------------------------------
using spectre::module::RendererModule;

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
