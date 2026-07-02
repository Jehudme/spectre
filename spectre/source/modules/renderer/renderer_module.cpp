#include "renderer_module.h"
#include <spectre/abi/renderer_service.h>
#include <spectre/abi/renderer_components.h>
#include <spectre/abi/prefabs_service.h>
#include <utilities/properties_helpers.h>
#include <sandbox/sdk/logs.hpp>
#include <sandbox/sdk/properties.hpp>
#include <raylib.h>
#include <rlgl.h>
#include <algorithm>
#include <string>
#include <vector>

namespace spectre::module {

// ─────────────────────────────────────────────────────────────────────────────
// Helper Declarations
// ─────────────────────────────────────────────────────────────────────────────

    // Raylib color conversion
    inline Color         to_raylib_color(const spectre_color_t& color);

    // Draw dispatch helpers
    inline void          draw_polygon(const spectre_polygon_renderer_t* polygon);
    inline void          draw_line(const spectre_line_renderer_t* line);
    inline void          draw_rectangle(const spectre_rectangle_renderer_t* rect);
    inline void          draw_custom_polygon(const spectre_custom_polygon_renderer_t* polygon);

    // Component factory helpers
    static spectre_color_t read_required_color(sandbox::properties& props, const std::string& key,
                                               flecs::world& world, const char* component_name);
    static std::pair<float,float> read_required_vec2(sandbox::properties& props, const std::string& key,
                                                     flecs::world& world, const char* component_name);

// ─────────────────────────────────────────────────────────────────────────────
// Component Factories
// ─────────────────────────────────────────────────────────────────────────────

    static void factory_Transform2D(ecs_world_t* ecs, ecs_entity_t entity_id,
                                    sandbox_properties_handle_t props_handle) {
        sandbox::properties props(props_handle, false);
        flecs::world world(ecs);
        spectre_transform_2d_t transform = {0};

        auto position = utilities::try_read_vec2(props, "position");
        if (!position.has_value()) {
            sandbox::modules::logs::warn(world,
                "Transform2D factory: 'position' missing or malformed — defaulting to (0,0).");
        }
        transform.position[0] = position ? position->first  : 0.0f;
        transform.position[1] = position ? position->second : 0.0f;
        transform.rotation    = static_cast<float>(props.get<double>("rotation").value_or(0.0));

        auto scale = utilities::try_read_vec2(props, "scale");
        if (!scale.has_value()) {
            sandbox::modules::logs::warn(world,
                "Transform2D factory: 'scale' missing or malformed — defaulting to (1,1).");
        }
        transform.scale[0] = scale ? scale->first  : 1.0f;
        transform.scale[1] = scale ? scale->second : 1.0f;

        world.entity(entity_id).set<spectre_transform_2d_t>(transform);
    }

    static void factory_Renderable2D(ecs_world_t* ecs, ecs_entity_t entity_id,
                                     sandbox_properties_handle_t props_handle) {
        sandbox::properties props(props_handle, false);
        flecs::world world(ecs);

        spectre_renderable_2d_t renderable;
        renderable.is_visible = props.get<bool>("is_visible").value_or(true);
        renderable.z_order    = static_cast<uint32_t>(props.get<int64_t>("z_order").value_or(0));

        const std::string type_str = props.get<std::string>("type").value_or("");
        if (type_str.empty()) {
            sandbox::modules::logs::warn(world,
                "Renderable2D factory: 'type' key missing — defaulting to SPECTRE_RENDER_2D_POLYGON.");
        }

        if      (type_str == "SPECTRE_RENDER_2D_LINE")           renderable.type = SPECTRE_RENDER_2D_LINE;
        else if (type_str == "SPECTRE_RENDER_2D_RECTANGLE")      renderable.type = SPECTRE_RENDER_2D_RECTANGLE;
        else if (type_str == "SPECTRE_RENDER_2D_CUSTOM_POLYGON") renderable.type = SPECTRE_RENDER_2D_CUSTOM_POLYGON;
        else                                                      renderable.type = SPECTRE_RENDER_2D_POLYGON;

        world.entity(entity_id).set<spectre_renderable_2d_t>(renderable);
    }

