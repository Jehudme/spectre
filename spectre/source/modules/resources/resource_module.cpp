#include "resource_module.h"
#include <sandbox/sdk/logs.hpp>
#include <sandbox/sdk/filesystem.hpp>
#include <stdexcept>
#include <functional>

namespace spectre::module {

    // Simple hash function for path to uint64_t handle
    static uint64_t hash_path(const std::string& path) {
        std::hash<std::string> hasher;
        return hasher(path);
    }

    ResourceModule::ResourceModule(flecs::world& ecs) : m_entity_world(ecs) {
        sandbox::modules::logs::trace(m_entity_world, "Resources Module: Initializing...");
        sandbox::modules::logs::info(m_entity_world, "Resources Module: Initialized successfully.");
    }

    ResourceModule::~ResourceModule() {
        sandbox::modules::logs::trace(m_entity_world, "Resources Module: Shutting down...");
        for (auto& [handle, instance] : m_instances) {
            if (instance.instance && instance.resource && instance.resource->free_resource) {
                instance.resource->free_resource(instance.instance);
                instance.instance = nullptr;
            }
        }
    }

    void ResourceModule::create_resource(const char* type, spectre_load_resource_fn load_fn, spectre_free_resource_fn free_fn) {
        if (!type || !load_fn || !free_fn) {
            sandbox::modules::logs::error(m_entity_world, "Resources Module: Invalid arguments for create_resource");
            throw std::invalid_argument("Invalid arguments for create_resource");
        }
        
        std::string stype(type);
        if (m_resources.find(stype) != m_resources.end()) {
            sandbox::modules::logs::warn(m_entity_world, "Resources Module: Resource type '{}' already exists", stype);
            return;
        }

        m_resources[stype] = { stype, load_fn, free_fn };
        sandbox::modules::logs::info(m_entity_world, "Resources Module: Registered resource type '{}'", stype);
    }

    void ResourceModule::create_resource_instance(const char* type, const char* path) {
        if (!type || !path) {
            sandbox::modules::logs::error(m_entity_world, "Resources Module: Invalid arguments for create_resource_instance");
            throw std::invalid_argument("Invalid arguments for create_resource_instance");
        }

        std::string stype(type);
        std::string spath(path);

        auto res_it = m_resources.find(stype);
        if (res_it == m_resources.end()) {
            sandbox::modules::logs::error(m_entity_world, "Resources Module: Unknown resource type '{}'", stype);
            throw std::runtime_error("Unknown resource type");
        }

        uint64_t h = hash_path(spath);
        if (m_instances.find(h) != m_instances.end()) {
            sandbox::modules::logs::warn(m_entity_world, "Resources Module: Resource instance '{}' already exists", spath);
            return;
        }

        m_instances[h] = { &res_it->second, spath, nullptr, 0 };
        m_path_to_handle[spath] = h;
        
        sandbox::modules::logs::info(m_entity_world, "Resources Module: Created resource instance for '{}' (type: {})", spath, stype);
    }

    uint64_t ResourceModule::handle(const char* path) {
        if (!path) return 0;
        
        std::string spath(path);
        auto it = m_path_to_handle.find(spath);
        if (it != m_path_to_handle.end()) {
            return it->second;
        }
        
        sandbox::modules::logs::warn(m_entity_world, "Resources Module: Handle for path '{}' not found", spath);
        return 0;
    }

