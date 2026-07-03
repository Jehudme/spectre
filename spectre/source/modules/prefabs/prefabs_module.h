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
        explicit PrefabsModule(flecs::world& ecs); // create prefab from app://state/prefabs.json then make the entity an prefab
        ~PrefabsModule() = default;

        void register_component_factory(std::string_view component_name, spectre_component_factory_fn factory_fn);
        bool has_component_factory(std::string_view component_name) const;

        flecs::entity create_prefab(const char* name, const sandbox::properties& props); // create the entity from configuration and make it an prefab, if the prefab already exist with this name but is empty kus add to it, if it exist and is not empty logs error and ignore it
        flecs::entity create_entity(const sandbox::properties& props); // create the entity from configuration if it have an an prefab configuration, if it already exist apply to it if not create an empty prefab to his name and later i should automaticly sync when this prefab get "created" 

    private:

        flecs::world m_world;
        std::unordered_map<std::string, spectre_component_factory_fn> m_component_factories;
    };

} // namespace spectre::module
