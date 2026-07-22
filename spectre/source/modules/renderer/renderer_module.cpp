#include "renderer_module.h"
#include "spectre/components/renderer_component.h"
#include "spectre/sdk/scenes.hpp"
#include "spectre/services/renderer_service.h"

#include "sandbox/sdk/logs.hpp"
#include "spectre/sdk/components.hpp"
#include "spectre/sdk/serializer.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

#include "spectre/sdk/renderer.hpp"
#include "spectre/sdk/resources.hpp"
#include "sandbox/sdk/filesystem.hpp"
#include <raylib.h>
#include <rlgl.h>

namespace spectre::modules {

static sandbox_requirement_info_t renderer_requirements[] = {
    {.kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
     .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
     .name = "logs",
     .architecture = "sandbox",
     .version_major = 1,
     .version_minor = 0,
     .version_patch = -1},
    {.kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
     .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
     .name = "resources",
     .architecture = "spectre",
     .version_major = 1,
     .version_minor = 0,
     .version_patch = -1}
};

SANDBOX_DECLARE_MODULE(renderer_module_t, {.name = "renderer",
                                           .description = "Renderer Module",
                                           .architecture = "spectre",
                                           .version_major = 1,
                                           .version_minor = 0,
                                           .version_patch = 0,
                                           .service = &spectre_renderer_service_t_info,
                                           .requirements = renderer_requirements,
                                           .requirement_count = 2})

static void deserialize_renderer_cb(ecs_world_t* world, ecs_entity_t entity,
                                    sandbox_properties_handle_t properties_handle) {
    spectre::modules::renderer::deserialize_renderer(flecs::world(world), entity, properties_handle);
}

static sandbox_properties_handle_t serialize_renderer_cb(ecs_world_t* world, ecs_entity_t entity_id) {
    return spectre::modules::renderer::serialize_renderer(flecs::world(world), entity_id);
}

static ecs_entity_t register_renderable_comp(ecs_world_t* world) {
    return flecs::world(world).component<spectre_renderable_t>("Renderable").member<char>("dummy").id();
}
static ecs_entity_t register_rectangle_comp(ecs_world_t* world) {
    auto w = flecs::world(world);
    if (!w.lookup("spectre_color_t").is_valid()) {
        w.component<spectre_color_t>("Color").member<float>("r").member<float>("g").member<float>("b").member<float>("a");
    }
    return w.component<spectre_rectange_renderable_t>("RectangleRenderable")
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
        w.component<spectre_color_t>("Color").member<float>("r").member<float>("g").member<float>("b").member<float>("a");
    }
    return w.component<spectre_polygone_renderable_t>("PolygoneRenderable")
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
        w.component<spectre_color_t>("Color").member<float>("r").member<float>("g").member<float>("b").member<float>("a");
    }
    return w.component<spectre_custom_polygone_renderable_t>("CustomPolygoneRenderable")
        .member<uint32_t>("vertex_count")
        .member<spectre_color_t>("fill_color")
        .member<spectre_color_t>("outline_color")
        .member<float>("outline_thickness")
        .id();
}
static ecs_entity_t register_line_comp(ecs_world_t* world) {
    auto w = flecs::world(world);
    if (!w.lookup("spectre_color_t").is_valid()) {
        w.component<spectre_color_t>("Color").member<float>("r").member<float>("g").member<float>("b").member<float>("a");
    }
    return w.component<spectre_ligne_renderable_t>("LigneRenderable")
        .member<double>("position_x1")
        .member<double>("position_y1")
        .member<double>("position_x2")
        .member<double>("position_y2")
        .member<spectre_color_t>("color")
        .member<float>("thickness")
        .id();
}
static ecs_entity_t register_texture_comp(ecs_world_t* world) {
    auto w = flecs::world(world);
    return w.component<spectre_texture_renderable_t>("TextureRenderable")
        .member<float>("width")
        .member<float>("height")
        .member<float>("source_x")
        .member<float>("source_y")
        .member<float>("source_width")
        .member<float>("source_height")
        .member<spectre_color_t>("tint")
        .member<bool>("flip_x")
        .member<bool>("flip_y")
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
            if (auto f = props.get<float>(key))
                return *f;
            if (auto d = props.get<double>(key))
                return static_cast<float>(*d);
            if (auto i = props.get<int>(key))
                return static_cast<float>(*i);
            return 255.0f;
        };
        color.r = get_val("r");
        color.g = get_val("g");
        color.b = get_val("b");
        color.a = get_val("a");
    }
    std::cout << "deserialize_color parsed: r=" << color.r << ", g=" << color.g << ", b=" << color.b
              << ", a=" << color.a << std::endl;
    return color;
}

