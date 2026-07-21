#include "resources_module.h"
#include "spectre/sdk/serializer.hpp"
#include "spectre/services/resources_service.h"

#include "sandbox/sdk/logs.hpp"
#include <iostream>

#include "spectre/sdk/resources.hpp"

namespace spectre::modules {

static void deserialize_resource_cb(ecs_world_t* world, ecs_entity_t entity,
                                    sandbox_properties_handle_t properties_handle);
static sandbox_properties_handle_t serialize_resource_cb(ecs_world_t* world, ecs_entity_t entity_id);

// Serializers for resource component
static sandbox_properties_handle_t serialize_resource_comp_cb(ecs_world_t* world, ecs_entity_t entity) {
    if (!world || !entity)
        return {0};
    flecs::world flecs_world(world);
    const auto* comp = flecs_world.entity(entity).try_get<spectre_resource_component_t>();
    if (!comp || !comp->path)
        return {0};
    sandbox::properties props;
    props.set("path", std::string(comp->path));
    sandbox_properties_handle_t handle = props.get_raw();
    props.release();
    return handle;
}
static void deserialize_resource_comp_cb(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t handle) {
    if (!world)
        return;
    sandbox::properties props(handle, false);
    if (!props.is_valid())
        return;
    flecs::world flecs_world(world);
    flecs::entity e(flecs_world, entity);
    spectre_resource_component_t comp = {};
    std::string path_str = props.get<std::string>("path").value_or("");
    if (!path_str.empty()) {
        char* path_copy = new char[path_str.size() + 1];
        std::copy(path_str.begin(), path_str.end(), path_copy);
        path_copy[path_str.size()] = '\0';
        comp.path = path_copy;
    } else {
        comp.path = nullptr;
    }
    comp.instance = nullptr;
    e.set<spectre_resource_component_t>(comp);
}

// Component Registration Callbacks
static ecs_entity_t register_resource_component(ecs_world_t* world) {
    return flecs::world(world).component<spectre_resource_component_t>().id();
}
static ecs_entity_t register_resource_loader_component(ecs_world_t* world) {
    return flecs::world(world).component<spectre_resource_loader_component_t>().id();
}
static ecs_entity_t register_use_loader_relation(ecs_world_t* world) {
    return flecs::world(world).component<spectre_use_loader_relation_t>().member<char>("dummy").id();
}
static ecs_entity_t register_resource_flag(ecs_world_t* world) {
    return flecs::world(world).component<spectre_resource_flag_t>().member<char>("dummy").id();
}

static sandbox_requirement_info_t resources_requirements[] = {{.kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
                                                               .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
                                                               .name = "logs",
                                                               .architecture = "sandbox",
                                                               .version_major = 1,
                                                               .version_minor = 0,
                                                               .version_patch = -1}};

SANDBOX_DECLARE_MODULE(resource_module_t, {.name = "resources",
                                           .description = "Resources Module",
                                           .architecture = "spectre",
                                           .version_major = 1,
                                           .version_minor = 0,
                                           .version_patch = 0,
                                           .service = &spectre_resources_service_t_info,
                                           .requirements = resources_requirements,
                                           .requirement_count = 1})

resource_module_t::resource_module_t(flecs::world& world) : m_world(world) {
    sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Resources Module] Initializing...");

    // Register components and relations
    auto deserialize_empty = [](ecs_world_t*, ecs_entity_t, sandbox_properties_handle_t) {};
    auto serialize_empty = [](ecs_world_t*, ecs_entity_t) -> sandbox_properties_handle_t { return {0}; };
    spectre_serializer_component empty_serializer = {deserialize_empty, serialize_empty};
    spectre_serializer_component resource_comp_serializer = {deserialize_resource_comp_cb, serialize_resource_comp_cb};

    register_resource_component(m_world.c_ptr());
    spectre::modules::serializer::register_serializer(m_world, "spectre_resource_component_t", &resource_comp_serializer);

    register_resource_loader_component(m_world.c_ptr());
    spectre::modules::serializer::register_serializer(m_world, "spectre_resource_loader_component_t", &empty_serializer);

    register_use_loader_relation(m_world.c_ptr());
    spectre::modules::serializer::register_serializer(m_world, "spectre_use_loader_relation_t", &empty_serializer);

