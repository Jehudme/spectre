#pragma once
#include <cstdint>
#include <ctime>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include <spectre/abi/resources_service.h>
#include "flecs/addons/cpp/flecs.hpp"

namespace spectre::module {
    struct Resource {
        std::string type;
        spectre_load_resource_fn load_resource;
        spectre_free_resource_fn free_resource;
    };

    struct ResourceInstance {
        Resource* resource;

        std::string path;
        void *instance = nullptr;
        uint32_t ref_count = 0;
    };

    class ResourceModule {
    public:
        ResourceModule(flecs::world& ecs);
        ~ResourceModule();

        void create_resource(const char* type, spectre_load_resource_fn load_fn, spectre_free_resource_fn free_fn);
        void create_resource_instance(const char* type, const char* path);

        uint64_t handle(const char* path);

        void* load(uint64_t handle);
        void free_instance(uint64_t handle);
        
    private:
        flecs::world m_entity_world;
        std::unordered_map<std::string, Resource> m_resources;
        std::unordered_map<uint64_t, ResourceInstance> m_instances;
        std::unordered_map<std::string, uint64_t> m_path_to_handle;
    };
}
