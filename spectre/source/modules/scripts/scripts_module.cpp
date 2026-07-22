#include "scripts_module.h"
#include "sandbox/sdk/logs.hpp"
#include "spectre/sdk/serializer.hpp"
#include "spectre/services/scripts_service.h"
#include <sandbox/sdk/filesystem.hpp>

#include <lua.hpp>
#include <algorithm>
#include "spectre/sdk/scripts.hpp"

namespace spectre::modules {

static void deserialize_script_args_cb(ecs_world_t* world, ecs_entity_t entity,
                                       sandbox_properties_handle_t properties_handle);
static sandbox_properties_handle_t serialize_script_args_cb(ecs_world_t* world, ecs_entity_t entity_id);

static sandbox_requirement_info_t scripts_requirements[] = {{.kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
                                                             .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
                                                             .name = "logs",
                                                             .architecture = "sandbox",
                                                             .version_major = 1,
                                                             .version_minor = 0,
                                                             .version_patch = -1},
                                                            {.kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
                                                             .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
                                                             .name = "filesystem",
                                                             .architecture = "sandbox::core",
                                                             .version_major = 1,
                                                             .version_minor = 0,
                                                             .version_patch = -1}};

SANDBOX_DECLARE_MODULE(script_module_t, {.name = "scripts",
                                         .description = "Scripts Module",
                                         .architecture = "spectre",
                                         .version_major = 1,
                                         .version_minor = 0,
                                         .version_patch = 0,
                                         .service = &spectre_scripts_service_t_info,
                                         .requirements = scripts_requirements,
                                         .requirement_count = 2})

static int custom_vfs_loader(lua_State* L) {
    const char* modname = luaL_checkstring(L, 1);
    auto* world = static_cast<ecs_world_t*>(lua_touserdata(L, lua_upvalueindex(1)));

    std::string path1 = std::string("app://resources/assets/scripts/externals/") + modname + ".lua";
    std::string path2 = std::string("app://resources/assets/scripts/") + modname + ".lua";

    uint8_t* data = nullptr;
    size_t data_size = 0;

    if (!sandbox_filesystem_read_all_bytes(world, path1.c_str(), &data, &data_size)) {
        if (!sandbox_filesystem_read_all_bytes(world, path2.c_str(), &data, &data_size)) {
            std::string modpath = modname;
            std::replace(modpath.begin(), modpath.end(), '.', '/');
            std::string path3 = std::string("app://resources/assets/scripts/") + modpath + ".lua";
            if (!sandbox_filesystem_read_all_bytes(world, path3.c_str(), &data, &data_size)) {
                lua_pushstring(L, "\n\tno file found in sandbox VFS");
                return 1;
            }
        }
    }

    int status = luaL_loadbuffer(L, (const char*)data, data_size, modname);
    sandbox_filesystem_free_bytes(world, data);

    if (status == 0) {
        return 1;
    }

    return lua_error(L);
}

const char* script_module_t::intern_string(const std::string& str) {
    return m_string_pool.insert(str).first->c_str();
}

template<typename T>
void register_relation_hooks(flecs::world& world, const char* name) {
    world.component<T>(name).on_remove([](flecs::entity e, T& rel) {
        if (rel.arguments) {
            for (int i = 0; i < rel.argument_count; ++i) {
                if (rel.arguments[i].type == SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING && rel.arguments[i].value.string_value) {
                    free((void*)rel.arguments[i].value.string_value);
                }
            }
            delete[] rel.arguments;
        }
    });
}

script_module_t::script_module_t(flecs::world& world) : m_world(world), m_ffi_initialized(false) {
    sandbox::modules::logs::trace(const_cast<flecs::world&>(m_world), "[Scripts Module] Initializing...");

    m_scripts_root = m_world.entity("::scripts");
    m_script_prefab = m_world.prefab("::scripts::prefab");

    m_lua = luaL_newstate();
    luaL_openlibs(m_lua);

    lua_pushlightuserdata(m_lua, m_world.c_ptr());
    lua_setglobal(m_lua, "g_world");

    lua_getglobal(m_lua, "package");
    lua_getfield(m_lua, -1, "loaders");
    int num_loaders = lua_objlen(m_lua, -1);
    lua_pushlightuserdata(m_lua, m_world.c_ptr());
    lua_pushcclosure(m_lua, custom_vfs_loader, 1);
    lua_rawseti(m_lua, -2, num_loaders + 1);
    lua_pop(m_lua, 2);

    luaL_dostring(m_lua, "local ffi = require('ffi')");

    auto deserialize_empty = [](ecs_world_t*, ecs_entity_t, sandbox_properties_handle_t) {};
    auto serialize_empty = [](ecs_world_t*, ecs_entity_t) -> sandbox_properties_handle_t { return {0}; };
    spectre_serializer_component empty_serializer = {deserialize_empty, serialize_empty};

    m_world.component<spectre_script_t>("Script").on_remove([](flecs::entity e, spectre_script_t& s) {
        delete[] s.arguments_name;
        delete[] s.argument_types;
        if (s.lua_function_ref != LUA_REFNIL) {
            const auto* mod_ptr = e.world().try_get<script_module_t>();
            if (mod_ptr) {
                luaL_unref(mod_ptr->m_lua, LUA_REGISTRYINDEX, s.lua_function_ref);
            }
        }
    });
    spectre::modules::serializer::register_serializer(m_world, "spectre_script_t", &empty_serializer);

    register_relation_hooks<spectre_use_script_on_create_relation_t>(m_world, "UseScriptOnCreateRelation");
    spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_create_relation_t", &empty_serializer);

    register_relation_hooks<spectre_use_script_on_destroy_relation_t>(m_world, "UseScriptOnDestroyRelation");
    spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_destroy_relation_t", &empty_serializer);

    register_relation_hooks<spectre_use_script_on_update_relation_t>(m_world, "UseScriptOnUpdateRelation");
    spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_update_relation_t", &empty_serializer);

    register_relation_hooks<spectre_use_script_on_enter_relation_t>(m_world, "UseScriptOnEnterRelation");
    spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_enter_relation_t", &empty_serializer);

    register_relation_hooks<spectre_use_script_on_exit_relation_t>(m_world, "UseScriptOnExitRelation");
    spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_exit_relation_t", &empty_serializer);

    register_relation_hooks<spectre_use_script_on_render_relation_t>(m_world, "UseScriptOnRenderRelation");
    spectre::modules::serializer::register_serializer(m_world, "spectre_use_script_on_render_relation_t", &empty_serializer);

    spectre_serializer_component script_serializer = {};
    script_serializer.deserialize = deserialize_script_args_cb;
    script_serializer.serialize = serialize_script_args_cb;
    spectre::modules::serializer::register_serializer(m_world, "scripts", &script_serializer);

    sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Scripts Module] Initialized successfully.");
}