// Serializers
static sandbox_properties_handle_t serialize_rectangle_renderable(ecs_world_t* world, ecs_entity_t entity) {
    if (!world || !entity)
        return {0};
    flecs::world flecs_world(world);
    const auto* comp = flecs_world.entity(entity).try_get<spectre_rectange_renderable_t>();
    if (!comp)
        return {0};
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
    if (!world)
        return;
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
        w.component<spectre_color_t>("Color").member<float>("r").member<float>("g").member<float>("b").member<float>("a");
    }
    return w.component<spectre_circle_renderable_t>("CircleRenderable")
        .member<float>("radius")
        .member<spectre_color_t>("fill_color")
        .member<spectre_color_t>("outline_color")
        .member<float>("outline_thickness")
        .id();
}

static sandbox_properties_handle_t serialize_circle_renderable(ecs_world_t* world, ecs_entity_t entity) {
    if (!world || !entity)
        return {0};
    flecs::world flecs_world(world);
    const auto* comp = flecs_world.entity(entity).try_get<spectre_circle_renderable_t>();
    if (!comp)
        return {0};
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
    if (!world)
        return;
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
    if (!world || !entity)
        return {0};
    flecs::world flecs_world(world);
    const auto* comp = flecs_world.entity(entity).try_get<spectre_polygone_renderable_t>();
    if (!comp)
        return {0};
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
    if (!world)
        return;
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
    if (!world || !entity)
        return {0};
    flecs::world flecs_world(world);
    const auto* comp = flecs_world.entity(entity).try_get<spectre_ligne_renderable_t>();
    if (!comp)
        return {0};
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
    if (!world)
        return;
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

static sandbox_properties_handle_t serialize_texture_renderable(ecs_world_t* world, ecs_entity_t entity) {
    if (!world || !entity)
        return {0};
    flecs::world flecs_world(world);
    const auto* comp = flecs_world.entity(entity).try_get<spectre_texture_renderable_t>();
    if (!comp)
        return {0};
    sandbox::properties props;
    props.set("width", comp->width);
    props.set("height", comp->height);
    props.set("source_x", comp->source_x);
    props.set("source_y", comp->source_y);
    props.set("source_width", comp->source_width);
    props.set("source_height", comp->source_height);
    props.merge("tint", serialize_color(comp->tint));
    props.set("flip_x", comp->flip_x);
    props.set("flip_y", comp->flip_y);
    sandbox_properties_handle_t handle = props.get_raw();
    props.release();
    return handle;
}
static void deserialize_texture_renderable(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t handle) {
    if (!world)
        return;
    sandbox::properties props(handle, false);
    flecs::world flecs_world(world);
    flecs::entity e(flecs_world, entity);
    spectre_texture_renderable_t comp = {};
    comp.width = props.get<float>("width").value_or(0.0f);
    comp.height = props.get<float>("height").value_or(0.0f);
    comp.source_x = props.get<float>("source_x").value_or(0.0f);
    comp.source_y = props.get<float>("source_y").value_or(0.0f);
    comp.source_width = props.get<float>("source_width").value_or(0.0f);
    comp.source_height = props.get<float>("source_height").value_or(0.0f);
    comp.tint = deserialize_color(props.sub("tint"));
    comp.flip_x = props.get<bool>("flip_x").value_or(false);
    comp.flip_y = props.get<bool>("flip_y").value_or(false);
    
    std::string name;
    if (props.get<std::string>("name", name)) {
        ecs_entity_t resource_entity = spectre::modules::resources::find_resource(flecs_world, name.c_str());
        if (resource_entity) {
            e.add<spectre_use_resource_relation_t>(resource_entity);
        } else {
            sandbox::modules::logs::error(flecs_world, "[Renderer Module] Texture resource '{}' not found", name.c_str());
        }
    }
    e.set<spectre_texture_renderable_t>(comp);
}

static ecs_entity_t register_text_comp(ecs_world_t* world) {
    auto w = flecs::world(world);
    if (!w.lookup("spectre_color_t").is_valid()) {
        w.component<spectre_color_t>("Color").member<float>("r").member<float>("g").member<float>("b").member<float>("a");
    }
    return w.component<spectre_text_renderable_t>("TextRenderable")
        .member<float>("font_size")
        .member<float>("spacing")
        .member<spectre_color_t>("tint")
        .member<bool>("bold")
        .member<bool>("italic")
        .id();
}

static sandbox_properties_handle_t serialize_text_renderable(ecs_world_t* world, ecs_entity_t entity) {
    if (!world || !entity)
        return {0};
    flecs::world flecs_world(world);
    const auto* comp = flecs_world.entity(entity).try_get<spectre_text_renderable_t>();
    if (!comp)
        return {0};
    sandbox::properties props;
    if (comp->content) props.set("content", std::string(comp->content));
    props.set("font_size", comp->font_size);
    props.set("spacing", comp->spacing);
    props.merge("tint", serialize_color(comp->tint));
    props.set("bold", comp->bold);
    props.set("italic", comp->italic);
    sandbox_properties_handle_t handle = props.get_raw();
    props.release();
    return handle;
}

static void deserialize_text_renderable(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t handle) {
    if (!world) return;
    sandbox::properties props(handle, false);
    flecs::world flecs_world(world);
    flecs::entity e(flecs_world, entity);
    spectre_text_renderable_t comp = {};
    
    sandbox::modules::logs::info(flecs_world, "[Renderer Module] Deserializing TextRenderable for entity {}", e.name().c_str());
    
    std::string content;
    if (props.get<std::string>("content", content)) {
        comp.content = strdup(content.c_str());
        sandbox::modules::logs::info(flecs_world, "[Renderer Module] Parsed text content: {}", content.c_str());
    } else {
        comp.content = nullptr;
    }
    
    comp.font_size = props.get<float>("font_size").value_or(20.0f);
    comp.spacing = props.get<float>("spacing").value_or(1.0f);
    comp.tint = deserialize_color(props.sub("tint"));
    comp.bold = props.get<bool>("bold").value_or(false);
    comp.italic = props.get<bool>("italic").value_or(false);
    
    std::string name;
    if (props.get<std::string>("name", name)) {
        ecs_entity_t resource_entity = spectre::modules::resources::find_resource(flecs_world, name.c_str());
        if (resource_entity) {
            e.add<spectre_use_resource_relation_t>(resource_entity);
            sandbox::modules::logs::info(flecs_world, "[Renderer Module] Found font resource: {}", name.c_str());
        } else {
            sandbox::modules::logs::error(flecs_world, "[Renderer Module] Font resource '{}' not found", name.c_str());
        }
    }
    e.set<spectre_text_renderable_t>(comp);
}

struct spectre_renderer_update_marker_t {
    char dummy;
};

static sandbox_properties_handle_t serialize_empty(ecs_world_t*, ecs_entity_t) {
    return {0};
}
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

static ecs_entity_t register_transform2d_comp(ecs_world_t* world) {
    return flecs::world(world).component<spectre_2D_transform_component_t>("Transform2D")
        .member<float>("position_x")
        .member<float>("position_y")
        .member<float>("position_z")
        .member<float>("scale_x")
        .member<float>("scale_y")
        .member<float>("origin_x")
        .member<float>("origin_y")
        .member<float>("rotation")
        .id();
}

static sandbox_properties_handle_t serialize_2D_transform_component(ecs_world_t* world, ecs_entity_t entity) {
    if (!world || !entity)
        return {0};
    flecs::world flecs_world(world);
    const auto* comp = flecs_world.entity(entity).try_get<spectre_2D_transform_component_t>();
    if (!comp)
        return {0};
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
static void deserialize_2D_transform_component(ecs_world_t* world, ecs_entity_t entity,
                                               sandbox_properties_handle_t handle) {
    if (!world)
        return;
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

static void texture_load_fn(ecs_world_t* world, spectre_resource_component_t* resource) {
    if (!world || !resource || !resource->path) return;
    flecs::world w(world);
    try {
        std::vector<uint8_t> data = sandbox::modules::filesystem::read_all_bytes(w, resource->path);
        if (data.empty()) return;
        
        // Find extension
        std::string path_str(resource->path);
        std::string ext = ".png";
        auto pos = path_str.find_last_of('.');
        if (pos != std::string::npos) {
            ext = path_str.substr(pos);
        }
        
        Image img = LoadImageFromMemory(ext.c_str(), data.data(), static_cast<int>(data.size()));
        if (img.data != nullptr) {
            Texture2D* tex = new Texture2D;
            *tex = LoadTextureFromImage(img);
            UnloadImage(img);

            sandbox::properties configs(resource->properties_handle, false);
            if (configs.is_valid()) {
                std::string filtering;
                if (configs.get<std::string>("filtering", filtering)) {
                    if (filtering == "point") SetTextureFilter(*tex, TEXTURE_FILTER_POINT);
                    else if (filtering == "bilinear") SetTextureFilter(*tex, TEXTURE_FILTER_BILINEAR);
                    else if (filtering == "trilinear") SetTextureFilter(*tex, TEXTURE_FILTER_TRILINEAR);
                    sandbox::modules::logs::info(w, "[Renderer Module] Applied filtering {} to texture {}", filtering.c_str(), resource->path);
                }
                std::string wrap_mode;
                if (configs.get<std::string>("wrap_mode", wrap_mode)) {
                    if (wrap_mode == "repeat") SetTextureWrap(*tex, TEXTURE_WRAP_REPEAT);
                    else if (wrap_mode == "clamp") SetTextureWrap(*tex, TEXTURE_WRAP_CLAMP);
                    else if (wrap_mode == "mirror_repeat") SetTextureWrap(*tex, TEXTURE_WRAP_MIRROR_REPEAT);
                    else if (wrap_mode == "mirror_clamp") SetTextureWrap(*tex, TEXTURE_WRAP_MIRROR_CLAMP);
                    sandbox::modules::logs::info(w, "[Renderer Module] Applied wrap mode {} to texture {}", wrap_mode.c_str(), resource->path);
                }
            }

            resource->instance = tex;
        }
    } catch (const std::exception& e) {
        sandbox::modules::logs::error(w, "[Renderer Module] Failed to load texture {}: {}", resource->path, e.what());
    }
}

static void texture_free_fn(ecs_world_t* world, spectre_resource_component_t* resource) {
    if (!resource || !resource->instance) return;
    Texture2D* tex = static_cast<Texture2D*>(resource->instance);
    UnloadTexture(*tex);
    delete tex;
    resource->instance = nullptr;
}

renderer_module_t::renderer_module_t(flecs::world& world) : m_world(world) {
    sandbox::modules::logs::trace(m_world, "[Renderer Module] Initializing...");

    spectre_resource_loader_component_t tex_loader = {};
    tex_loader.load_fn = texture_load_fn;
    tex_loader.free_fn = texture_free_fn;
    spectre::modules::resources::register_resource_loader(m_world, "texture", tex_loader);

    auto font_load_fn = [](ecs_world_t* world, spectre_resource_component_t* resource) {
        if (!world || !resource || !resource->path) return;
        flecs::world w(world);
        try {
            std::vector<uint8_t> data = sandbox::modules::filesystem::read_all_bytes(w, resource->path);
            if (data.empty()) return;
            
            std::string path_str(resource->path);
            std::string ext = ".ttf";
            auto pos = path_str.find_last_of('.');
            if (pos != std::string::npos) ext = path_str.substr(pos);
            
            sandbox::properties configs(resource->properties_handle, false);
            int font_size = 32;
            if (configs.is_valid()) configs.get<int>("font_size", font_size);
            
            Font* font = new Font;
            *font = LoadFontFromMemory(ext.c_str(), data.data(), static_cast<int>(data.size()), font_size, nullptr, 0);
            
            if (configs.is_valid()) {
                std::string filtering;
                if (configs.get<std::string>("filtering", filtering)) {
                    if (filtering == "point") SetTextureFilter(font->texture, TEXTURE_FILTER_POINT);
                    else if (filtering == "bilinear") SetTextureFilter(font->texture, TEXTURE_FILTER_BILINEAR);
                    else if (filtering == "trilinear") SetTextureFilter(font->texture, TEXTURE_FILTER_TRILINEAR);
                    sandbox::modules::logs::info(w, "[Renderer Module] Applied filtering {} to font {}", filtering.c_str(), resource->path);
                }
            }
            resource->instance = font;
        } catch (const std::exception& e) {
            sandbox::modules::logs::error(w, "[Renderer Module] Failed to load font {}: {}", resource->path, e.what());
        }
    };
    
    auto font_free_fn = [](ecs_world_t* world, spectre_resource_component_t* resource) {
        if (!resource || !resource->instance) return;
        Font* font = static_cast<Font*>(resource->instance);
        UnloadFont(*font);
        delete font;
        resource->instance = nullptr;
    };
    
    spectre_resource_loader_component_t font_loader = {};
    font_loader.load_fn = font_load_fn;
    font_loader.free_fn = font_free_fn;
    spectre::modules::resources::register_resource_loader(m_world, "font", font_loader);

    spectre_serializer_component serializer_callbacks = {};
    serializer_callbacks.deserialize = deserialize_renderer_cb;
    serializer_callbacks.serialize = serialize_renderer_cb;

    spectre::modules::serializer::register_serializer(m_world, "renderer", &serializer_callbacks);

    // Register Renderer Components via SDK
    spectre_serializer_component empty_serializer = {deserialize_empty, serialize_empty};
    spectre_serializer_component renderable_serializer = {deserialize_renderable, serialize_renderable};

    spectre_serializer_component rect_serializer = {deserialize_rectangle_renderable, serialize_rectangle_renderable};
    spectre_serializer_component poly_serializer = {deserialize_polygon_renderable, serialize_polygon_renderable};
    spectre_serializer_component cpoly_serializer = {deserialize_empty, serialize_empty};
    spectre_serializer_component line_serializer = {deserialize_line_renderable, serialize_line_renderable};
    spectre_serializer_component transform_serializer = {deserialize_2D_transform_component,
                                                         serialize_2D_transform_component};
    spectre_serializer_component circle_serializer = {deserialize_circle_renderable, serialize_circle_renderable};
    spectre_serializer_component texture_serializer = {deserialize_texture_renderable, serialize_texture_renderable};
    spectre_serializer_component text_serializer = {deserialize_text_renderable, serialize_text_renderable};

    spectre::modules::components::register_component(m_world, "Renderable", register_renderable_comp, renderable_serializer);
    spectre::modules::components::register_component(m_world, "Transform2D", register_transform2d_comp, transform_serializer);
    spectre::modules::components::register_component(m_world, "RectangleRenderable", register_rectangle_comp, rect_serializer);
    spectre::modules::components::register_component(m_world, "CircleRenderable", register_circle_comp, circle_serializer);
    spectre::modules::components::register_component(m_world, "PolygoneRenderable", register_polygon_comp, poly_serializer);
    spectre::modules::components::register_component(m_world, "CustomPolygoneRenderable", register_custom_polygon_comp, cpoly_serializer);
    spectre::modules::components::register_component(m_world, "LigneRenderable", register_line_comp, line_serializer);
    spectre::modules::components::register_component(m_world, "TextureRenderable", register_texture_comp, texture_serializer);
    spectre::modules::components::register_component(m_world, "TextRenderable", register_text_comp, text_serializer);

    flecs::entity on_renderer_phase = m_world.entity("on_renderer").add(flecs::Phase).depends_on(flecs::OnUpdate);

    sandbox::modules::logs::info(m_world, "[Renderer Module] Initialized successfully.");
}

renderer_module_t::~renderer_module_t() = default;

void renderer_module_t::deserialize_renderer(flecs::entity target, const sandbox::properties& properties) {
    sandbox::modules::logs::debug(const_cast<flecs::world&>(m_world),
                                  "[Renderer Module] Deserializing renderer entity.");
}

sandbox::properties renderer_module_t::serialize_renderer(flecs::entity renderer_entity) {
    return sandbox::properties{};
}

void renderer_module_t::register_renderer(const sandbox::properties& properties) {
    m_renderer = m_world.entity("::renderer").add<spectre_renderer_update_marker_t>();

    m_world.system<spectre_renderer_update_marker_t>("RendererSystem")
        .kind(flecs::OnUpdate)
        .each([this](flecs::entity entity, spectre_renderer_update_marker_t&) { this->render_frame(); });

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
        renderable_query.each([this](flecs::entity entity, spectre_renderable_t& renderable) { this->render(entity); });
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
            if (parent_entity.has<spectre_state_use_scene_relation_t>() &&
                parent_entity.target(flecs::ChildOf) == current_state) {
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

    std::sort(entities_to_render.begin(), entities_to_render.end(),
              [](const renderable_entity_t& entity_a, const renderable_entity_t& entity_b) {
                  return entity_a.layer < entity_b.layer;
              });

    for (const auto& renderable_entity : entities_to_render) {
        this->render(renderable_entity.entity);
    }

    static int frame_count = 0;
    if (frame_count++ == 0) {}

    EndDrawing();
}

void renderer_module_t::render(flecs::entity entity_to_render) {
    sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Renderer Module] Rendering entity {}", entity_to_render.name().c_str());
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
        return Color{(unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b, (unsigned char)c.a};
    };

    if (entity_to_render.has<spectre_rectange_renderable_t>()) {
        const auto* rect = entity_to_render.try_get<spectre_rectange_renderable_t>();
        if (rect) {
            DrawRectangle(0, 0, (int)rect->width, (int)rect->height, to_raylib_color(rect->fill_color));
            if (rect->outline_thickness > 0) {
                DrawRectangleLinesEx(Rectangle{0, 0, rect->width, rect->height}, rect->outline_thickness,
                                     to_raylib_color(rect->outline_color));
            }
        }
    }

    if (entity_to_render.has<spectre_circle_renderable_t>()) {
        const auto* circle = entity_to_render.try_get<spectre_circle_renderable_t>();
        if (circle) {
            DrawCircleV(Vector2{0.0f, 0.0f}, circle->radius, to_raylib_color(circle->fill_color));
            if (circle->outline_thickness > 0) {
                // raylib DrawCircleLines lacks thickness, but we can draw a ring
                DrawRing(Vector2{0.0f, 0.0f}, circle->radius - circle->outline_thickness, circle->radius, 0, 360, 36,
                         to_raylib_color(circle->outline_color));
            }
        }
    }

    if (entity_to_render.has<spectre_polygone_renderable_t>()) {
        const auto* poly = entity_to_render.try_get<spectre_polygone_renderable_t>();
        if (poly) {
            DrawPoly(Vector2{0.0f, 0.0f}, poly->point_count, poly->radius, 0.0f, to_raylib_color(poly->fill_color));
            if (poly->outline_thickness > 0) {
                DrawPolyLinesEx(Vector2{0.0f, 0.0f}, poly->point_count, poly->radius, 0.0f, poly->outline_thickness,
                                to_raylib_color(poly->outline_color));
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

    if (entity_to_render.has<spectre_texture_renderable_t>()) {
        const auto* tex_comp = entity_to_render.try_get<spectre_texture_renderable_t>();
        flecs::entity resource_entity = entity_to_render.target<spectre_use_resource_relation_t>();
        
        if (resource_entity.is_valid()) {
            if (spectre::modules::resources::is_resource_loaded(m_world, resource_entity.id())) {
                void* instance = spectre::modules::resources::get_resource(m_world, resource_entity.id());
                if (instance && tex_comp) {
                    Texture2D* tex = static_cast<Texture2D*>(instance);
                    Rectangle source = { 
                        tex_comp->source_x, 
                        tex_comp->source_y, 
                        tex_comp->source_width > 0 ? tex_comp->source_width : (float)tex->width, 
                        tex_comp->source_height > 0 ? tex_comp->source_height : (float)tex->height 
                    };
                    
                    if (tex_comp->flip_x) source.width = -source.width;
                    if (tex_comp->flip_y) source.height = -source.height;

                    Rectangle dest = { 0.0f, 0.0f, tex_comp->width > 0 ? tex_comp->width : (float)tex->width, 
                                       tex_comp->height > 0 ? tex_comp->height : (float)tex->height };
                    Vector2 origin = { 0.0f, 0.0f };
                    
                    // If tint is completely 0 (default zero-init), treat it as white
                    spectre_color_t actual_tint = tex_comp->tint;
                    if (actual_tint.r == 0 && actual_tint.g == 0 && actual_tint.b == 0 && actual_tint.a == 0) {
                        actual_tint = {255.0f, 255.0f, 255.0f, 255.0f};
                    }
                    
                    DrawTexturePro(*tex, source, dest, origin, 0.0f, to_raylib_color(actual_tint));
                } else {
                    sandbox::modules::logs::trace(m_world, "[Renderer Module] Instance or tex_comp is null");
                }
            } else {
                spectre::modules::resources::load_resource(m_world, resource_entity.id());
            }
        } else {
            sandbox::modules::logs::trace(m_world, "[Renderer Module] TextureRenderable found but resource_entity is invalid on entity {}", entity_to_render.name().c_str());
        }
    }

    if (entity_to_render.has<spectre_text_renderable_t>()) {
        const auto* text_comp = entity_to_render.try_get<spectre_text_renderable_t>();
        flecs::entity resource_entity = entity_to_render.target<spectre_use_resource_relation_t>();
        
        if (resource_entity.is_valid()) {
            if (spectre::modules::resources::is_resource_loaded(m_world, resource_entity.id())) {
                void* instance = spectre::modules::resources::get_resource(m_world, resource_entity.id());
                if (instance && text_comp && text_comp->content) {
                    Font* font = static_cast<Font*>(instance);
                    
                    spectre_color_t actual_tint = text_comp->tint;
                    if (actual_tint.r == 0 && actual_tint.g == 0 && actual_tint.b == 0 && actual_tint.a == 0) {
                        actual_tint = {255.0f, 255.0f, 255.0f, 255.0f};
                    }
                    
                    Vector2 origin = { 0.0f, 0.0f };
                    
                    sandbox::modules::logs::info(m_world, "[Renderer Module] Drawing text: {}", text_comp->content);
                    
                    if (text_comp->bold) {
                        DrawTextEx(*font, text_comp->content, Vector2{1.0f, 0.0f}, text_comp->font_size, text_comp->spacing, to_raylib_color(actual_tint));
                    }
                    DrawTextEx(*font, text_comp->content, origin, text_comp->font_size, text_comp->spacing, to_raylib_color(actual_tint));
                }
            } else {
                spectre::modules::resources::load_resource(m_world, resource_entity.id());
            }
        } else {
            sandbox::modules::logs::trace(m_world, "[Renderer Module] TextRenderable found but resource_entity is invalid on entity {}", entity_to_render.name().c_str());
        }
    }

    rlPopMatrix();
}

flecs::query<> renderer_module_t::find_renderable() const {
    return m_world.query<spectre_renderable_t>();
}

} // namespace spectre::modules
