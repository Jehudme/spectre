#pragma once
#include "flecs/addons/cpp/flecs.hpp"
#include "sandbox/sdk/properties.hpp"

namespace spectre::modules {
    class scenes_module_t {
    public:
        explicit scenes_module_t(flecs::world& world);
        ~scenes_module_t();

        scenes_module_t(const scenes_module_t&) = delete;
        scenes_module_t& operator=(const scenes_module_t&) = delete;
        scenes_module_t(scenes_module_t&&) = delete;
        scenes_module_t& operator=(scenes_module_t&&) = delete;

        sandbox::properties serialize_state(flecs::entity state);
        flecs::entity deserialize_state(sandbox::properties props);

        sandbox::properties serialize_scene(flecs::entity scene);
        flecs::entity deserialize_scene(sandbox::properties props);

        void register_state(sandbox::properties props);
        void register_scene(sandbox::properties props);

        flecs::entity find_state(std::string_view name);
        flecs::entity find_scene(std::string_view name);

        bool has_state(std::string_view name) const;
        bool has_scene(std::string_view name) const;

        bool is_state(flecs::entity entity) const;
        bool is_scene(flecs::entity entity) const;

        flecs::entity find_current_state();
        flecs::query<> find_current_scenes();

        void push_state(flecs::entity state);
        void pop_state();

    private:
        flecs::world m_world;
        flecs::entity m_scenes_module;
        flecs::entity m_scene_serializer;
        flecs::entity m_state_serializer;
    };
}
