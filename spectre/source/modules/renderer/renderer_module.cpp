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

        transform.z = static_cast<float>(props.get<double>("z").value_or(0.0));

        world.entity(entity_id).set<spectre_transform_2d_t>(transform);
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
                "PolygoneRenderable factory: 'fill_color' missing — defaulting to white.");
        }
        polygon.fill_color = fill_color.value_or(spectre_color_t{1,1,1,1});

        auto outline_color = utilities::try_read_color(props, "outline_color");
        if (!outline_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "PolygoneRenderable factory: 'outline_color' missing — defaulting to white.");
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
                "LineRenderable factory: 'end_point_local' missing or malformed — defaulting to (0,0).");
        }
        line.end_point_local[0] = endpoint ? endpoint->first  : 0.0f;
        line.end_point_local[1] = endpoint ? endpoint->second : 0.0f;

        auto color = utilities::try_read_color(props, "color");
        if (!color.has_value()) {
            sandbox::modules::logs::warn(world,
                "LineRenderable factory: 'color' missing — defaulting to white.");
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
                "RectangleRenderable factory: 'fill_color' missing — defaulting to white.");
        }
        rect.fill_color = fill_color.value_or(spectre_color_t{1,1,1,1});

        auto outline_color = utilities::try_read_color(props, "outline_color");
        if (!outline_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "RectangleRenderable factory: 'outline_color' missing — defaulting to white.");
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
                "CustomPolygoneRenderable factory: 'vertex_count' is 0 or missing — no vertices created.");
        } else {
            polygon.vertices     = new float[vertex_count * 2];
            polygon.vertex_count = static_cast<uint32_t>(vertex_count);

            auto vertices_node = props.sub("vertices");
            if (!vertices_node.is_valid()) {
                sandbox::modules::logs::warn(world,
                    "CustomPolygoneRenderable factory: 'vertices' node missing — all vertices will be (0,0).");
            }

            for (int vertex_index = 0; vertex_index < vertex_count; ++vertex_index) {
                const std::string vertex_key = std::to_string(vertex_index);
                auto vertex_node = vertices_node.sub(vertex_key);
                if (!vertex_node.is_valid()) {
                    sandbox::modules::logs::warn(world,
                        "CustomPolygoneRenderable factory: Vertex '{}' missing — defaulting to (0,0).", vertex_key);
                }
                polygon.vertices[vertex_index * 2]     = static_cast<float>(vertex_node.get<double>("x").value_or(0.0));
                polygon.vertices[vertex_index * 2 + 1] = static_cast<float>(vertex_node.get<double>("y").value_or(0.0));
            }
        }

        auto fill_color = utilities::try_read_color(props, "fill_color");
        if (!fill_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "CustomPolygoneRenderable factory: 'fill_color' missing — defaulting to white.");
        }
        polygon.fill_color = fill_color.value_or(spectre_color_t{1,1,1,1});

        auto outline_color = utilities::try_read_color(props, "outline_color");
        if (!outline_color.has_value()) {
            sandbox::modules::logs::warn(world,
                "CustomPolygoneRenderable factory: 'outline_color' missing — defaulting to white.");
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
            .member<float>("scale", 2)
            .member<float>("z");

        world.component<spectre_polygon_renderer_t>("PolygoneRenderable")
            .member<float>("radius")
            .member<uint32_t>("point_count")
            .member<spectre_color_t>("fill_color")
            .member<spectre_color_t>("outline_color")
            .member<float>("outline_thickness");

        world.component<spectre_line_renderer_t>("LineRenderable")
            .member<float>("end_point_local", 2)
            .member<spectre_color_t>("color")
            .member<float>("thickness");

        world.component<spectre_rectangle_renderer_t>("RectangleRenderable");

        world.component<spectre_custom_polygon_renderer_t>("CustomPolygoneRenderable")
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
        prefabs->api->register_component_factory(ecs, "PolygoneRenderable",     factory_PolygonRenderer);
        prefabs->api->register_component_factory(ecs, "LineRenderable",         factory_LineRenderer);
        prefabs->api->register_component_factory(ecs, "RectangleRenderable",    factory_RectangleRenderer);
        prefabs->api->register_component_factory(ecs, "CustomPolygoneRenderable",factory_CustomPolygonRenderer);
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

        // Use create_entity to properly test the PrefabsModule instantiation
        if (const auto* prefabs = SANDBOX_GET_SERVICE(m_entity_world, spectre_prefabs_service_t)) {
            // Instantiate object_1 with an override
            std::string obj1_json = R"({
                "prefab": "object_1",
                "components": {
                    "Transform2D": {
                        "position": { "x": 200, "y": 200 }
                    }
                }
            })";
            sandbox::properties obj1_props(obj1_json, sandbox::properties::Format::JSON);
            prefabs->api->create_entity(m_entity_world.c_ptr(), obj1_props.get_raw());

            // Instantiate object_2 (which has object_1 as a child)
            std::string obj2_json = R"({
                "prefab": "object_2",
                "components": {
                    "Transform2D": {
                        "position": { "x": 600, "y": 300 }
                    }
                }
            })";
            sandbox::properties obj2_props(obj2_json, sandbox::properties::Format::JSON);
            prefabs->api->create_entity(m_entity_world.c_ptr(), obj2_props.get_raw());
        }

        // Create the persistent render query, ordered by Transform2D's z value
        m_render_query = m_entity_world.query_builder<spectre_transform_2d_t>()
            .order_by<spectre_transform_2d_t>([](flecs::entity_t, const spectre_transform_2d_t* t1, flecs::entity_t, const spectre_transform_2d_t* t2) {
                if (t1->z < t2->z) return -1;
                if (t1->z > t2->z) return 1;
                return 0;
            })
            .build();

        sandbox::modules::logs::info(m_entity_world, "Renderer Module: Initialized.");
    }

    RendererModule::~RendererModule() {
        if (m_render_system.is_valid()) {
            m_render_system.destruct();
        }
    }

    void RendererModule::render_frame() {
        m_render_query.each([](flecs::entity entity, spectre_transform_2d_t& transform) {
            flecs::world w = entity.world();
            sandbox::modules::logs::info(w, "Renderer: Rendering entity '{}'", entity.path().c_str());
            rlPushMatrix();
            rlTranslatef(transform.position[0], transform.position[1], 0.0f);
            rlRotatef(transform.rotation * RAD2DEG, 0.0f, 0.0f, 1.0f);
            rlScalef(transform.scale[0], transform.scale[1], 1.0f);

            bool drawn = false;
            flecs::entity current = entity;
            
            while (current.is_valid() && !drawn) {
                current.get([&](const spectre_polygon_renderer_t& polygon) {
                    sandbox::modules::logs::info(w, "Renderer: Drawing Polygon for entity '{}' at ({}, {})", entity.path().c_str(), transform.position[0], transform.position[1]);
                    draw_polygon(&polygon);
                    drawn = true;
                });
                if (drawn) break;

                current.get([&](const spectre_rectangle_renderer_t& rect) {
                    sandbox::modules::logs::info(w, "Renderer: Drawing Rectangle for entity '{}' at ({}, {})", entity.path().c_str(), transform.position[0], transform.position[1]);
                    draw_rectangle(&rect);
                    drawn = true;
                });
                if (drawn) break;

                current.get([&](const spectre_line_renderer_t& line) {
                    draw_line(&line);
                    drawn = true;
                });
                if (drawn) break;

                current.get([&](const spectre_custom_polygon_renderer_t& cpoly) {
                    draw_custom_polygon(&cpoly);
                    drawn = true;
                });
                if (drawn) break;

                // Move up the prefab hierarchy
                current = current.target(flecs::IsA);
            }

            if (!drawn) {
                sandbox::modules::logs::warn(w, "Renderer: Entity '{}' has Transform2D but no valid renderable component!", entity.path().c_str());
            }

            rlPopMatrix();
        });
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