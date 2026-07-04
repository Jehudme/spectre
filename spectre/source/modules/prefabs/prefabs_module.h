#pragma once

#include <spectre/abi/prefabs_service.h>
#include <sandbox/sdk/properties.hpp>
#include <flecs.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace spectre::module {

    class PrefabsModule {
    public:
        explicit PrefabsModule(flecs::world& ecs);
        ~PrefabsModule() = default;

        static flecs::entity create_prefab(flecs::world& world, const char* name, const sandbox::properties& props);
        static flecs::entity create_entity(flecs::world& world, const sandbox::properties& props);

    private:
        static std::string ensure_global_name(const std::string& name);
        static void apply_component(flecs::world& world, flecs::entity entity, const std::string& component_name, const sandbox::properties& component_props);
        static void build_entity(flecs::world& world, flecs::entity entity, const sandbox::properties& props);

        flecs::world m_world;
    };

} // namespace spectre::module