    static void factory_PolygonRenderer(ecs_world_t* ecs, ecs_entity_t entity_id,
                                        sandbox_properties_handle_t props_handle) {
        sandbox::properties props(props_handle, false);
        flecs::world world(ecs);

        spectre_polygon_renderer_t polygon = {0};
        polygon.radius    = static_cast<float>(props.get<double>("radius").value_or(10.0));
        polygon.point_count = static_cast<uint32_t>(props.get<int64_t>("point_count").value_or(4));
        polygon.outline_thickness = static_cast<float>(props.get<double>("outline_thickness").value_or(1.0));

        auto fill_color = utilities::try_read_color(props, "fill_color");
        if (!fill_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "PolygonRenderer factory: 'fill_color' missing — defaulting to white.");
        }
        polygon.fill_color = fill_color.value_or(spectre_color_t{1,1,1,1});

        auto outline_color = utilities::try_read_color(props, "outline_color");
        if (!outline_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "PolygonRenderer factory: 'outline_color' missing — defaulting to white.");
        }
        polygon.outline_color = outline_color.value_or(spectre_color_t{1,1,1,1});

        world.entity(entity_id).set<spectre_polygon_renderer_t>(polygon);
    }

    static void factory_LineRenderer(ecs_world_t* ecs, ecs_entity_t entity_id,
                                     sandbox_properties_handle_t props_handle) {
        sandbox::properties props(props_handle, false);
        flecs::world world(ecs);

        spectre_line_renderer_t line = {0};
        line.thickness = static_cast<float>(props.get<double>("thickness").value_or(1.0));

        auto endpoint = utilities::try_read_vec2(props, "end_point_local");
        if (!endpoint.has_value()) {
            sandbox::modules::logs::warn(world,
                "LineRenderer factory: 'end_point_local' missing or malformed — defaulting to (0,0).");
        }
        line.end_point_local[0] = endpoint ? endpoint->first  : 0.0f;
        line.end_point_local[1] = endpoint ? endpoint->second : 0.0f;

        auto color = utilities::try_read_color(props, "color");
        if (!color.has_value()) {
            sandbox::modules::logs::warn(world,
                "LineRenderer factory: 'color' missing — defaulting to white.");
        }
        line.color = color.value_or(spectre_color_t{1,1,1,1});

        world.entity(entity_id).set<spectre_line_renderer_t>(line);
    }

    static void factory_RectangleRenderer(ecs_world_t* ecs, ecs_entity_t entity_id,
                                          sandbox_properties_handle_t props_handle) {
        sandbox::properties props(props_handle, false);
        flecs::world world(ecs);

        spectre_rectangle_renderer_t rect = {0};
        rect.width  = static_cast<float>(props.get<double>("width").value_or(50.0));
        rect.height = static_cast<float>(props.get<double>("height").value_or(50.0));
        rect.outline_thickness = static_cast<float>(props.get<double>("outline_thickness").value_or(1.0));

        auto fill_color = utilities::try_read_color(props, "fill_color");
        if (!fill_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "RectangleRenderer factory: 'fill_color' missing — defaulting to white.");
        }
        rect.fill_color = fill_color.value_or(spectre_color_t{1,1,1,1});

        auto outline_color = utilities::try_read_color(props, "outline_color");
        if (!outline_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "RectangleRenderer factory: 'outline_color' missing — defaulting to white.");
        }
        rect.outline_color = outline_color.value_or(spectre_color_t{1,1,1,1});

        world.entity(entity_id).set<spectre_rectangle_renderer_t>(rect);
    }

    static void factory_CustomPolygonRenderer(ecs_world_t* ecs, ecs_entity_t entity_id,
                                              sandbox_properties_handle_t props_handle) {
        sandbox::properties props(props_handle, false);
        flecs::world world(ecs);

        spectre_custom_polygon_renderer_t polygon = {0};
        const int vertex_count = static_cast<int>(props.get<int64_t>("vertex_count").value_or(0));

        if (vertex_count <= 0) {
            sandbox::modules::logs::warn(world,
                "CustomPolygonRenderer factory: 'vertex_count' is 0 or missing — no vertices created.");
        } else {
            polygon.vertices     = new float[vertex_count * 2];
            polygon.vertex_count = static_cast<uint32_t>(vertex_count);

            auto vertices_node = props.sub("vertices");
            if (!vertices_node.is_valid()) {
                sandbox::modules::logs::warn(world,
                    "CustomPolygonRenderer factory: 'vertices' node missing — all vertices will be (0,0).");
            }

            for (int vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
                const std::string vertex_key = std::to_string(vertex_index);
                auto vertex_node = vertices_node.sub(vertex_key);
                if (!vertex_node.is_valid()) {
                    sandbox::modules::logs::warn(world,
                        "CustomPolygonRenderer factory: Vertex '{}' missing — defaulting to (0,0).", vertex_key);
                }
                polygon.vertices[vertex_index * 2]     = static_cast<float>(vertex_node.get<double>("x").value_or(0.0));
                polygon.vertices[vertex_index * 2 + 1] = static_cast<float>(vertex_node.get<double>("y").value_or(0.0));
            }
        }

        auto fill_color = utilities::try_read_color(props, "fill_color");
        if (!fill_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "CustomPolygonRenderer factory: 'fill_color' missing — defaulting to white.");
        }
        polygon.fill_color = fill_color.value_or(spectre_color_t{1,1,1,1});

        auto outline_color = utilities::try_read_color(props, "outline_color");
        if (!outline_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "CustomPolygonRenderer factory: 'outline_color' missing — defaulting to white.");
        }
        polygon.outline_color     = outline_color.value_or(spectre_color_t{1,1,1,1});
        polygon.outline_thickness = static_cast<float>(props.get<double>("outline_thickness").value_or(1.0));

        world.entity(entity_id).set<spectre_custom_polygon_renderer_t>(polygon);
    }

