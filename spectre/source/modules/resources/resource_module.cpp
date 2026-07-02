#include "resource_module.h"
#include <utilities/hashing.h>
#include <sandbox/sdk/logs.hpp>
#include <sandbox/sdk/filesystem.hpp>
#include <stdexcept>

namespace spectre::module {

// ─────────────────────────────────────────────────────────────────────────────
// Helpers (declarations at top, implementations at bottom of file)
// ─────────────────────────────────────────────────────────────────────────────

    inline bool is_valid_type_name(const char* type_name);
    inline bool is_valid_path(const char* path);

// ─────────────────────────────────────────────────────────────────────────────
// MODULE LIFECYCLE
// ─────────────────────────────────────────────────────────────────────────────

    ResourceModule::ResourceModule(flecs::world& ecs) : m_entity_world(ecs) {
        sandbox::modules::logs::trace(m_entity_world, "Resources Module: Initialized.");
    }

    ResourceModule::~ResourceModule() {
        sandbox::modules::logs::trace(m_entity_world, "Resources Module: Shutting down — freeing all loaded instances...");

        for (auto& [handle, instance] : m_instances) {
            if (instance.data && instance.resource_type && instance.resource_type->free_fn) {
                sandbox::modules::logs::trace(m_entity_world,
                    "Resources Module: Freeing '{}' on shutdown.", instance.virtual_path);
                instance.resource_type->free_fn(instance.data);
                instance.data = nullptr;
            }
        }
    }

// ─────────────────────────────────────────────────────────────────────────────
// TYPE REGISTRATION
// ─────────────────────────────────────────────────────────────────────────────

    void ResourceModule::create_resource(const char* type_name,
                                         spectre_load_resource_fn load_fn,
                                         spectre_free_resource_fn free_fn) {
        if (!is_valid_type_name(type_name)) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: create_resource() called with null or empty type_name.");
            throw std::invalid_argument("type_name must not be null or empty");
        }
        if (!load_fn) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: create_resource('{}') — load_fn is null.", type_name);
            throw std::invalid_argument("load_fn must not be null");
        }
        if (!free_fn) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: create_resource('{}') — free_fn is null.", type_name);
            throw std::invalid_argument("free_fn must not be null");
        }

        const std::string type_key(type_name);
        if (m_resource_types.count(type_key)) {
            sandbox::modules::logs::warn(m_entity_world,
                "Resources Module: Resource type '{}' is already registered — ignoring duplicate.", type_key);
            return;
        }

        m_resource_types[type_key] = ResourceType{ type_key, load_fn, free_fn };
        sandbox::modules::logs::info(m_entity_world,
            "Resources Module: Registered resource type '{}'.", type_key);
    }

// ─────────────────────────────────────────────────────────────────────────────
// INSTANCE MANAGEMENT
// ─────────────────────────────────────────────────────────────────────────────

    void ResourceModule::create_resource_instance(const char* type_name, const char* virtual_path) {
        if (!is_valid_type_name(type_name)) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: create_resource_instance() — type_name is null or empty.");
            throw std::invalid_argument("type_name must not be null or empty");
        }
        if (!is_valid_path(virtual_path)) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: create_resource_instance('{}') — virtual_path is null or empty.", type_name);
            throw std::invalid_argument("virtual_path must not be null or empty");
        }

        const std::string type_key(type_name);
        const std::string path_key(virtual_path);

        auto type_it = m_resource_types.find(type_key);
        if (type_it == m_resource_types.end()) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: create_resource_instance('{}') — unknown resource type '{}'.",
                path_key, type_key);
            throw std::runtime_error("Unknown resource type: " + type_key);
        }

        const uint64_t path_handle = utilities::hash_path(path_key);
        if (m_instances.count(path_handle)) {
            sandbox::modules::logs::warn(m_entity_world,
                "Resources Module: Instance for '{}' already exists — ignoring duplicate.", path_key);
            return;
        }

        m_instances[path_handle] = ResourceInstance{ &type_it->second, path_key, nullptr, 0 };
        m_path_to_handle[path_key] = path_handle;

        sandbox::modules::logs::info(m_entity_world,
            "Resources Module: Registered instance '{}' (type: {}).", path_key, type_key);
    }

    uint64_t ResourceModule::handle_for_path(const char* virtual_path) {
        if (!is_valid_path(virtual_path)) return 0;

        const std::string path_key(virtual_path);
        auto handle_it = m_path_to_handle.find(path_key);
        if (handle_it == m_path_to_handle.end()) {
            sandbox::modules::logs::warn(m_entity_world,
                "Resources Module: No handle found for path '{}' — was it registered?", path_key);
            return 0;
        }
        return handle_it->second;
    }

    void* ResourceModule::load(uint64_t path_handle) {
        auto instance_it = m_instances.find(path_handle);
        if (instance_it == m_instances.end()) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: load() called with unknown handle {}.", path_handle);
            throw std::runtime_error("Unknown resource handle");
        }

        ResourceInstance& instance = instance_it->second;

        // Already loaded — just bump the ref count.
        if (instance.data) {
            instance.ref_count++;
            sandbox::modules::logs::trace(m_entity_world,
                "Resources Module: '{}' already loaded (ref_count: {}).",
                instance.virtual_path, instance.ref_count);
            return instance.data;
        }

        sandbox::modules::logs::trace(m_entity_world,
            "Resources Module: Loading '{}' from disk...", instance.virtual_path);

        std::vector<uint8_t> raw_bytes =
            sandbox::modules::filesystem::read_all_bytes(m_entity_world, instance.virtual_path.c_str());

        if (raw_bytes.empty()) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: File '{}' is empty or could not be read.", instance.virtual_path);
            throw std::runtime_error("Empty or unreadable resource file: " + instance.virtual_path);
        }

        spectre_resource_content_t content{ raw_bytes.data(), raw_bytes.size() };
        instance.data = instance.resource_type->load_fn(&content);

        if (!instance.data) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: load_fn returned null for '{}'.", instance.virtual_path);
            throw std::runtime_error("Resource load_fn failed for: " + instance.virtual_path);
        }

        instance.ref_count = 1;
        sandbox::modules::logs::info(m_entity_world,
            "Resources Module: Loaded '{}' successfully.", instance.virtual_path);
        return instance.data;
    }

    void ResourceModule::free_instance(uint64_t path_handle) {
        auto instance_it = m_instances.find(path_handle);
        if (instance_it == m_instances.end()) {
            sandbox::modules::logs::error(m_entity_world,
                "Resources Module: free_instance() called with unknown handle {}.", path_handle);
            throw std::runtime_error("Unknown resource handle");
        }

        ResourceInstance& instance = instance_it->second;
        if (instance.ref_count == 0) {
            sandbox::modules::logs::warn(m_entity_world,
                "Resources Module: free_instance() called on '{}' with ref_count already 0 — ignoring.",
                instance.virtual_path);
            return;
        }

        instance.ref_count--;
        if (instance.ref_count == 0 && instance.data) {
            sandbox::modules::logs::trace(m_entity_world,
                "Resources Module: Releasing '{}' (ref_count hit 0).", instance.virtual_path);
            instance.resource_type->free_fn(instance.data);
            instance.data = nullptr;
            sandbox::modules::logs::info(m_entity_world,
                "Resources Module: Freed '{}'.", instance.virtual_path);
        }
    }

