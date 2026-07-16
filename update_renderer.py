import re

filepath = "/home/jehud/CLionProjects/spectre/spectre/source/modules/renderer/renderer_module.cpp"

with open(filepath, 'r') as f:
    content = f.read()

# 1. Add #include <rlgl.h> if missing
if '<rlgl.h>' not in content:
    content = content.replace('#include <raylib.h>', '#include <raylib.h>\n#include <rlgl.h>')

# 2. Add circle register and serializer functions
circle_funcs = """
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
"""

if 'register_circle_comp' not in content:
    # Insert right before serialize_polygon_renderable
    content = content.replace('    static sandbox_properties_handle_t serialize_polygon_renderable', circle_funcs + '\n    static sandbox_properties_handle_t serialize_polygon_renderable')

# 3. Add to registration block in renderer_module_t constructor
reg_block = """
        register_circle_comp(m_world.c_ptr());
        spectre_serializer_component circle_serializer = {deserialize_circle_renderable, serialize_circle_renderable};
        spectre::modules::serializer::register_serializer(m_world, "spectre_circle_renderable_t", &circle_serializer);
"""
if 'register_circle_comp(m_world.c_ptr())' not in content:
    content = content.replace('        register_polygon_comp(m_world.c_ptr());', reg_block + '\n        register_polygon_comp(m_world.c_ptr());')

# 4. Refactor `render` method
render_func = """    void renderer_module_t::render(flecs::entity entity_to_render) {
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
    }"""

# regex replace the entire render method
pattern = r'    void renderer_module_t::render\(flecs::entity entity_to_render\) \{.*?(?=    flecs::query<> renderer_module_t::find_renderable\(\) const \{)'
content = re.sub(pattern, render_func + '\n\n', content, flags=re.DOTALL)

with open(filepath, 'w') as f:
    f.write(content)