// ─────────────────────────────────────────────────────────────────────────────
// Component Registration
// ─────────────────────────────────────────────────────────────────────────────

    static void register_all_components(flecs::world& world) {
        world.component<spectre_color_t>("spectre_color_t")
            .member<float>("r").member<float>("g").member<float>("b").member<float>("a");

        world.component<spectre_transform_2d_t>("Transform2D")
            .member<float>("position", 2)
            .member<float>("rotation")
            .member<float>("scale", 2);

        world.component<spectre_render_type_2d_t>("spectre_render_type_2d_t")
            .constant("SPECTRE_RENDER_2D_POLYGON",        SPECTRE_RENDER_2D_POLYGON)
            .constant("SPECTRE_RENDER_2D_LINE",            SPECTRE_RENDER_2D_LINE)
            .constant("SPECTRE_RENDER_2D_RECTANGLE",       SPECTRE_RENDER_2D_RECTANGLE)
            .constant("SPECTRE_RENDER_2D_CUSTOM_POLYGON",  SPECTRE_RENDER_2D_CUSTOM_POLYGON);

        world.component<spectre_renderable_2d_t>("Renderable2D")
            .member<spectre_render_type_2d_t>("type")
            .member<bool>("is_visible")
            .member<uint32_t>("z_order");

        world.component<spectre_polygon_renderer_t>("PolygonRenderer")
            .member<float>("radius")
            .member<uint32_t>("point_count")
            .member<spectre_color_t>("fill_color")
            .member<spectre_color_t>("outline_color")
            .member<float>("outline_thickness");

        world.component<spectre_line_renderer_t>("LineRenderer")
            .member<float>("end_point_local", 2)
            .member<spectre_color_t>("color")
            .member<float>("thickness");

        world.component<spectre_rectangle_renderer_t>("RectangleRenderer");

        world.component<spectre_custom_polygon_renderer_t>("CustomPolygonRenderer")
            .on_remove([](flecs::entity /*entity*/, spectre_custom_polygon_renderer_t& polygon) {
                delete[] polygon.vertices;
                polygon.vertices = nullptr;
            });
    }

// ─────────────────────────────────────────────────────────────────────────────
// Factory Registration
// ─────────────────────────────────────────────────────────────────────────────

    static void register_all_factories(flecs::world& world) {
        const auto* prefabs = SANDBOX_GET_SERVICE(world, spectre_prefabs_service_t);
        if (!prefabs || !prefabs->api) {
            sandbox::modules::logs::error(world,
                "Renderer Module: PrefabsService not available — component factories not registered.");
            return;
        }

        auto* ecs = world.c_ptr();
        prefabs->api->register_component_factory(ecs, "Transform2D",            factory_Transform2D);
        prefabs->api->register_component_factory(ecs, "Renderable2D",           factory_Renderable2D);
        prefabs->api->register_component_factory(ecs, "PolygonRenderer",        factory_PolygonRenderer);
        prefabs->api->register_component_factory(ecs, "LineRenderer",           factory_LineRenderer);
        prefabs->api->register_component_factory(ecs, "RectangleRenderer",      factory_RectangleRenderer);
        prefabs->api->register_component_factory(ecs, "CustomPolygonRenderer",  factory_CustomPolygonRenderer);
    }