// ─────────────────────────────────────────────────────────────────────────────
// HELPER IMPLEMENTATIONS
// ─────────────────────────────────────────────────────────────────────────────

    inline bool is_valid_type_name(const char* type_name) {
        return type_name != nullptr && type_name[0] != '\0';
    }

    inline bool is_valid_path(const char* path) {
        return path != nullptr && path[0] != '\0';
    }

} // namespace spectre::module


// ─────────────────────────────────────────────────────────────────────────────
// C-ABI BRIDGE
// ─────────────────────────────────────────────────────────────────────────────

using spectre::module::ResourceModule;

static ResourceModule* get_module(ecs_world_t* ecs) {
    if (!ecs) return nullptr;
    return flecs::world(ecs).try_get_mut<ResourceModule>();
}

static void abi_create_resource(ecs_world_t* ecs, const char* type_name,
                                spectre_load_resource_fn load_fn, spectre_free_resource_fn free_fn) {
    if (auto* module = get_module(ecs)) module->create_resource(type_name, load_fn, free_fn);
}

static void abi_create_resource_instance(ecs_world_t* ecs, const char* type_name, const char* virtual_path) {
    if (auto* module = get_module(ecs)) module->create_resource_instance(type_name, virtual_path);
}

static uint64_t abi_handle(ecs_world_t* ecs, const char* virtual_path) {
    if (auto* module = get_module(ecs)) return module->handle_for_path(virtual_path);
    return 0;
}

static void* abi_load(ecs_world_t* ecs, uint64_t path_handle) {
    if (auto* module = get_module(ecs)) return module->load(path_handle);
    return nullptr;
}

static void abi_free_instance(ecs_world_t* ecs, uint64_t path_handle) {
    if (auto* module = get_module(ecs)) module->free_instance(path_handle);
}

static spectre_resources_api_t g_resources_api = {
    .create_resource          = abi_create_resource,
    .create_resource_instance = abi_create_resource_instance,
    .handle                   = abi_handle,
    .load                     = abi_load,
    .free_instance            = abi_free_instance,
};

SANDBOX_DEFINE_SERVICE(spectre_resources_service_t, spectre_resources_api_t, &g_resources_api)

// ─────────────────────────────────────────────────────────────────────────────
// MODULE REGISTRATION
// ─────────────────────────────────────────────────────────────────────────────

namespace spectre::module {

    static sandbox_requirement_info_t resources_requirements[] = {
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
            .name          = "filesystem",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(ResourceModule, {
        .name          = "resources",
        .description   = "Resource type registry and ref-counted instance loader.",
        .architecture  = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service       = &spectre_resources_service_t_info,
        .requirements  = resources_requirements,
        .requirement_count = 2
    })

} // namespace spectre::module
