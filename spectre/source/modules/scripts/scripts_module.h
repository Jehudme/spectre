#pragma once
#include "flecs.h"
#include "sandbox/sdk/properties.hpp"
#include "spectre/spectre.h"
#include <lua.hpp>
#include <string_view>
#include <vector>
#include <unordered_set>
#include <string>

namespace spectre::modules {
using script_argument_t = spectre_script_argument_t;
using script_argument_type_t = spectre_script_argument_type_t;
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

    sandbox::properties serialize_scripts(flecs::entity entity);
    void deserialize_scripts(flecs::entity target, sandbox::properties props);

    bool is_script(flecs::entity entity) const;
    flecs::entity find_script(std::string_view function_name) const;

    bool has_script(std::string_view function_name, const std::vector<script_argument_type_t>&) const {
        return find_script(function_name).is_valid();
    }

    void include_code(std::string_view path);
    void eval_code(std::string_view source_code, std::string_view chunk_name);

    void execute_script(std::string_view function_name, script_arguments_t& args) {
        execute_script_with_target(flecs::entity::null(), find_script(function_name), args.data(), args.size());
    }

    void execute_on_create(flecs::entity entity);
    void execute_on_destroy(flecs::entity entity);
    void execute_on_update(flecs::entity entity);
    void execute_on_enter(flecs::entity entity);
    void execute_on_exit(flecs::entity entity);

  private:
    void init_ffi_if_needed();
    void execute_script_with_target(flecs::entity target, flecs::entity script_ent, spectre_script_argument_t* args,
                                    size_t arg_count);
    const char* intern_string(const std::string& str);

  private:
    flecs::world m_world;
    flecs::entity m_scripts_root;
    flecs::entity m_script_prefab;

    lua_State* m_lua;
    std::unordered_set<std::string> m_string_pool;
    bool m_ffi_initialized = false;
};
} // namespace spectre::modules
