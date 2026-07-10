#include "scripts_module.h"
#include "spectre/services/scripts_service.h"
#include "sandbox/sdk/logs.hpp"
#include "../serializer/serializer_module.h"
#include <lua.hpp>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>

extern "C" {
    int luaopen_spectre_api(lua_State* L);
}

namespace spectre::modules {

    static ecs_entity_t deserialize_script_args_cb(ecs_world_t* world, sandbox_properties_handle_t props_handle);
    static sandbox_properties_handle_t serialize_script_args_cb(ecs_world_t* world, ecs_entity_t entity);


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
        
        auto* serializer_mod = const_cast<serializer_module*>(m_world.try_get_mut<serializer_module>());
        if (serializer_mod) {
            spectre_serializer_component ser_comp;
            ser_comp.deserialize = deserialize_script_args_cb;
            ser_comp.serialize = serialize_script_args_cb;
            serializer_mod->register_serializer("scripts", ser_comp);
        }


        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "Scripts module initialized successfully.");
    }

    
    static ecs_entity_t deserialize_script_args_cb(ecs_world_t* world, sandbox_properties_handle_t props_handle) {
        flecs::world w(world);
        auto* scripts_mod = const_cast<script_module_t*>(w.try_get_mut<script_module_t>());
        if (scripts_mod) {
            sandbox::properties props(props_handle, false);
            return scripts_mod->deserialize_scripts(std::move(props)).id();
        }
        return 0;
    }

    flecs::entity script_module_t::deserialize_scripts(sandbox::properties props) {
        if (!props.is_valid()) return flecs::entity::null();
        flecs::entity temp = m_world.entity();

        auto deserialize_relation = [&](const char* rel_name, auto relation_type) {
            if (!props.has(rel_name)) return;
            sandbox::properties rel_props = props.sub(rel_name);
            std::vector<std::string> indices = rel_props.keys("");
            for (const auto& idx : indices) {
                sandbox::properties item = rel_props.sub(idx);
                std::string func_name;
                if (!item.get<std::string>("function", func_name)) continue;

                flecs::entity scripts_root = m_world.lookup("::scripts");
                if (!scripts_root.is_valid()) continue;
                flecs::entity script_ent = scripts_root.lookup(func_name.c_str());
                if (!script_ent.is_valid()) continue;
                
                const spectre_script_t* script = script_ent.try_get<spectre_script_t>();
                if (!script) continue;

                size_t arg_count = script->argument_count;
                spectre_script_argument_t* args = nullptr;
                if (arg_count > 0) {
                    args = new spectre_script_argument_t[arg_count];
                    sandbox::properties args_props = item.sub("arguments");
                    for (size_t i = 0; i < arg_count; ++i) {
                        const char* arg_name = script->arguments_name[i];
                        spectre_script_argument_type_t arg_type = script->argument_types[i];
                        args[i].type = arg_type;
                        
                        switch (arg_type) {
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER: {
                                double val = 0.0;
                                args_props.get<double>(arg_name, val);
                                args[i].value.number_value = val;
                                break;
                            }
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER: {
                                long long val = 0;
                                args_props.get<long long>(arg_name, val);
                                args[i].value.integer_value = val;
                                break;
                            }
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN: {
                                bool val = false;
                                args_props.get<bool>(arg_name, val);
                                args[i].value.boolean_value = val;
                                break;
                            }
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING: {
                                std::string val;
                                args_props.get<std::string>(arg_name, val);
                                args[i].value.string_value = strdup(val.c_str());
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }

                using RelType = decltype(relation_type);
                RelType rel_data;
                rel_data.arguments = args;
                rel_data.argument_count = arg_count;
                
                temp.set<RelType>(script_ent, rel_data);
            }
        };

        deserialize_relation("on_create", spectre_use_script_on_create_relation_t{});
        deserialize_relation("on_destroy", spectre_use_script_on_destroy_relation_t{});
        deserialize_relation("on_update", spectre_use_script_on_update_relation_t{});
        deserialize_relation("on_render", spectre_use_script_on_render_relation_t{});
        deserialize_relation("on_enter", spectre_use_script_on_enter_relation_t{});
        deserialize_relation("on_exit", spectre_use_script_on_exit_relation_t{});

        return temp;
    }

    static sandbox_properties_handle_t serialize_script_args_cb(ecs_world_t* world, ecs_entity_t entity) {
        flecs::world w(world);
        auto* scripts_mod = const_cast<script_module_t*>(w.try_get_mut<script_module_t>());
        if (scripts_mod) {
            sandbox::properties props = scripts_mod->serialize_scripts(flecs::entity(w, entity));
            sandbox_properties_handle_t handle = props.get_raw();
            props.release();
            return handle;
        }
        return {0};
    }

    sandbox::properties script_module_t::serialize_scripts(flecs::entity e) {
        sandbox::properties props;

        auto serialize_relation = [&](const char* rel_name, auto relation_type) {
            using RelType = decltype(relation_type);
            std::vector<sandbox::properties> items;

            e.each<RelType>([&](flecs::entity script_ent) {
                const RelType* rel_data_ptr = e.try_get<RelType>(script_ent);
                if (!rel_data_ptr) return;
                const RelType& rel_data = *rel_data_ptr;
                
                sandbox::properties item;
                item.set<std::string>("function", script_ent.name().c_str());
                
                sandbox::properties args_props;
                const spectre_script_t* script = script_ent.try_get<spectre_script_t>();
                if (script && rel_data.arguments && rel_data.argument_count > 0) {
                    for (size_t i = 0; i < rel_data.argument_count && i < script->argument_count; ++i) {
                        const char* arg_name = script->arguments_name[i];
                        spectre_script_argument_type_t arg_type = script->argument_types[i];
                        switch(arg_type) {
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER:
                                args_props.set<double>(arg_name, rel_data.arguments[i].value.number_value);
                                break;
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER:
                                args_props.set<long long>(arg_name, rel_data.arguments[i].value.integer_value);
                                break;
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN:
                                args_props.set<bool>(arg_name, rel_data.arguments[i].value.boolean_value);
                                break;
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING:
                                if (rel_data.arguments[i].value.string_value) {
                                    args_props.set<std::string>(arg_name, rel_data.arguments[i].value.string_value);
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                if (args_props.is_valid()) {
                    item.merge("arguments", args_props);
                    args_props.release();
                }
                items.push_back(std::move(item));
            });

            if (!items.empty()) {
                sandbox::properties arr_props;
                for (size_t i = 0; i < items.size(); ++i) {
                    arr_props.merge(std::to_string(i), items[i]);
                }
                props.merge(rel_name, arr_props);
                arr_props.release();
            }
        };

        serialize_relation("on_create", spectre_use_script_on_create_relation_t{});
        serialize_relation("on_destroy", spectre_use_script_on_destroy_relation_t{});
        serialize_relation("on_update", spectre_use_script_on_update_relation_t{});
        serialize_relation("on_render", spectre_use_script_on_render_relation_t{});
        serialize_relation("on_enter", spectre_use_script_on_enter_relation_t{});
        serialize_relation("on_exit", spectre_use_script_on_exit_relation_t{});

        return props;
    }


    script_module_t::~script_module_t() {
        if (m_lua) {
            lua_close(m_lua);
        }
    }

    scripts_t script_module_t::parse_code(std::string_view code) {
        scripts_t scripts;
        std::string code_str(code);
        std::istringstream stream(code_str);
        std::string line;

        std::vector<spectre_script_argument_type_t> pending_types;
        
        while (std::getline(stream, line)) {
            // Trim leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            if (line.empty()) continue;
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            // Check for --- @param <type>
            std::regex param_regex(R"(^---\s*@param\s+([a-zA-Z_][a-zA-Z0-9_]*))");
            std::smatch param_match;
            if (std::regex_search(line, param_match, param_regex)) {
                std::string type_str = param_match[1].str();
                spectre_script_argument_type_t type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL;
                
                if (type_str == "nil") type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL;
                else if (type_str == "boolean") type = SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN;
                else if (type_str == "number") type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER;
                else if (type_str == "integer") type = SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER;
                else if (type_str == "string") type = SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING;
                else if (type_str == "table") type = SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE;
                else if (type_str == "userdata") type = SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA;
                else {
                    sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Unknown parameter type '{}' in Lua script.", type_str);
                    type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL;
                }
                pending_types.push_back(type);
                continue;
            }

            // Check for function <name>(<args>)
            std::regex func_regex(R"(^function\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\))");
            std::smatch func_match;
            if (std::regex_search(line, func_match, func_regex)) {
                std::string func_name = func_match[1].str();
                std::string args_str = func_match[2].str();

                std::vector<std::string> args;
                std::regex arg_regex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
                auto args_begin = std::sregex_iterator(args_str.begin(), args_str.end(), arg_regex);
                auto args_end = std::sregex_iterator();
                for (std::sregex_iterator j = args_begin; j != args_end; ++j) {
                    args.push_back((*j).str());
                }

                size_t arg_count = args.size();

                if (pending_types.size() != arg_count) {
                    sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Parser Error: Function '{}' expects {} parameters based on arguments, but {} @param types were provided.", func_name, arg_count, pending_types.size());
                    pending_types.clear();
                    continue;
                }

                m_string_pool.push_back(func_name);
                const char* c_func_name = m_string_pool.back().c_str();

                std::vector<const char*> local_ptrs;
                std::vector<spectre_script_argument_type_t> local_types = pending_types;

                for (const auto& arg : args) {
                    m_string_pool.push_back(arg);
                    local_ptrs.push_back(m_string_pool.back().c_str());
                }

                m_ptr_pool.push_back(std::move(local_ptrs));
                m_type_pool.push_back(std::move(local_types));

                spectre_script_t script{};
                script.function_name = c_func_name;
                script.argument_count = arg_count;
                script.arguments_name = arg_count > 0 ? m_ptr_pool.back().data() : nullptr;
                script.argument_types = arg_count > 0 ? m_type_pool.back().data() : nullptr;

                scripts.push_back(script);

                // Reset for next function
                pending_types.clear();
            } else {
                if (line.find("--") != 0) {
                    pending_types.clear();
                }
            }
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
        
        flecs::entity script_ent = find_script(function_name);
        const auto* script_comp = script_ent.try_get<spectre_script_t>();

        if (script_comp->argument_count != args.size()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Script '{}' expects {} arguments, but {} were provided.", function_name, script_comp->argument_count, args.size());
            return;
        }

        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i].type != script_comp->argument_types[i]) {
                sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Script '{}' argument {} type mismatch. Expected {}, got {}.", function_name, i, (int)script_comp->argument_types[i], (int)args[i].type);
                return;
            }
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
                    lua_pushlightuserdata(m_lua, arg.value.userdata_pointer);
                    break;
                default:
                    lua_pushnil(m_lua);
                    break;
            }
        }

        if (lua_pcall(m_lua, args.size(), 0, 0) != LUA_OK) {
            const char* err = lua_tostring(m_lua, -1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Error executing {}: {}", function_name, err);
            lua_pop(m_lua, 1);
        }
    }

    namespace {
        flecs::entity resolve_entity_arg(flecs::entity target, const char* entity_str) {
            if (!entity_str) return flecs::entity::null();
            std::string s(entity_str);
            if (s == "%self%") {
                return target;
            } else if (s == "%scene%") {
                flecs::entity current = target;
                while (current.is_valid() && current.id() != 0) {
                    if (current.has<spectre_scene_context_t>()) {
                        return current.try_get<spectre_scene_context_t>()->scene_entity;
                    }
                    current = current.parent();
                }
            } else if (s == "%state%") {
                flecs::entity current = target;
                while (current.is_valid() && current.id() != 0) {
                    if (current.has<spectre_state_context_t>()) {
                        return current.try_get<spectre_state_context_t>()->state_entity;
                    }
                    current = current.parent();
                }
            }
            return flecs::entity::null();
        }
    }

    void script_module_t::execute_script_with_target(flecs::entity target, flecs::entity script_ent, spectre_script_argument_t* args, size_t arg_count) {
        if (!script_ent.is_valid() || !script_ent.has<spectre_script_t>()) return;
        
        const auto* script_comp = script_ent.try_get<spectre_script_t>();
        std::string function_name = script_comp->function_name;

        if (script_comp->argument_count != arg_count) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Script '{}' expects {} arguments, but {} were provided.", function_name, script_comp->argument_count, arg_count);
            return;
        }

        for (size_t i = 0; i < arg_count; ++i) {
            if (args[i].type != script_comp->argument_types[i]) {
                sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Script '{}' argument {} type mismatch. Expected {}, got {}.", function_name, i, (int)script_comp->argument_types[i], (int)args[i].type);
                return;
            }
        }

        lua_getglobal(m_lua, function_name.c_str());
        if (!lua_isfunction(m_lua, -1)) {
            lua_pop(m_lua, 1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Lua global {} is not a function.", function_name);
            return;
        }

        for (size_t i = 0; i < arg_count; ++i) {
            const auto& arg = args[i];
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
                    lua_pushlightuserdata(m_lua, arg.value.userdata_pointer);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_ENTITY: {
                    flecs::entity resolved = resolve_entity_arg(target, arg.value.entity);
                    lua_pushinteger(m_lua, resolved.id());
                    break;
                }
                default:
                    lua_pushnil(m_lua);
                    break;
            }
        }

        if (lua_pcall(m_lua, arg_count, 0, 0) != LUA_OK) {
            const char* err = lua_tostring(m_lua, -1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "Error executing {}: {}", function_name, err);
            lua_pop(m_lua, 1);
        }
    }

    void script_module_t::execute_on_create(flecs::entity entity) {
        if (!entity.is_valid()) return;
        flecs::entity scripts_ent = entity.lookup("scripts");
        if (!scripts_ent.is_valid()) scripts_ent = entity;
        scripts_ent.each<spectre_use_script_on_create_relation_t>([&](flecs::entity script_ent) {
            const auto* rel = scripts_ent.try_get<spectre_use_script_on_create_relation_t>(script_ent);
            if (rel) execute_script_with_target(entity, script_ent, rel->arguments, rel->argument_count);
        });
    }

    void script_module_t::execute_on_destroy(flecs::entity entity) {
        if (!entity.is_valid()) return;
        flecs::entity scripts_ent = entity.lookup("scripts");
        if (!scripts_ent.is_valid()) scripts_ent = entity;
        scripts_ent.each<spectre_use_script_on_destroy_relation_t>([&](flecs::entity script_ent) {
            const auto* rel = scripts_ent.try_get<spectre_use_script_on_destroy_relation_t>(script_ent);
            if (rel) execute_script_with_target(entity, script_ent, rel->arguments, rel->argument_count);
        });
    }

    void script_module_t::execute_on_update(flecs::entity entity) {
        if (!entity.is_valid()) return;
        flecs::entity scripts_ent = entity.lookup("scripts");
        if (!scripts_ent.is_valid()) scripts_ent = entity;
        scripts_ent.each<spectre_use_script_on_update_relation_t>([&](flecs::entity script_ent) {
            const auto* rel = scripts_ent.try_get<spectre_use_script_on_update_relation_t>(script_ent);
            if (rel) execute_script_with_target(entity, script_ent, rel->arguments, rel->argument_count);
        });
    }

    void script_module_t::execute_on_enter(flecs::entity entity) {
        if (!entity.is_valid()) return;
        flecs::entity scripts_ent = entity.lookup("scripts");
        if (!scripts_ent.is_valid()) scripts_ent = entity;
        scripts_ent.each<spectre_use_script_on_enter_relation_t>([&](flecs::entity script_ent) {
            const auto* rel = scripts_ent.try_get<spectre_use_script_on_enter_relation_t>(script_ent);
            if (rel) execute_script_with_target(entity, script_ent, rel->arguments, rel->argument_count);
        });
    }

    void script_module_t::execute_on_exit(flecs::entity entity) {
        if (!entity.is_valid()) return;
        flecs::entity scripts_ent = entity.lookup("scripts");
        if (!scripts_ent.is_valid()) scripts_ent = entity;
        scripts_ent.each<spectre_use_script_on_exit_relation_t>([&](flecs::entity script_ent) {
            const auto* rel = scripts_ent.try_get<spectre_use_script_on_exit_relation_t>(script_ent);
            if (rel) execute_script_with_target(entity, script_ent, rel->arguments, rel->argument_count);
        });
    }
}
