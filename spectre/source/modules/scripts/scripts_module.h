#pragma once
#include <string_view>
#include "flecs.h"
#include "sandbox/sdk/properties.hpp"
#include "spectre/components.h"

namespace spectre::modules {
    using script_argument_t = spectre_script_argument_t;
    using script_arguments_t = std::vector<spectre_script_argument_t>;

    using script_t = spectre_script_t;

    class script_module_t {
    public:
        explicit script_module_t(flecs::world& world);
        ~script_module_t();

        script_module_t(const script_module_t&) = delete;
        script_module_t& operator=(const script_module_t&) = delete;
        script_module_t(script_module_t&&) = delete;
        script_module_t& operator=(script_module_t&&) = delete;

        sandbox::properties serialize_script(flecs::entity script);
        flecs::entity deserialize_script(sandbox::properties props);

        void register_script(sandbox::properties props);
        bool has_script(std::string_view name) const;
        bool is_script(flecs::entity entity) const;
        flecs::entity find_script(std::string_view name);

        void include_code(std::string_view path);
        void execute_script(flecs::entity script_entity, script_arguments_t args);

    private:
        flecs::world m_world;
        flecs::entity m_scripts;
        flecs::entity m_script_serializer;
    };
}
