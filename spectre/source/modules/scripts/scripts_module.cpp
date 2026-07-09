#include "scripts_module.h"
#include "spectre/services/scripts_service.h"
#include "sandbox/sdk/logs.hpp"
#include <lua.hpp>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>

extern "C" {
    int luaopen_spectre_api(lua_State* L);
}

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(script_module_t, {
        .name = "scripts",
        .description = "Scripts Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_scripts_service_t_info,
        .requirements = nullptr,
        .requirement_count = 0
    })

    script_module_t::script_module_t(flecs::world& world) : m_world(world) {
        sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "Initializing scripts module...");

        m_scripts_root = m_world.entity("::scripts");
        m_script_prefab = m_world.prefab("::scripts::prefab");

        m_lua = luaL_newstate();
        luaL_openlibs(m_lua);
        
        // Initialize SWIG bindings
        luaopen_spectre_api(m_lua);
        lua_setglobal(m_lua, "spectre_api");

        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "Scripts module initialized successfully.");
    }

    script_module_t::~script_module_t() {
        if (m_lua) {
            lua_close(m_lua);
        }
    }

    scripts_t script_module_t::parse_code(std::string_view code) {
        scripts_t scripts;
        std::string code_str(code);
        
        // Simple regex to extract function names and arguments.
        // e.g. function update(dt, entity)
        std::regex func_regex(R"(function\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\))");
        auto words_begin = std::sregex_iterator(code_str.begin(), code_str.end(), func_regex);
        auto words_end = std::sregex_iterator();

        for (std::sregex_iterator i = words_begin; i != words_end; ++i) {
            std::smatch match = *i;
            std::string func_name = match[1].str();
            std::string args_str = match[2].str();

            m_string_pool.push_back(func_name);
            const char* c_func_name = m_string_pool.back().c_str();

            std::vector<std::string> args;
            std::regex arg_regex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
            auto args_begin = std::sregex_iterator(args_str.begin(), args_str.end(), arg_regex);
            auto args_end = std::sregex_iterator();
            for (std::sregex_iterator j = args_begin; j != args_end; ++j) {
                args.push_back((*j).str());
            }

            size_t arg_count = args.size();
            
            std::vector<const char*> local_ptrs;
            std::vector<spectre_script_argument_type_t> local_types;
            
            for (const auto& arg : args) {
                m_string_pool.push_back(arg);
                local_ptrs.push_back(m_string_pool.back().c_str());
                local_types.push_back(SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL);
            }

            m_ptr_pool.push_back(std::move(local_ptrs));
            m_type_pool.push_back(std::move(local_types));

            spectre_script_t script{};
            script.function_name = c_func_name;
            script.argument_count = arg_count;
            script.arguments_name = arg_count > 0 ? m_ptr_pool.back().data() : nullptr;
            script.argument_types = arg_count > 0 ? m_type_pool.back().data() : nullptr;

            scripts.push_back(script);
        }
        return scripts;
    }

    void script_module_t::register_scripts(const scripts_t& scripts) {
        for (const auto& script : scripts) {
            std::string full_path = std::string("::scripts::") + script.function_name;
            if (!has_script(script.function_name, {})) {
                flecs::entity script_ent = m_world.entity(full_path.c_str())
                                                  .is_a(m_script_prefab)
                                                  .set<spectre_script_t>(script);
            } else {
                flecs::entity script_ent = find_script(script.function_name);
                script_ent.set<spectre_script_t>(script);
            }
        }
    }

    bool script_module_t::has_script(std::string_view function_name, const std::vector<script_argument_type_t>& args) const {
        if (function_name.empty()) return false;
        flecs::entity script_ent = m_scripts_root.lookup(std::string(function_name).c_str());
        if (!is_script(script_ent)) return false;
        
        return true;
    }

    bool script_module_t::is_script(flecs::entity entity) const {
        return entity.is_valid() && entity.has(flecs::IsA, m_script_prefab);
    }

    flecs::entity script_module_t::find_script(std::string_view function_name) {
        if (function_name.empty()) return flecs::entity::null();
        return m_scripts_root.lookup(std::string(function_name).c_str());
    }

    void script_module_t::include_code(std::string_view path) {
        std::string path_str(path);
        std::ifstream file(path_str);
        if (!file.is_open()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Failed to open script: {}", path);
            return;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string code = buffer.str();

        // Parse and register scripts
        scripts_t parsed_scripts = parse_code(code);
        register_scripts(parsed_scripts);

        // Execute code
        if (luaL_dostring(m_lua, code.c_str()) != LUA_OK) {
            const char* err = lua_tostring(m_lua, -1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Lua Error: {}", err);
            lua_pop(m_lua, 1);
        } else {
            sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "Successfully included script: {}", path);
        }
    }

    void script_module_t::execute_script(std::string_view function_name, script_arguments_t& args) {
        if (!has_script(function_name, {})) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Cannot execute unknown script: {}", function_name);
            return;
        }

        lua_getglobal(m_lua, std::string(function_name).c_str());
        if (!lua_isfunction(m_lua, -1)) {
            lua_pop(m_lua, 1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Lua global {} is not a function.", function_name);
            return;
        }

        for (const auto& arg : args) {
            switch (arg.type) {
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL:
                    lua_pushnil(m_lua);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN:
                    lua_pushboolean(m_lua, arg.value.boolean_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER:
                    lua_pushnumber(m_lua, arg.value.number_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER:
                    lua_pushinteger(m_lua, arg.value.integer_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING:
                    lua_pushstring(m_lua, arg.value.string_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE:
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA:
                default:
                    lua_pushnil(m_lua); // Not fully supported easily via plain values
                    break;
            }
        }

        if (lua_pcall(m_lua, args.size(), 0, 0) != LUA_OK) {
            const char* err = lua_tostring(m_lua, -1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Error executing {}: {}", function_name, err);
            lua_pop(m_lua, 1);
        }
    }
}