static void deserialize_script_args_cb(ecs_world_t* world, ecs_entity_t entity,
                                       sandbox_properties_handle_t properties_handle) {
    const auto* module_ptr = flecs::world(world).try_get<script_module_t>();
    if (module_ptr)
        const_cast<script_module_t*>(module_ptr)->deserialize_scripts(flecs::world(world).entity(entity), sandbox::properties(properties_handle, false));
}

namespace {
flecs::entity resolve_entity_argument(flecs::entity target_entity, const std::string& entity_str) {
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
} // namespace

void script_module_t::deserialize_scripts(flecs::entity target_entity, sandbox::properties properties) {
    if (!properties.is_valid() || !target_entity.is_valid())
        return;

    auto deserialize_relation = [&](const char* relation_name, auto relation_type) {
        if (!properties.has(relation_name))
            return;
        sandbox::properties relation_properties = properties.sub(relation_name);
        std::vector<std::string> item_indices = relation_properties.keys("");
        for (const auto& index_str : item_indices) {
            sandbox::properties script_item = relation_properties.sub(index_str);
            std::string function_name;
            if (!script_item.get<std::string>("function", function_name))
                continue;

            flecs::entity script_entity = m_scripts_root.lookup(function_name.c_str());
            if (!script_entity.is_valid()) {
                sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world),
                                             "[Scripts Module] Script '{}' not found "
                                             "when deserializing relation '{}'.",
                                             function_name, relation_name);
                continue;
            }

            const spectre_script_t* script_component = script_entity.try_get<spectre_script_t>();
            if (!script_component)
                continue;

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
                            std::string str_val;
                            if (arguments_properties.get<std::string>(argument_name, str_val)) {
                                flecs::entity resolved = resolve_entity_argument(target_entity, str_val);
                                script_arguments[index].value.integer_value = resolved.id();
                            } else {
                                long long integer_value = 0;
                                arguments_properties.get<long long>(argument_name, integer_value);
                                script_arguments[index].value.integer_value = integer_value;
                            }
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
                        default:
                            break;
                    }
                }
            }

            using RelationType = decltype(relation_type);
            RelationType relation_data;
            relation_data.arguments = script_arguments;
            relation_data.argument_count = (int)argument_count;

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
            if (!relation_data_pointer)
                return;
            const RelationType& relation_data = *relation_data_pointer;

            sandbox::properties item_properties;
            item_properties.set<std::string>("function", script_entity.name().c_str());

            sandbox::properties arguments_properties;
            const spectre_script_t* script_component = script_entity.try_get<spectre_script_t>();
            if (script_component && relation_data.arguments && relation_data.argument_count > 0) {
                for (size_t index = 0; index < relation_data.argument_count && index < script_component->argument_count;
                     ++index) {
                    const char* argument_name = script_component->arguments_name[index];
                    spectre_script_argument_type_t argument_type = script_component->argument_types[index];
                    switch (argument_type) {
                        case SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER:
                            arguments_properties.set<double>(argument_name,
                                                             relation_data.arguments[index].value.number_value);
                            break;
                        case SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER:
                            arguments_properties.set<long long>(argument_name,
                                                                relation_data.arguments[index].value.integer_value);
                            break;
                        case SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN:
                            arguments_properties.set<bool>(argument_name,
                                                           relation_data.arguments[index].value.boolean_value);
                            break;
                        case SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING:
                            if (relation_data.arguments[index].value.string_value) {
                                arguments_properties.set<std::string>(argument_name,
                                                                      relation_data.arguments[index].value.string_value);
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

static spectre_script_argument_type_t map_string_to_type(const std::string& type_str) {
    if (type_str == "nil") return SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL;
    if (type_str == "boolean") return SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN;
    if (type_str == "number") return SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER;
    if (type_str == "integer") return SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER;
    if (type_str == "string") return SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING;
    if (type_str == "table") return SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE;
    if (type_str == "userdata") return SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA;
    return SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL;
}

void script_module_t::include_code(std::string_view file_path) {
    if (file_path.empty())
        return;

    uint8_t* data = nullptr;
    size_t data_size = 0;
    if (!sandbox_filesystem_read_all_bytes(m_world.c_ptr(), file_path.data(), &data, &data_size)) {
        sandbox::modules::logs::error(const_cast<flecs::world&>(m_world),
                                      "[Scripts Module] Failed to open/read script: {}", file_path.data());
        return;
    }
    
    std::string source_code((const char*)data, data_size);
    sandbox_filesystem_free_bytes(m_world.c_ptr(), data);
    
    eval_code(source_code, file_path);
}

void script_module_t::eval_code(std::string_view source_code, std::string_view chunk_name) {
    if (source_code.empty()) return;
    
    if (luaL_loadbuffer(m_lua, source_code.data(), source_code.size(), chunk_name.data()) != LUA_OK) {
        sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Lua parse error: {}",
                                      lua_tostring(m_lua, -1));
        lua_pop(m_lua, 1);
        return;
    }

    if (lua_pcall(m_lua, 0, 1, 0) != LUA_OK) {
        sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Lua execution error: {}",
                                      lua_tostring(m_lua, -1));
        lua_pop(m_lua, 1);
        return;
    }

    if (!lua_istable(m_lua, -1)) {
        sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Script {} did not return a metadata table", chunk_name.data());
        lua_pop(m_lua, 1);
        return;
    }

    lua_pushnil(m_lua);
    while (lua_next(m_lua, -2) != 0) {
        std::string function_name = lua_tostring(m_lua, -2);

        if (lua_istable(m_lua, -1)) {
            lua_getfield(m_lua, -1, "func");
            int lua_func_ref = LUA_REFNIL;
            if (lua_isfunction(m_lua, -1)) {
                lua_func_ref = luaL_ref(m_lua, LUA_REGISTRYINDEX);
            } else {
                lua_pop(m_lua, 1);
            }

            std::vector<const char*> arg_names;
            std::vector<spectre_script_argument_type_t> arg_types;

            lua_getfield(m_lua, -1, "args");
            if (lua_istable(m_lua, -1)) {
                size_t arg_count = lua_objlen(m_lua, -1);
                for (size_t i = 1; i <= arg_count; ++i) {
                    lua_rawgeti(m_lua, -1, i);
                    
                    lua_getfield(m_lua, -1, "name");
                    arg_names.push_back(intern_string(lua_tostring(m_lua, -1)));
                    lua_pop(m_lua, 1);
                    
                    lua_getfield(m_lua, -1, "type");
                    arg_types.push_back(map_string_to_type(lua_tostring(m_lua, -1)));
                    lua_pop(m_lua, 1);
                    
                    lua_pop(m_lua, 1);
                }
            }
            lua_pop(m_lua, 1); // pop args

            std::string full_path = std::string("::scripts::") + function_name;
            flecs::entity script_entity = m_world.entity(full_path.c_str()).is_a(m_script_prefab);

            size_t count = arg_names.size();
            const char** stable_names = nullptr;
            spectre_script_argument_type_t* stable_types = nullptr;
            
            if (count > 0) {
                stable_names = new const char*[count];
                stable_types = new spectre_script_argument_type_t[count];
                for (size_t i = 0; i < count; ++i) {
                    stable_names[i] = arg_names[i];
                    stable_types[i] = arg_types[i];
                }
            }

            spectre_script_t script_data = {
                lua_func_ref,
                stable_names,
                stable_types,
                (uint32_t)count
            };
            script_entity.set<spectre_script_t>(script_data);
        }
        lua_pop(m_lua, 1);
    }
    lua_pop(m_lua, 1);
    
    sandbox::modules::logs::info(const_cast<flecs::world&>(m_world),
                                 "[Scripts Module] Successfully evaluated script: {}", chunk_name.data());
}


bool script_module_t::is_script(flecs::entity entity_to_check) const {
    return entity_to_check.is_valid() && entity_to_check.has(flecs::IsA, m_script_prefab);
}

flecs::entity script_module_t::find_script(std::string_view function_name) const {
    if (function_name.empty())
        return flecs::entity::null();
    return m_scripts_root.lookup(std::string(function_name).c_str());
}

void script_module_t::execute_script_with_target(flecs::entity target_entity, flecs::entity script_entity,
                                                 spectre_script_argument_t* script_arguments, size_t argument_count) {
    if (!script_entity.is_valid() || !script_entity.has<spectre_script_t>())
        return;

    const auto* script_component = script_entity.try_get<spectre_script_t>();

    if (script_component->argument_count != argument_count) {
        sandbox::modules::logs::error(const_cast<flecs::world&>(m_world),
                                      "[Scripts Module] Script '{}' expects {} arguments, but {} were "
                                      "provided.",
                                      script_entity.name().c_str(), script_component->argument_count, argument_count);
        return;
    }

    for (size_t index = 0; index < argument_count; ++index) {
        if (script_arguments[index].type != script_component->argument_types[index]) {
            sandbox::modules::logs::error(const_cast<flecs::world&>(m_world),
                                          "[Scripts Module] Script '{}' argument {} type mismatch. Expected "
                                          "{}, got {}.",
                                          script_entity.name().c_str(), index, (int)script_component->argument_types[index],
                                          (int)script_arguments[index].type);
            return;
        }
    }

    if (script_component->lua_function_ref == LUA_REFNIL) {
        sandbox::modules::logs::error(const_cast<flecs::world&>(m_world),
                                      "[Scripts Module] Script '{}' has invalid lua reference.", script_entity.name().c_str());
        return;
    }

    lua_rawgeti(m_lua, LUA_REGISTRYINDEX, script_component->lua_function_ref);
    
    lua_pushinteger(m_lua, target_entity.id()); // First arg is ALWAYS the entity ID

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
            default:
                lua_pushnil(m_lua);
                break;
        }
    }

    if (lua_pcall(m_lua, argument_count + 1, 0, 0) != LUA_OK) {
        const char* error_message = lua_tostring(m_lua, -1);
        sandbox::modules::logs::error(const_cast<flecs::world&>(m_world), "[Scripts Module] Error executing '{}': {}",
                                      script_entity.name().c_str(), error_message);
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
} // namespace spectre::modules