    void* ResourceModule::load(uint64_t h) {
        auto it = m_instances.find(h);
        if (it == m_instances.end()) {
            sandbox::modules::logs::error(m_entity_world, "Resources Module: Invalid handle {} for load", h);
            throw std::runtime_error("Invalid handle for load");
        }

        ResourceInstance& inst = it->second;
        if (inst.instance) {
            inst.ref_count++;
            return inst.instance;
        }

        sandbox::modules::logs::trace(m_entity_world, "Resources Module: Loading resource from '{}'", inst.path);
        
        try {
            std::vector<uint8_t> bytes = sandbox::modules::filesystem::read_all_bytes(m_entity_world, inst.path.c_str());
            
            spectre_resource_content_t content;
            content.data = bytes.data();
            content.size = bytes.size();
            
            inst.instance = inst.resource->load_resource(&content);
            if (!inst.instance) {
                sandbox::modules::logs::error(m_entity_world, "Resources Module: Failed to load resource instance from '{}'", inst.path);
                throw std::runtime_error("Failed to load resource instance");
            }
            
            inst.ref_count = 1;
            sandbox::modules::logs::info(m_entity_world, "Resources Module: Successfully loaded '{}'", inst.path);
            return inst.instance;
        } catch (const std::exception& e) {
            sandbox::modules::logs::error(m_entity_world, "Resources Module: Exception loading resource '{}': {}", inst.path, e.what());
            throw;
        }
    }

    void ResourceModule::free_instance(uint64_t h) {
        auto it = m_instances.find(h);
        if (it == m_instances.end()) {
            sandbox::modules::logs::error(m_entity_world, "Resources Module: Invalid handle {} for free_instance", h);
            throw std::runtime_error("Invalid handle for free_instance");
        }

        ResourceInstance& inst = it->second;
        if (inst.ref_count > 0) {
            inst.ref_count--;
            if (inst.ref_count == 0 && inst.instance) {
                sandbox::modules::logs::trace(m_entity_world, "Resources Module: Freeing resource '{}'", inst.path);
                inst.resource->free_resource(inst.instance);
                inst.instance = nullptr;
                sandbox::modules::logs::info(m_entity_world, "Resources Module: Successfully freed '{}'", inst.path);
            }
        }
    }
}

// C-ABI EXPORT
using spectre::module::ResourceModule;

static void api_create_resource(ecs_world_t* ecs, const char* type, spectre_load_resource_fn load_fn, spectre_free_resource_fn free_fn) {
    if (!ecs) return;
    flecs::world w(ecs);
    if (auto* mod = w.try_get_mut<ResourceModule>()) {
        mod->create_resource(type, load_fn, free_fn);
    }
}

static void api_create_resource_instance(ecs_world_t* ecs, const char* type, const char* path) {
    if (!ecs) return;
    flecs::world w(ecs);
    if (auto* mod = w.try_get_mut<ResourceModule>()) {
        mod->create_resource_instance(type, path);
    }
}

static uint64_t api_handle(ecs_world_t* ecs, const char* path) {
    if (!ecs) return 0;
    flecs::world w(ecs);
    if (auto* mod = w.try_get_mut<ResourceModule>()) {
        return mod->handle(path);
    }
    return 0;
}

static void* api_load(ecs_world_t* ecs, uint64_t handle) {
    if (!ecs) return nullptr;
    flecs::world w(ecs);
    if (auto* mod = w.try_get_mut<ResourceModule>()) {
        return mod->load(handle);
    }
    return nullptr;
}

static void api_free_instance(ecs_world_t* ecs, uint64_t handle) {
    if (!ecs) return;
    flecs::world w(ecs);
    if (auto* mod = w.try_get_mut<ResourceModule>()) {
        mod->free_instance(handle);
    }
}

static spectre_resources_api_t g_resources_api = {
    .create_resource = api_create_resource,
    .create_resource_instance = api_create_resource_instance,
    .handle = api_handle,
    .load = api_load,
    .free_instance = api_free_instance
};

SANDBOX_DEFINE_SERVICE(spectre_resources_service_t, spectre_resources_api_t, &g_resources_api)

namespace spectre::module {

    static sandbox_requirement_info_t resources_requirements[] = {
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
            .name = "filesystem",
            .architecture = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(ResourceModule, {
        .name = "resources",
        .description = "Resource Management Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_resources_service_t_info,
        .requirements = resources_requirements,
        .requirement_count = 2
    })
}