// ─────────────────────────────────────────────────────────────────────────────
// Test Entity Spawning
// ─────────────────────────────────────────────────────────────────────────────

    static void spawn_test_entity_at(flecs::world& world, const char* prefab_name,
                                     float x, float y) {
        const auto* prefabs = SANDBOX_GET_SERVICE(world, spectre_prefabs_service_t);
        if (!prefabs || !prefabs->api) return;

        sandbox_properties_handle_t no_override = {0};
        const ecs_entity_t entity_id = prefabs->api->create_prefab(world.c_ptr(), prefab_name, no_override);

        if (!entity_id) {
            sandbox::modules::logs::warn(world,
                "Renderer Module: Failed to create test entity from prefab '{}'.", prefab_name);
            return;
        }

        const ecs_id_t transform_component_id = world.component<spectre_transform_2d_t>().id();
        const auto* existing_transform =
            static_cast<const spectre_transform_2d_t*>(
                ecs_get_id(world.c_ptr(), entity_id, transform_component_id));

        if (!existing_transform) {
            sandbox::modules::logs::warn(world,
                "Renderer Module: Prefab '{}' has no Transform2D — cannot reposition.", prefab_name);
            return;
        }

        spectre_transform_2d_t repositioned = *existing_transform;
        repositioned.position[0] = x;
        repositioned.position[1] = y;
        ecs_set_id(world.c_ptr(), entity_id, transform_component_id,
                   sizeof(spectre_transform_2d_t), &repositioned);
    }

// ─────────────────────────────────────────────────────────────────────────────
// MODULE LIFECYCLE
// ─────────────────────────────────────────────────────────────────────────────

    RendererModule::RendererModule(flecs::world& world) : m_entity_world(world) {
        sandbox::modules::logs::trace(m_entity_world, "Renderer Module: Initializing...");

        register_all_components(m_entity_world);

        // Build the per-frame render system.
        m_render_system = m_entity_world.system("RenderSystem")
            .kind(flecs::OnUpdate)
            .run([this](flecs::iter& /*iter*/) {
                BeginDrawing();
                ClearBackground(BLACK);
                render_frame();
                EndDrawing();
            });

        register_all_factories(m_entity_world);

        // Spawn test entities placed across the scene.
        spawn_test_entity_at(m_entity_world, "asteroid",           300.0f, 200.0f);
        spawn_test_entity_at(m_entity_world, "spaceship_striker",  640.0f, 400.0f);
        spawn_test_entity_at(m_entity_world, "space_station",     1000.0f, 500.0f);

        sandbox::modules::logs::info(m_entity_world, "Renderer Module: Initialized.");
    }

    RendererModule::~RendererModule() {
        if (m_render_system.is_valid()) {
            m_render_system.destruct();
        }
    }

    void RendererModule::render_frame() {
        // ── Phase A: Collect visible renderables ──────────────────────────────
        struct RenderEntry {
            flecs::entity           entity;
            spectre_transform_2d_t* transform;
            spectre_renderable_2d_t* renderable;
        };

        std::vector<RenderEntry> render_queue;

        m_entity_world.query<spectre_transform_2d_t, spectre_renderable_2d_t>()
            .each([&render_queue](flecs::entity entity,
                                  spectre_transform_2d_t& transform,
                                  spectre_renderable_2d_t& renderable) {
                if (renderable.is_visible) {
                    render_queue.push_back({ entity, &transform, &renderable });
                }
            });

        // ── Phase B: Sort by z-order (ascending = back to front) ──────────────
        std::sort(render_queue.begin(), render_queue.end(),
            [](const RenderEntry& lhs, const RenderEntry& rhs) {
                return lhs.renderable->z_order < rhs.renderable->z_order;
            });

        // ── Phase C: Draw each entry ──────────────────────────────────────────
        for (const auto& entry : render_queue) {
            rlPushMatrix();
            rlTranslatef(entry.transform->position[0], entry.transform->position[1], 0.0f);
            rlRotatef(entry.transform->rotation * RAD2DEG, 0.0f, 0.0f, 1.0f);
            rlScalef(entry.transform->scale[0], entry.transform->scale[1], 1.0f);

            switch (entry.renderable->type) {
                case SPECTRE_RENDER_2D_POLYGON:
                    if (const auto* polygon = entry.entity.try_get<spectre_polygon_renderer_t>())
                        draw_polygon(polygon);
                    break;
                case SPECTRE_RENDER_2D_LINE:
                    if (const auto* line = entry.entity.try_get<spectre_line_renderer_t>())
                        draw_line(line);
                    break;
                case SPECTRE_RENDER_2D_RECTANGLE:
                    if (const auto* rect = entry.entity.try_get<spectre_rectangle_renderer_t>())
                        draw_rectangle(rect);
                    break;
                case SPECTRE_RENDER_2D_CUSTOM_POLYGON:
                    if (const auto* cpoly = entry.entity.try_get<spectre_custom_polygon_renderer_t>())
                        draw_custom_polygon(cpoly);
                    break;
            }

            rlPopMatrix();
        }
    }

