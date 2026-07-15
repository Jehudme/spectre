#pragma once
#include <functional>
#include <unordered_map>

#include "flecs.h"
#include "sandbox/sdk/properties.hpp"
#include "spectre/spectre.h"
namespace spectre::modules {

    using serializer_t = spectre_serializer_component;
    class serializer_module {
    public:
        explicit serializer_module(flecs::world& world);
        ~serializer_module();

        serializer_module(const serializer_module&) = delete;
        serializer_module& operator=(const serializer_module&) = delete;
        serializer_module(serializer_module&&) = delete;
        serializer_module& operator=(serializer_module&&) = delete;

        // We use the updated service/API structure here to register serializers across modules
        void register_serializer(std::string_view serializer_type, const serializer_t& serializer_component);
        bool has_serializer(std::string_view type) const;
        bool is_serializer(flecs::entity entity) const;
        flecs::entity find_serializer(std::string_view type) const;

        sandbox::properties serialize_entity(flecs::entity serializer_entity, flecs::entity target_entity);
        void deserialize_entity(flecs::entity serializer_entity, flecs::entity target_entity, const sandbox::properties& props);

    private:
        flecs::entity m_serializer;
        flecs::entity m_serializable_prefab;
        flecs::world m_world;
    };

} // namespace spectre::module