#pragma once
#include <functional>
#include <string_view>

#include "flecs.h"
#include "sandbox/sdk/properties.hpp"
#include "sandbox/sdk/detail/bootstrapper.inl"
#include "spectre/api/resources_api.h"

namespace spectre::modules {
    using ResourceLoader = spectre_resource_loader_t;
    using Resource = spectre_resource_t;

    class resource_module_t {
    public:
        explicit resource_module_t(flecs::world& world);
        ~resource_module_t();

        resource_module_t(const resource_module_t&) = delete;
        resource_module_t& operator=(const resource_module_t&) = delete;
        resource_module_t(resource_module_t&&) = delete;
        resource_module_t& operator=(resource_module_t&&) = delete;

        flecs::entity deserialize_resource(sandbox::properties props);
        sandbox::properties serialize_resource(flecs::entity resourceEntity);

        void register_resource_loader(std::string_view type, ResourceLoader loader);
        void register_resource(sandbox::properties props);

        bool has_resource_loader(std::string_view type) const;
        bool has_resource(std::string_view name) const;
        bool is_resource(flecs::entity entity) const;

        flecs::entity find_resource_loader(std::string_view type);
        flecs::entity find_resource(std::string_view name);

        bool is_resource_loaded(flecs::entity resource) const;

        void load_resource(flecs::entity resourceEntity);
        void free_resource(flecs::entity resourceEntity);

    private:
        flecs::world m_world;
        flecs::entity m_resource_module;
        flecs::entity m_resource_serializer;
    };

} // namespace spectre::module