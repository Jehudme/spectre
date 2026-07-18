#include "scripts_module.h"
#include "spectre/services/scripts_service.h"
#include "sandbox/sdk/logs.hpp"
#include "spectre/sdk/serializer.hpp"
#include <sandbox/sdk/filesystem.hpp>

#include <lua.hpp>
#include <regex>
#include <iostream>
#include <fstream>
#include <sstream>

#include "spectre/sdk/scripts.hpp"

    // LuaJIT FFI initialization will happen in constructor

namespace spectre::modules {

    static void deserialize_script_args_cb(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t properties_handle);
    static sandbox_properties_handle_t serialize_script_args_cb(ecs_world_t* world, ecs_entity_t entity_id);

    // Component Registration Callbacks
    static ecs_entity_t register_script_comp(ecs_world_t* world) { 
        return flecs::world(world).component<spectre_script_t>()
            .member<uint32_t>("argument_count")
            .id(); 
    }
    static ecs_entity_t register_use_script_on_create_rel(ecs_world_t* world) { 
        return flecs::world(world).component<spectre_use_script_on_create_relation_t>()
            .member<int>("argument_count").id(); 
    }
    static ecs_entity_t register_use_script_on_destroy_rel(ecs_world_t* world) { 
        return flecs::world(world).component<spectre_use_script_on_destroy_relation_t>()
            .member<int>("argument_count").id(); 
    }
    static ecs_entity_t register_use_script_on_update_rel(ecs_world_t* world) { 
        return flecs::world(world).component<spectre_use_script_on_update_relation_t>()
            .member<int>("argument_count").id(); 
    }
    static ecs_entity_t register_use_script_on_enter_rel(ecs_world_t* world) { 
        return flecs::world(world).component<spectre_use_script_on_enter_relation_t>()
            .member<int>("argument_count").id(); 
    }
    static ecs_entity_t register_use_script_on_exit_rel(ecs_world_t* world) { 
        return flecs::world(world).component<spectre_use_script_on_exit_relation_t>()
            .member<int>("argument_count").id(); 
    }

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
        sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Scripts Module] Initializing...");

        m_scripts_root = m_world.entity("::scripts");
        m_script_prefab = m_world.prefab("::scripts::prefab");

        m_lua = luaL_newstate();
        luaL_openlibs(m_lua);

        // Initialize LuaJIT FFI
        luaL_dostring(m_lua, "local ffi = require('ffi')");
        
        char** files = nullptr;
        size_t file_count = 0;
        if (sandbox_filesystem_list_files(m_world.c_ptr(), "app://resources/assets/scripts/declarations", false, &files, &file_count)) {
            for (size_t i = 0; i < file_count; ++i) {
                if (std::string(files[i]).find(".h") != std::string::npos) {
                    uint8_t* data = nullptr;
                    size_t data_size = 0;
                    if (sandbox_filesystem_read_all_bytes(m_world.c_ptr(), files[i], &data, &data_size)) {
                        std::string cdef_content((const char*)data, data_size);
                        sandbox_filesystem_free_bytes(m_world.c_ptr(), data);
                        
                        lua_getglobal(m_lua, "require");
                        lua_pushstring(m_lua, "ffi");
                        lua_pcall(m_lua, 1, 1, 0);
                        lua_getfield(m_lua, -1, "cdef");
                        lua_pushlstring(m_lua, cdef_content.c_str(), cdef_content.size());
                        if (lua_pcall(m_lua, 1, 0, 0) != LUA_OK) {
                            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] FFI cdef error in {}: {}", files[i], lua_tostring(m_lua, -1));
                            lua_pop(m_lua, 1);
                        }
                        lua_pop(m_lua, 1); // pop ffi
                    }
                }
            }
            sandbox_filesystem_free_file_list(m_world.c_ptr(), files, file_count);
        } else {
            sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Scripts Module] Declarations folder not found");
        }
        
        // Register components using the SDK
        spectre_serializer_component empty_serializer = {nullptr, nullptr};
        
        register_script_comp(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_script_t", &empty_serializer);
        
        register_use_script_on_create_rel(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_create_relation_t", &empty_serializer);
        
        register_use_script_on_destroy_rel(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_destroy_relation_t", &empty_serializer);
        
        register_use_script_on_update_rel(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_update_relation_t", &empty_serializer);
        
        register_use_script_on_enter_rel(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_enter_relation_t", &empty_serializer);
        
        register_use_script_on_exit_rel(m_world.c_ptr());
        spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_exit_relation_t", &empty_serializer);

        spectre_serializer_component script_serializer = {};
        script_serializer.deserialize = deserialize_script_args_cb;
        script_serializer.serialize = serialize_script_args_cb;
        spectre::modules::serializer::register_serializer(m_world, "scripts", &script_serializer);

        sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Scripts Module] Initialized successfully.");
    }
    
    static void deserialize_script_args_cb(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t properties_handle) {
        auto* module = flecs::world(world).try_get_mut<script_module_t>();
        if (module) module->deserialize_scripts(flecs::world(world).entity(entity), sandbox::properties(properties_handle, false));
    }

    void script_module_t::deserialize_scripts(flecs::entity target_entity, sandbox::properties properties) {
        if (!properties.is_valid() || !target_entity.is_valid()) return;

        auto deserialize_relation = [&](const char* relation_name, auto relation_type) {
            if (!properties.has(relation_name)) return;
            sandbox::properties relation_properties = properties.sub(relation_name);
            std::vector<std::string> item_indices = relation_properties.keys("");
            for (const auto& index_str : item_indices) {
                sandbox::properties script_item = relation_properties.sub(index_str);
                std::string function_name;
                if (!script_item.get<std::string>("function", function_name)) continue;

                flecs::entity script_entity = m_scripts_root.lookup(function_name.c_str());
                if (!script_entity.is_valid()) {
                    sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Scripts Module] Script '{}' not found when deserializing relation '{}'.", function_name, relation_name);
                    continue;
                }
                
                const spectre_script_t* script_component = script_entity.try_get<spectre_script_t>();
                if (!script_component) continue;

                size_t argument_count = script_component->argument_count;
                spectre_script_argument_t* script_arguments = nullptr;
                if (argument_count > 0) {
                    script_arguments = new spectre_script_argument_t[argument_count];
                    sandbox::properties arguments_properties = script_item.sub("arguments");
                    for (size_t index = 0; index < argument_count; ++index) {
                        const char* argument_name = script_component->arguments_name[index];
                        spectre_script_argument_type_t argument_type = script_component->argument_types[index];
                        script_arguments[index].type = argument_type;
                        
                        switch (argument_type) {
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER: {
                                double number_value = 0.0;
                                arguments_properties.get<double>(argument_name, number_value);
                                script_arguments[index].value.number_value = number_value;
                                break;
                            }
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER: {
                                long long integer_value = 0;
                                arguments_properties.get<long long>(argument_name, integer_value);
                                script_arguments[index].value.integer_value = integer_value;
                                break;
                            }
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN: {
                                bool boolean_value = false;
                                arguments_properties.get<bool>(argument_name, boolean_value);
                                script_arguments[index].value.boolean_value = boolean_value;
                                break;
                            }
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING: {
                                std::string string_value;
                                arguments_properties.get<std::string>(argument_name, string_value);
                                script_arguments[index].value.string_value = strdup(string_value.c_str());
                                break;
                            }
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_ENTITY: {
                                std::string entity_value;
                                arguments_properties.get<std::string>(argument_name, entity_value);
                                if (entity_value.empty()) entity_value = "%self%";
                                script_arguments[index].value.entity = strdup(entity_value.c_str());
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }

                using RelationType = decltype(relation_type);
                RelationType relation_data;
                relation_data.arguments = script_arguments;
                relation_data.argument_count = argument_count;
                
                target_entity.set<RelationType>(script_entity, relation_data);
            }
        };

        deserialize_relation("on_create", spectre_use_script_on_create_relation_t{});
        deserialize_relation("on_destroy", spectre_use_script_on_destroy_relation_t{});
        deserialize_relation("on_update", spectre_use_script_on_update_relation_t{});
        deserialize_relation("on_render", spectre_use_script_on_render_relation_t{});
        deserialize_relation("on_enter", spectre_use_script_on_enter_relation_t{});
        deserialize_relation("on_exit", spectre_use_script_on_exit_relation_t{});
    }

    static sandbox_properties_handle_t serialize_script_args_cb(ecs_world_t* world, ecs_entity_t entity_id) {
        return spectre_scripts_serialize_scripts(world, entity_id);
    }

    sandbox::properties script_module_t::serialize_scripts(flecs::entity entity_to_serialize) {
        sandbox::properties serialized_properties;

        auto serialize_relation = [&](const char* relation_name, auto relation_type) {
            using RelationType = decltype(relation_type);
            std::vector<sandbox::properties> relation_items;

            entity_to_serialize.each<RelationType>([&](flecs::entity script_entity) {
                const RelationType* relation_data_pointer = entity_to_serialize.try_get<RelationType>(script_entity);
                if (!relation_data_pointer) return;
                const RelationType& relation_data = *relation_data_pointer;
                
                sandbox::properties item_properties;
                item_properties.set<std::string>("function", script_entity.name().c_str());
                
                sandbox::properties arguments_properties;
                const spectre_script_t* script_component = script_entity.try_get<spectre_script_t>();
                if (script_component && relation_data.arguments && relation_data.argument_count > 0) {
                    for (size_t index = 0; index < relation_data.argument_count && index < script_component->argument_count; ++index) {
                        const char* argument_name = script_component->arguments_name[index];
                        spectre_script_argument_type_t argument_type = script_component->argument_types[index];
                        switch(argument_type) {
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER:
                                arguments_properties.set<double>(argument_name, relation_data.arguments[index].value.number_value);
                                break;
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER:
                                arguments_properties.set<long long>(argument_name, relation_data.arguments[index].value.integer_value);
                                break;
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN:
                                arguments_properties.set<bool>(argument_name, relation_data.arguments[index].value.boolean_value);
                                break;
                            case SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING:
                                if (relation_data.arguments[index].value.string_value) {
                                    arguments_properties.set<std::string>(argument_name, relation_data.arguments[index].value.string_value);
                                }
                                break;
                            default:
                                break;
                        }
                    }
                }
                if (arguments_properties.is_valid()) {
                    item_properties.merge("arguments", arguments_properties);
                    arguments_properties.release();
                }
                relation_items.push_back(std::move(item_properties));
            });

            if (!relation_items.empty()) {
                sandbox::properties array_properties;
                for (size_t index = 0; index < relation_items.size(); ++index) {
                    array_properties.merge(std::to_string(index), relation_items[index]);
                }
                serialized_properties.merge(relation_name, array_properties);
                array_properties.release();
            }
        };

        serialize_relation("on_create", spectre_use_script_on_create_relation_t{});
        serialize_relation("on_destroy", spectre_use_script_on_destroy_relation_t{});
        serialize_relation("on_update", spectre_use_script_on_update_relation_t{});
        serialize_relation("on_render", spectre_use_script_on_render_relation_t{});
        serialize_relation("on_enter", spectre_use_script_on_enter_relation_t{});
        serialize_relation("on_exit", spectre_use_script_on_exit_relation_t{});

        return serialized_properties;
    }


    script_module_t::~script_module_t() {
        if (m_lua) {
            lua_close(m_lua);
        }
    }

    scripts_t script_module_t::parse_code(std::string_view source_code) {
        scripts_t parsed_scripts;
        std::string source_code_string(source_code);
        std::istringstream stream(source_code_string);
        std::string line;

        std::vector<spectre_script_argument_type_t> pending_argument_types;
        
        while (std::getline(stream, line)) {
            // Trim leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            if (line.empty()) continue;
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            // Check for --- @param <type>
            std::regex param_regex(R"(^---\s*@param\s+([a-zA-Z_][a-zA-Z0-9_]*))");
            std::smatch param_match;
            if (std::regex_search(line, param_match, param_regex)) {
                std::string type_string = param_match[1].str();
                spectre_script_argument_type_t argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL;
                
                if (type_string == "nil") argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL;
                else if (type_string == "boolean") argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN;
                else if (type_string == "number") argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER;
                else if (type_string == "integer") argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER;
                else if (type_string == "string") argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING;
                else if (type_string == "table") argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE;
                else if (type_string == "userdata") argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA;
                else if (type_string == "entity") argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_ENTITY;
                else {
                    sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Scripts Module] Unknown parameter type '{}' in Lua script.", type_string);
                    argument_type = SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL;
                }
                pending_argument_types.push_back(argument_type);
                continue;
            }

            // Check for function <name>(<args>)
            std::regex function_regex(R"(^function\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\))");
            std::smatch function_match;
            if (std::regex_search(line, function_match, function_regex)) {
                std::string function_name = function_match[1].str();
                std::string arguments_string = function_match[2].str();

                std::vector<std::string> argument_names;
                std::regex argument_regex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
                auto arguments_begin = std::sregex_iterator(arguments_string.begin(), arguments_string.end(), argument_regex);
                auto arguments_end = std::sregex_iterator();
                for (std::sregex_iterator iterator = arguments_begin; iterator != arguments_end; ++iterator) {
                    argument_names.push_back((*iterator).str());
                }

                size_t argument_count = argument_names.size();

                if (pending_argument_types.size() != argument_count) {
                    sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Parser Error: Function '{}' expects {} parameters based on arguments, but {} @param types were provided.", function_name, argument_count, pending_argument_types.size());
                    pending_argument_types.clear();
                    continue;
                }

                m_string_pool.push_back(function_name);
                const char* c_function_name = m_string_pool.back().c_str();

                std::vector<const char*> local_pointers;
                std::vector<spectre_script_argument_type_t> local_types = pending_argument_types;

                for (const auto& argument_name : argument_names) {
                    m_string_pool.push_back(argument_name);
                    local_pointers.push_back(m_string_pool.back().c_str());
                }

                m_ptr_pool.push_back(std::move(local_pointers));
                m_type_pool.push_back(std::move(local_types));

                spectre_script_t script{};
                script.function_name = c_function_name;
                script.argument_count = argument_count;
                script.arguments_name = argument_count > 0 ? m_ptr_pool.back().data() : nullptr;
                script.argument_types = argument_count > 0 ? m_type_pool.back().data() : nullptr;

                parsed_scripts.push_back(script);

                // Reset for next function
                pending_argument_types.clear();
            } else {
                if (line.find("--") != 0) {
                    pending_argument_types.clear();
                }
            }
        }
        return parsed_scripts;
    }

    void script_module_t::register_scripts(const scripts_t& scripts) {
        for (const auto& script : scripts) {
            std::string full_path = std::string("::scripts::") + script.function_name;
            if (!has_script(script.function_name, {})) {
                flecs::entity script_entity = m_world.entity(full_path.c_str())
                                                  .is_a(m_script_prefab)
                                                  .set<spectre_script_t>(script);
            } else {
                flecs::entity script_entity = find_script(script.function_name);
                script_entity.set<spectre_script_t>(script);
            }
        }
    }

    bool script_module_t::has_script(std::string_view function_name, const std::vector<script_argument_type_t>& arguments) const {
        if (function_name.empty()) return false;
        flecs::entity script_entity = m_scripts_root.lookup(std::string(function_name).c_str());
        if (!is_script(script_entity)) return false;
        
        return true;
    }

    bool script_module_t::is_script(flecs::entity entity_to_check) const {
        return entity_to_check.is_valid() && entity_to_check.has(flecs::IsA, m_script_prefab);
    }

    flecs::entity script_module_t::find_script(std::string_view function_name) {
        if (function_name.empty()) return flecs::entity::null();
        return m_scripts_root.lookup(std::string(function_name).c_str());
    }

    void script_module_t::include_code(std::string_view file_path) {
        if (file_path.substr(file_path.find_last_of(".") + 1) != "lua") return;

        std::string source_code = sandbox::modules::filesystem::read_all_text(const_cast<flecs::world&>(m_world), file_path.data());
        if (source_code.empty()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Failed to open/read script: {}", file_path);
            return;
        }

        // Parse and register scripts
        scripts_t parsed_scripts = parse_code(source_code);
        register_scripts(parsed_scripts);

        // Execute code
        if (luaL_dostring(m_lua, source_code.c_str()) != LUA_OK) {
            const char* error_message = lua_tostring(m_lua, -1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Lua Error: {}", error_message);
            lua_pop(m_lua, 1);
        } else {
            sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Scripts Module] Successfully included script: {}", file_path);
        }
    }

    void script_module_t::execute_script(std::string_view function_name, script_arguments_t& arguments) {
        if (!has_script(function_name, {})) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Cannot execute unknown script: {}", function_name);
            return;
        }
        
        flecs::entity script_entity = find_script(function_name);
        const auto* script_component = script_entity.try_get<spectre_script_t>();

        if (script_component->argument_count != arguments.size()) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Script '{}' expects {} arguments, but {} were provided.", function_name, script_component->argument_count, arguments.size());
            return;
        }

        for (size_t index = 0; index < arguments.size(); ++index) {
            if (arguments[index].type != script_component->argument_types[index]) {
                sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Script '{}' argument {} type mismatch. Expected {}, got {}.", function_name, index, (int)script_component->argument_types[index], (int)arguments[index].type);
                return;
            }
        }

        lua_getglobal(m_lua, std::string(function_name).c_str());
        if (!lua_isfunction(m_lua, -1)) {
            lua_pop(m_lua, 1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Lua global '{}' is not a function.", function_name);
            return;
        }

        for (const auto& argument : arguments) {
            switch (argument.type) {
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL:
                    lua_pushnil(m_lua);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN:
                    lua_pushboolean(m_lua, argument.value.boolean_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER:
                    lua_pushnumber(m_lua, argument.value.number_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER:
                    lua_pushinteger(m_lua, argument.value.integer_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING:
                    lua_pushstring(m_lua, argument.value.string_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE:
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA:
                    lua_pushlightuserdata(m_lua, argument.value.userdata_pointer);
                    break;
                default:
                    lua_pushnil(m_lua);
                    break;
            }
        }

        if (lua_pcall(m_lua, arguments.size(), 0, 0) != LUA_OK) {
            const char* error_message = lua_tostring(m_lua, -1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Error executing '{}': {}", function_name, error_message);
            lua_pop(m_lua, 1);
        }
    }

    namespace {
        flecs::entity resolve_entity_argument(flecs::entity target_entity, const char* entity_string) {
            if (!entity_string) return flecs::entity::null();
            std::string entity_str(entity_string);
            if (entity_str == "%self%") {
                return target_entity;
            } else if (entity_str == "%scene%") {
                flecs::entity current_entity = target_entity;
                while (current_entity.is_valid() && current_entity.id() != 0) {
                    if (current_entity.has<spectre_scene_context_t>()) {
                        return current_entity.try_get<spectre_scene_context_t>()->scene_entity;
                    }
                    current_entity = current_entity.parent();
                }
            } else if (entity_str == "%state%") {
                flecs::entity current_entity = target_entity;
                while (current_entity.is_valid() && current_entity.id() != 0) {
                    if (current_entity.has<spectre_state_context_t>()) {
                        return current_entity.try_get<spectre_state_context_t>()->state_entity;
                    }
                    current_entity = current_entity.parent();
                }
            }
            return flecs::entity::null();
        }
    }

    void script_module_t::execute_script_with_target(flecs::entity target_entity, flecs::entity script_entity, spectre_script_argument_t* script_arguments, size_t argument_count) {
        if (!script_entity.is_valid() || !script_entity.has<spectre_script_t>()) return;
        
        const auto* script_component = script_entity.try_get<spectre_script_t>();
        std::string function_name = script_component->function_name;

        if (script_component->argument_count != argument_count) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Script '{}' expects {} arguments, but {} were provided.", function_name, script_component->argument_count, argument_count);
            return;
        }

        for (size_t index = 0; index < argument_count; ++index) {
            if (script_arguments[index].type != script_component->argument_types[index]) {
                sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Script '{}' argument {} type mismatch. Expected {}, got {}.", function_name, index, (int)script_component->argument_types[index], (int)script_arguments[index].type);
                return;
            }
        }

        lua_getglobal(m_lua, function_name.c_str());
        if (!lua_isfunction(m_lua, -1)) {
            lua_pop(m_lua, 1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Lua global '{}' is not a function.", function_name);
            return;
        }

        for (size_t index = 0; index < argument_count; ++index) {
            const auto& argument = script_arguments[index];
            switch (argument.type) {
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL:
                    lua_pushnil(m_lua);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN:
                    lua_pushboolean(m_lua, argument.value.boolean_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER:
                    lua_pushnumber(m_lua, argument.value.number_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER:
                    lua_pushinteger(m_lua, argument.value.integer_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING:
                    lua_pushstring(m_lua, argument.value.string_value);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE:
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA:
                    lua_pushlightuserdata(m_lua, argument.value.userdata_pointer);
                    break;
                case SPECTRE_SCRIPT_ARGUMENT_TYPE_ENTITY: {
                    flecs::entity resolved_entity = resolve_entity_argument(target_entity, argument.value.entity);
                    lua_pushinteger(m_lua, resolved_entity.id());
                    break;
                }
                default:
                    lua_pushnil(m_lua);
                    break;
            }
        }

        if (lua_pcall(m_lua, argument_count, 0, 0) != LUA_OK) {
            const char* error_message = lua_tostring(m_lua, -1);
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Error executing '{}': {}", function_name, error_message);
            lua_pop(m_lua, 1);
        }
    }

    void script_module_t::execute_on_create(flecs::entity target_entity) {
        if (!target_entity.is_valid()) return;
        flecs::entity scripts_entity = target_entity.lookup("scripts");
        if (!scripts_entity.is_valid()) scripts_entity = target_entity;
        scripts_entity.each<spectre_use_script_on_create_relation_t>([&](flecs::entity script_entity) {
            const auto* relation = scripts_entity.try_get<spectre_use_script_on_create_relation_t>(script_entity);
            if (relation) execute_script_with_target(target_entity, script_entity, relation->arguments, relation->argument_count);
        });
    }

    void script_module_t::execute_on_destroy(flecs::entity target_entity) {
        if (!target_entity.is_valid()) return;
        flecs::entity scripts_entity = target_entity.lookup("scripts");
        if (!scripts_entity.is_valid()) scripts_entity = target_entity;
        scripts_entity.each<spectre_use_script_on_destroy_relation_t>([&](flecs::entity script_entity) {
            const auto* relation = scripts_entity.try_get<spectre_use_script_on_destroy_relation_t>(script_entity);
            if (relation) execute_script_with_target(target_entity, script_entity, relation->arguments, relation->argument_count);
        });
    }

    void script_module_t::execute_on_update(flecs::entity target_entity) {
        if (!target_entity.is_valid()) return;
        flecs::entity scripts_entity = target_entity.lookup("scripts");
        if (!scripts_entity.is_valid()) scripts_entity = target_entity;
        scripts_entity.each<spectre_use_script_on_update_relation_t>([&](flecs::entity script_entity) {
            const auto* relation = scripts_entity.try_get<spectre_use_script_on_update_relation_t>(script_entity);
            if (relation) execute_script_with_target(target_entity, script_entity, relation->arguments, relation->argument_count);
        });
    }

    void script_module_t::execute_on_enter(flecs::entity target_entity) {
        if (!target_entity.is_valid()) return;
        flecs::entity scripts_entity = target_entity.lookup("scripts");
        if (!scripts_entity.is_valid()) scripts_entity = target_entity;
        scripts_entity.each<spectre_use_script_on_enter_relation_t>([&](flecs::entity script_entity) {
            const auto* relation = scripts_entity.try_get<spectre_use_script_on_enter_relation_t>(script_entity);
            if (relation) execute_script_with_target(target_entity, script_entity, relation->arguments, relation->argument_count);
        });
    }

    void script_module_t::execute_on_exit(flecs::entity target_entity) {
        if (!target_entity.is_valid()) return;
        flecs::entity scripts_entity = target_entity.lookup("scripts");
        if (!scripts_entity.is_valid()) scripts_entity = target_entity;
        scripts_entity.each<spectre_use_script_on_exit_relation_t>([&](flecs::entity script_entity) {
            const auto* relation = scripts_entity.try_get<spectre_use_script_on_exit_relation_t>(script_entity);
            if (relation) execute_script_with_target(target_entity, script_entity, relation->arguments, relation->argument_count);
        });
    }
}
