#pragma once

#include <spectre/abi/resources_service.h>
#include <flecs/addons/cpp/flecs.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

namespace spectre::module {

    /**
     * @brief Describes a registered resource type (e.g. "texture", "audio").
     *        Holds the factory functions needed to load and free one instance of
     *        that type from raw bytes.
     */
    struct ResourceType {
        std::string              type_name;
        spectre_load_resource_fn load_fn;
        spectre_free_resource_fn free_fn;
    };

    /**
     * @brief Represents a single loaded (or pending) resource file.
     *        Multiple callers may hold references via ref_count.
     */
    struct ResourceInstance {
        ResourceType* resource_type = nullptr;
        std::string   virtual_path;
        void*         data          = nullptr;
        uint32_t      ref_count     = 0;
    };

    /**
     * @brief Manages resource types and their loaded instances.
     *
     * Usage:
     *  1. Register a type with create_resource().
     *  2. Declare an instance path with create_resource_instance().
     *  3. Get a uint64_t handle via handle_for_path().
     *  4. Load the actual data with load().
     *  5. Release with free_instance() — data is freed when ref_count hits 0.
     */
    class ResourceModule {
    public:
        explicit ResourceModule(flecs::world& ecs);
        ~ResourceModule();

        // ── Type Registration ─────────────────────────────────────────────────
        void create_resource(const char* type_name, spectre_load_resource_fn load_fn,
                             spectre_free_resource_fn free_fn);

        // ── Instance Management ───────────────────────────────────────────────
        void     create_resource_instance(const char* type_name, const char* virtual_path);
        uint64_t handle_for_path(const char* virtual_path);
        void*    load(uint64_t handle);
        void     free_instance(uint64_t handle);

    private:
        flecs::world m_entity_world;

        std::unordered_map<std::string,  ResourceType>     m_resource_types;
        std::unordered_map<uint64_t,     ResourceInstance>  m_instances;
        std::unordered_map<std::string,  uint64_t>          m_path_to_handle;
    };

} // namespace spectre::module