// ─────────────────────────────────────────────────────────────────────────────
// Helper Implementations
// ─────────────────────────────────────────────────────────────────────────────

    inline Color to_raylib_color(const spectre_color_t& color) {
        return Color{
            static_cast<unsigned char>(color.r * 255.0f),
            static_cast<unsigned char>(color.g * 255.0f),
            static_cast<unsigned char>(color.b * 255.0f),
            static_cast<unsigned char>(color.a * 255.0f)
        };
    }

    inline void draw_polygon(const spectre_polygon_renderer_t* polygon) {
        DrawPoly(Vector2{0,0}, polygon->point_count, polygon->radius, 0.0f,
                 to_raylib_color(polygon->fill_color));
        if (polygon->outline_thickness > 0.0f) {
            DrawPolyLinesEx(Vector2{0,0}, polygon->point_count, polygon->radius, 0.0f,
                            polygon->outline_thickness, to_raylib_color(polygon->outline_color));
        }
    }

    inline void draw_line(const spectre_line_renderer_t* line) {
        DrawLineEx(
            Vector2{0.0f, 0.0f},
            Vector2{line->end_point_local[0], line->end_point_local[1]},
            line->thickness,
            to_raylib_color(line->color)
        );
    }

    inline void draw_rectangle(const spectre_rectangle_renderer_t* rect) {
        const Rectangle bounds = {
            -rect->width  / 2.0f,
            -rect->height / 2.0f,
             rect->width,
             rect->height
        };
        DrawRectangleRec(bounds, to_raylib_color(rect->fill_color));
        if (rect->outline_thickness > 0.0f) {
            DrawRectangleLinesEx(bounds, rect->outline_thickness, to_raylib_color(rect->outline_color));
        }
    }

    inline void draw_custom_polygon(const spectre_custom_polygon_renderer_t* polygon) {
        if (!polygon->vertices || polygon->vertex_count < 3) return;

        // Compute centroid for fan triangulation.
        Vector2 centroid = {0.0f, 0.0f};
        for (uint32_t vertex_index = 0; vertex_index < polygon->vertex_count; ++vertex_index) {
            centroid.x += polygon->vertices[vertex_index * 2];
            centroid.y += polygon->vertices[vertex_index * 2 + 1];
        }
        centroid.x /= static_cast<float>(polygon->vertex_count);
        centroid.y /= static_cast<float>(polygon->vertex_count);

        const Color fill_color    = to_raylib_color(polygon->fill_color);
        const Color outline_color = to_raylib_color(polygon->outline_color);

        for (uint32_t vertex_index = 0; vertex_index < polygon->vertex_count; ++vertex_index) {
            const uint32_t next_index = (vertex_index + 1) % polygon->vertex_count;
            const Vector2 current_vertex = { polygon->vertices[vertex_index * 2],
                                             polygon->vertices[vertex_index * 2 + 1] };
            const Vector2 next_vertex    = { polygon->vertices[next_index * 2],
                                             polygon->vertices[next_index * 2 + 1] };

            DrawTriangle(centroid, current_vertex, next_vertex, fill_color);

            if (polygon->outline_thickness > 0.0f) {
                DrawLineEx(current_vertex, next_vertex, polygon->outline_thickness, outline_color);
            }
        }
    }

} // namespace spectre::module


// ─────────────────────────────────────────────────────────────────────────────
// C-ABI EXPORT
// ─────────────────────────────────────────────────────────────────────────────

static spectre_renderer_api_t g_renderer_api = {};

SANDBOX_DEFINE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t, &g_renderer_api)

// ─────────────────────────────────────────────────────────────────────────────
// MODULE REGISTRATION
// ─────────────────────────────────────────────────────────────────────────────

namespace spectre::module {

    static sandbox_requirement_info_t renderer_requirements[] = {
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "logs",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "window",
            .architecture  = "spectre",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "prefabs",
            .architecture  = "spectre",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(RendererModule, {
        .name          = "renderer",
        .description   = "Raylib 2D batch renderer with z-ordered draw calls.",
        .architecture  = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service       = &spectre_renderer_service_t_info,
        .requirements  = renderer_requirements,
        .requirement_count = 3
    })

} // namespace spectre::module