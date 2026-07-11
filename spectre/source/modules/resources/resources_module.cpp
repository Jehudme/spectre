#include "resources_module.h"
#include "spectre/services/resources_service.h"
#include <iostream>
#include "sandbox/sdk/logs.hpp"
#include "../serializer/serializer_module.h"

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(resource_module_t, {
        .name = "resources",
        .description = "Resources Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_resources_service_t_info,
        .requirements = nullptr,
        .requirement_count = 0
    })

    static ecs_entity_t deserialize_resource_cb(ecs_world_t* world, sandbox_properties_handle_t props_handle) {
        flecs::world w(world);
        auto* mod = const_cast<resource_module_t*>(w.try_get_mut<resource_module_t>());
        if (mod) {
            sandbox::properties props(props_handle, false);
            return mod->deserialize_resource(props).id();
        }
        return 0;
    }

    static sandbox_properties_handle_t serialize_resource_cb(ecs_world_t* world, ecs_entity_t entity) {
        flecs::world w(world);
        auto* mod = const_cast<resource_module_t*>(w.try_get_mut<resource_module_t>());
        if (mod) {
            sandbox::properties props = mod->serialize_resource(flecs::entity(w, entity));
            sandbox_properties_handle_t handle = props.get_raw();
            props.release();
            return handle;
        }
        return {0};
    }

    resource_module_t::resource_module_t(flecs::world& world) : m_world(world) {
        m_resources_root = m_world.entity("::resources");
        m_resource_prefab = m_world.prefab("::resources::prefab").add<Resource>();
        
        m_world.entity("::resources::loaders");

        auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
        if (serializer_mod) {
            spectre_serializer_component ser_comp;
            ser_comp.deserialize = deserialize_resource_cb;
            ser_comp.serialize = serialize_resource_cb;
            serializer_mod->register_serializer("resources", ser_comp);
            m_resources_serializer = serializer_mod->find_serializer("resources");
        }
    }
    
    resource_module_t::~resource_module_t() = default;

    void resource_module_t::register_resource_loader(std::string_view type, ResourceLoader loader) {
        if (type.empty()) return;
        std::string path = "::resources::loaders::" + std::string(type);
        flecs::entity loader_ent = m_world.entity(path.c_str());
        if (loader_ent.has<ResourceLoader>()) {
            ::sandbox::modules::logs::warn(m_world, "Resource loader for type '{}' is being overridden.", type);
        }
        loader_ent.set<ResourceLoader>(loader);
    }

    void resource_module_t::register_resource(const sandbox::properties& props) {
        if (!props.is_valid()) return;
        
        auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
        if (!serializer_mod || !m_resources_serializer.is_valid()) {
            ::sandbox::modules::logs::error(m_world, "Serializer module or resources serializer not available");
            return;
        }

        std::vector<std::string> keys = props.keys("");
        for (const auto& key : keys) {
            sandbox::properties res_node = props.sub(key);
            if (!res_node.is_valid()) continue;
            
            flecs::entity res_ent = serializer_mod->deserialize_entity(m_resources_serializer, res_node);
            if (res_ent.is_valid()) {
                res_ent.set_name(key.c_str());
                res_ent.child_of(m_resources_root);
            }
        }
    }

    flecs::entity resource_module_t::deserialize_resource(const sandbox::properties& props) {
        if (!props.is_valid()) return flecs::entity::null();
        std::string type;
        if (!props.get<std::string>("type", type) && !props.get<std::string>("types", type)) {
            ::sandbox::modules::logs::error(m_world, "Resource missing 'type'");
            return flecs::entity::null();
        }
        std::string path;
        if (!props.get<std::string>("path", path)) {
            ::sandbox::modules::logs::error(m_world, "Resource missing 'path'");
            return flecs::entity::null();
        }
        
        flecs::entity loader_ent = find_resource_loader(type);
        if (!loader_ent.is_valid()) {
            ::sandbox::modules::logs::error(m_world, "Resource loader not found for type '{}'", type);
            return flecs::entity::null();
        }
        
        flecs::entity res_ent = m_world.entity()
            .is_a(m_resource_prefab)
            .add<spectre_use_loader_relation_t>(loader_ent);
            
        Resource res_comp;
        char* path_copy = new char[path.size() + 1];
        std::copy(path.begin(), path.end(), path_copy);
        path_copy[path.size()] = '\0';
        res_comp.path = path_copy;
        res_comp.instance = nullptr;
        res_ent.set<Resource>(res_comp);
        return res_ent;
    }

    sandbox::properties resource_module_t::serialize_resource(flecs::entity entity) {
        sandbox::properties props;
        
        if (!entity.is_valid() || !entity.has<Resource>()) {
            return props;
        }

        const auto* res = entity.try_get<Resource>();
        if (res && res->path) {
            props.set("path", std::string(res->path));
        }

        entity.each<spectre_use_loader_relation_t>([&](flecs::entity loader) {
            props.set("type", std::string(loader.name()));
        });

        return props;
    }

    bool resource_module_t::has_resource_loader(std::string_view type) const {
        if (type.empty()) return false;
        std::string path = "::resources::loaders::" + std::string(type);
        flecs::entity loader_ent = m_world.lookup(path.c_str());
        return loader_ent.is_valid() && loader_ent.has<ResourceLoader>();
    }

    bool resource_module_t::has_resource(std::string_view name) const {
        if (name.empty()) return false;
        std::string path = "::resources::" + std::string(name); // i really dont like that just create the resource root in the constructor and use it to store and get with minimal string manipulation
        flecs::entity res_ent = m_world.lookup(path.c_str());
        return is_resource(res_ent);
    }

    bool resource_module_t::is_resource(flecs::entity entity) const {
        return entity.is_valid() && entity.has(flecs::IsA, m_resource_prefab);
    }

    flecs::entity resource_module_t::find_resource_loader(std::string_view type) {
        if (type.empty()) return flecs::entity::null();
        std::string path = "::resources::loaders::" + std::string(type); // also just create the loader root in the constructor and use to store and get with minimal string manipulation
        flecs::entity loader_ent = m_world.lookup(path.c_str());
        if (loader_ent.is_valid() && loader_ent.has<ResourceLoader>()) {
            return loader_ent;
        }
        return flecs::entity::null();
    }

    flecs::entity resource_module_t::find_resource(std::string_view name) {
        if (name.empty()) return flecs::entity::null();
        std::string path = "::resources::" + std::string(name); // same here, fixe everywhere
        flecs::entity res_ent = m_world.lookup(path.c_str());
        if (is_resource(res_ent)) return res_ent;
        return flecs::entity::null();
    }

    bool resource_module_t::is_resource_loaded(flecs::entity resource) const {
        if (!is_resource(resource)) return false;
        return resource.has<spectre_resource_flag_t>();
    }

    void resource_module_t::load_resource(flecs::entity resourceEntity) {
        if (!is_resource(resourceEntity)) return;
        if (is_resource_loaded(resourceEntity)) return;
        
        flecs::entity loader_ent = resourceEntity.target<spectre_use_loader_relation_t>();
        if (!loader_ent.is_valid()) {
            ::sandbox::modules::logs::error(m_world, "Resource {} has no loader relation", resourceEntity.name().c_str());
            return;
        }
        
        const ResourceLoader* loader = &loader_ent.get<ResourceLoader>();
        if (!loader || !loader->load_fn) {
            ::sandbox::modules::logs::error(m_world, "Loader for resource {} is invalid", resourceEntity.name().c_str());
            return;
        }
        
        Resource* res = resourceEntity.try_get_mut<Resource>();
        if (res) {
            loader->load_fn(m_world.c_ptr(), res);
            resourceEntity.add<spectre_resource_flag_t>();
        }
    }

    void resource_module_t::free_resource(flecs::entity resourceEntity) {
        if (!is_resource(resourceEntity)) return;
        if (!is_resource_loaded(resourceEntity)) return;
        
        flecs::entity loader_ent = resourceEntity.target<spectre_use_loader_relation_t>();
        if (!loader_ent.is_valid()) return;
        
        const ResourceLoader* loader = &loader_ent.get<ResourceLoader>();
        if (!loader || !loader->free_fn) return;
        
        Resource* res = resourceEntity.try_get_mut<Resource>();
        if (res) {
            loader->free_fn(m_world.c_ptr(), res);
            resourceEntity.remove<spectre_resource_flag_t>();
        }
    }

    void* resource_module_t::get_resource(flecs::entity resourceEntity) {
        if (!is_resource(resourceEntity)) return nullptr;
        if (!is_resource_loaded(resourceEntity)) {
            load_resource(resourceEntity);
        }
        const Resource* res = &resourceEntity.get<Resource>();
        if (res) return res->instance;
        return nullptr;
    }
}