    register_resource_flag(m_world.c_ptr());
    spectre::modules::serializer::register_serializer(m_world, "spectre_resource_flag_t", &empty_serializer);

    // Create roots
    m_resources_root = m_world.entity("::resources");
    m_loaders_root = m_world.entity("::loaders");
    m_resource_prefab = m_world.prefab("::resources::prefab").add<Resource>();

    // Register serializer
    spectre_serializer_component resource_serializer = {};
    resource_serializer.deserialize = deserialize_resource_cb;
    resource_serializer.serialize = serialize_resource_cb;
    spectre::modules::serializer::register_serializer(m_world, "resources", &resource_serializer);

    m_resources_serializer = m_world.entity(spectre::modules::serializer::find_serializer(m_world, "resources"));

    sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Resources Module] Initialized successfully.");
}

resource_module_t::~resource_module_t() = default;

static void deserialize_resource_cb(ecs_world_t* world, ecs_entity_t entity,
                                    sandbox_properties_handle_t properties_handle) {
    auto* module = flecs::world(world).try_get_mut<resource_module_t>();
    if (module)
        module->deserialize_resource(flecs::world(world).entity(entity), sandbox::properties(properties_handle, false));
}

static sandbox_properties_handle_t serialize_resource_cb(ecs_world_t* world, ecs_entity_t entity_id) {
    return spectre::modules::resources::serialize_resource(flecs::world(world), entity_id);
}

void resource_module_t::register_resource_loader(std::string_view type, ResourceLoader loader) {
    if (type.empty())
        return;
    flecs::entity loader_entity = m_loaders_root.lookup(std::string(type).c_str());
    if (!loader_entity.is_valid()) {
        loader_entity = m_world.entity(std::string(type).c_str()).child_of(m_loaders_root);
    }
    if (loader_entity.has<ResourceLoader>()) {
        sandbox::modules::logs::warn(m_world, "[Resources Module] Resource loader for type '{}' is being overridden.",
                                     type);
    }
    loader_entity.set<ResourceLoader>(loader);
}

void resource_module_t::register_resource(const sandbox::properties& properties) {
    if (!properties.is_valid())
        return;

    if (!m_resources_serializer.is_valid()) {
        sandbox::modules::logs::error(m_world, "[Resources Module] Resources serializer not available.");
        return;
    }

    std::vector<std::string> keys = properties.keys("");
    for (const auto& key : keys) {
        sandbox::properties resource_node = properties.sub(key);
        if (!resource_node.is_valid())
            continue;

        sandbox_properties_handle_t handle = resource_node.get_raw();
        flecs::entity resource_entity = m_world.entity();
        resource_entity.set_name(key.c_str());
        resource_entity.child_of(m_resources_root);
        spectre::modules::serializer::deserialize_entity(m_world, m_resources_serializer.id(), resource_entity.id(),
                                                         handle);
    }
}

void resource_module_t::deserialize_resource(flecs::entity resource_entity, const sandbox::properties& properties) {
    if (!properties.is_valid() || !resource_entity.is_valid())
        return;
    std::string type;
    if (!properties.get<std::string>("type", type) && !properties.get<std::string>("types", type)) {
        sandbox::modules::logs::error(m_world, "[Resources Module] Resource missing 'type'.");
        return;
    }
    std::string path;
    if (!properties.get<std::string>("path", path)) {
        sandbox::modules::logs::error(m_world, "[Resources Module] Resource missing 'path'.");
        return;
    }

    flecs::entity loader_entity = find_resource_loader(type);
    if (!loader_entity.is_valid()) {
        sandbox::modules::logs::error(m_world, "[Resources Module] Resource loader not found for type '{}'.", type);
        return;
    }

    resource_entity.is_a(m_resource_prefab).add<spectre_use_loader_relation_t>(loader_entity);

    Resource resource_component;
    char* path_copy = new char[path.size() + 1];
    std::copy(path.begin(), path.end(), path_copy);
    path_copy[path.size()] = '\0';
    resource_component.path = path_copy;
    resource_component.instance = nullptr;
    resource_entity.set<Resource>(resource_component);
}

