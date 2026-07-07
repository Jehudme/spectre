#pragma once
#include <functional>
#include <unordered_map>

#include "flecs.h"
#include "sandbox/sdk/properties.hpp"
#include "../../../include/spectre/api/serializer_api.h"

namespace spectre::modules {

    using serializer_t = spectre_serializer_t;
    class serializer_module {
    public:
        explicit serializer_module(flecs::world& world);
        ~serializer_module();

        serializer_module(const serializer_module&) = delete;
        serializer_module& operator=(const serializer_module&) = delete;
        serializer_module(serializer_module&&) = delete;
        serializer_module& operator=(serializer_module&&) = delete;

        void register_serializer(std::string_view type, const serializer_t& serializer);
        bool has_serializer(std::string_view type) const;
        flecs::entity find_serializer(std::string_view type) const;

        sandbox::properties serialize_entity(flecs::entity serializer, flecs::entity entity);
        flecs::entity deserialize_entity(flecs::entity serializer, const sandbox::properties& props);

    private:
        flecs::entity m_serializer;
        flecs::world m_world;
    };

} // namespace spectre::module