sandbox::properties resource_module_t::serialize_resource(flecs::entity entity_to_serialize) {
    sandbox::properties result_properties;

    if (!entity_to_serialize.is_valid() || !entity_to_serialize.has<Resource>()) {
        return result_properties;
    }

    const auto* resource_component = entity_to_serialize.try_get<Resource>();
    if (resource_component && resource_component->path) {
        result_properties.set("path", std::string(resource_component->path));
    }

    entity_to_serialize.each<spectre_use_loader_relation_t>(
        [&](flecs::entity loader) { result_properties.set("type", std::string(loader.name())); });

    return result_properties;
}

bool resource_module_t::has_resource_loader(std::string_view type) const {
    if (type.empty())
        return false;
    flecs::entity loader_entity = m_loaders_root.lookup(std::string(type).c_str());
    return loader_entity.is_valid() && loader_entity.has<ResourceLoader>();
}

bool resource_module_t::has_resource(std::string_view name) const {
    if (name.empty())
        return false;
    flecs::entity resource_entity = m_resources_root.lookup(std::string(name).c_str());
    return is_resource(resource_entity);
}

bool resource_module_t::is_resource(flecs::entity entity_to_check) const {
    return entity_to_check.is_valid() && entity_to_check.has(flecs::IsA, m_resource_prefab);
}

flecs::entity resource_module_t::find_resource_loader(std::string_view type) {
    if (type.empty())
        return flecs::entity::null();
    flecs::entity loader_entity = m_loaders_root.lookup(std::string(type).c_str());
    if (loader_entity.is_valid() && loader_entity.has<ResourceLoader>()) {
        return loader_entity;
    }
    return flecs::entity::null();
}

flecs::entity resource_module_t::find_resource(std::string_view name) {
    if (name.empty())
        return flecs::entity::null();
    flecs::entity resource_entity = m_resources_root.lookup(std::string(name).c_str());
    if (is_resource(resource_entity))
        return resource_entity;
    return flecs::entity::null();
}

bool resource_module_t::is_resource_loaded(flecs::entity resource_entity) const {
    if (!is_resource(resource_entity))
        return false;
    return resource_entity.has<spectre_resource_flag_t>();
}

void resource_module_t::load_resource(flecs::entity resource_entity) {
    if (!is_resource(resource_entity))
        return;
    if (is_resource_loaded(resource_entity))
        return;

    flecs::entity loader_entity = resource_entity.target<spectre_use_loader_relation_t>();
    if (!loader_entity.is_valid()) {
        sandbox::modules::logs::error(m_world, "[Resources Module] Resource '{}' has no loader relation.",
                                      resource_entity.name().c_str());
        return;
    }

    const ResourceLoader* loader = &loader_entity.get<ResourceLoader>();
    if (!loader || !loader->load_fn) {
        sandbox::modules::logs::error(m_world, "[Resources Module] Loader for resource '{}' is invalid.",
                                      resource_entity.name().c_str());
        return;
    }

    Resource* resource_component = resource_entity.try_get_mut<Resource>();
    if (resource_component) {
        loader->load_fn(m_world.c_ptr(), resource_component);
        resource_entity.add<spectre_resource_flag_t>();
    }
}

void resource_module_t::free_resource(flecs::entity resource_entity) {
    if (!is_resource(resource_entity))
        return;
    if (!is_resource_loaded(resource_entity))
        return;

    flecs::entity loader_entity = resource_entity.target<spectre_use_loader_relation_t>();
    if (!loader_entity.is_valid())
        return;

    const ResourceLoader* loader = &loader_entity.get<ResourceLoader>();
    if (!loader || !loader->free_fn)
        return;

    Resource* resource_component = resource_entity.try_get_mut<Resource>();
    if (resource_component) {
        loader->free_fn(m_world.c_ptr(), resource_component);
        resource_entity.remove<spectre_resource_flag_t>();
    }
}

void* resource_module_t::get_resource(flecs::entity resource_entity) {
    if (!is_resource(resource_entity))
        return nullptr;
    if (!is_resource_loaded(resource_entity)) {
        load_resource(resource_entity);
    }
    const Resource* resource_component = &resource_entity.get<Resource>();
    if (resource_component)
        return resource_component->instance;
    return nullptr;
}
} // namespace spectre::modules
