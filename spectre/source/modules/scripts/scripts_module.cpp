#include "scripts_module.h"
#include <spectre/abi/renderer_components.h>
#include <sandbox/sdk/filesystem.hpp>
#include <sandbox/sdk/logs.hpp>
#include <sandbox/abi/bootstrapper.h>
#include <cstring>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// SWIG-generated setup function declaration
extern "C" int luaopen_spectre(lua_State* L);

namespace spectre::module {
    struct ScriptsModulePtr {
        ScriptsModule* ptr;
    };

    ScriptsModule::ScriptsModule(flecs::world& ecs) : m_world(ecs), m_state(nullptr) {
        m_world.set<ScriptsModulePtr>({this});
        sandbox::modules::logs::trace(m_world, "ScriptsModule: Initializing Lua 5.4 State...");
        
        // Initialize the isolated Lua State
        m_state = luaL_newstate();
        if (!m_state) {
            sandbox::modules::logs::error(m_world, "ScriptsModule: Failed to initialize Lua State.");
            return;
        }

        // Open standard language libraries
        luaL_openlibs(m_state);

        // Dynamic Linking Initialization: Inject the engine functions and structs
        luaL_requiref(m_state, "spectre", luaopen_spectre, 1);
        lua_pop(m_state, 1); // Remove the module from the stack after loading

        // Register script components with Inherit so prefab instances automatically get them
        m_world.component<spectre_script_on_update_t>()
            .add(flecs::OnInstantiate, flecs::Inherit);
        m_world.component<spectre_script_on_create_t>()
            .add(flecs::OnInstantiate, flecs::Inherit);
        m_world.component<spectre_script_on_delete_t>()
            .add(flecs::OnInstantiate, flecs::Inherit);

        m_world.component<spectre_script_t>()
            .on_remove([](flecs::entity e, spectre_script_t& script) {
                if (script.function_name) {
                    ecs_os_free((void*)script.function_name);
                    script.function_name = nullptr;
                }
            });
        m_world.component<spectre_script_t>().add(flecs::OnInstantiate, flecs::Inherit);

        // Match entities with both script component and on_update tag
        m_world.system<const spectre_script_t, const spectre_script_on_update_t>("ScriptUpdateSystem")
            .kind(flecs::OnUpdate)
            .each([this](flecs::entity e, const spectre_script_t& script, const spectre_script_on_update_t&) {
                execute_script(script.function_name, e);
            });

        // Load all scripts from "app://state/scripts/"
        m_world.system("ScriptsStartupLoader")
            .kind(flecs::OnStart)
            .run([this](flecs::iter& /*iter*/) {
                load_script("app://state/scripts/player_movement.lua");
            });

        sandbox::modules::logs::info(m_world, "ScriptsModule: Ready.");
    }

    ScriptsModule::~ScriptsModule() {
        if (m_state) {
            lua_close(m_state);
            m_state = nullptr;
        }
    }

    void ScriptsModule::load_script(const char* path) {
        if (!m_state) return;

        try {
            // Check the path against the filesystem layer, load the text into memory
            std::string script_content = sandbox::modules::filesystem::read_all_text(m_world, path);
            
            // Evaluate it once to cache it in the compiler sandbox
            if (luaL_dostring(m_state, script_content.c_str()) != LUA_OK) {
                const char* error_msg = lua_tostring(m_state, -1);
                sandbox::modules::logs::error(m_world, "ScriptsModule: Error loading script '{}': {}", path, error_msg);
                lua_pop(m_state, 1); // pop error message
            } else {
                sandbox::modules::logs::info(m_world, "ScriptsModule: Loaded script '{}' successfully.", path);
            }
        } catch (const std::exception& e) {
            sandbox::modules::logs::error(m_world, "ScriptsModule: Failed to read script '{}': {}", path, e.what());
        }
    }

    void ScriptsModule::execute_script(const char* function_name, flecs::entity entity) {
        if (!m_state) return;

        lua_getglobal(m_state, function_name);
        if (!lua_isfunction(m_state, -1)) {
            lua_pop(m_state, 1);
            sandbox::modules::logs::error(m_world, "ScriptsModule: Function '{}' not found in Lua global scope.", function_name);
            return;
        }

        // Push world pointer (as lightuserdata) and entity ID (as integer) for SWIG-generated bindings
        lua_pushlightuserdata(m_state, (void*)m_world.c_ptr());
        lua_pushinteger(m_state, entity.id());

        if (lua_pcall(m_state, 2, 0, 0) != LUA_OK) {
            const char* error_msg = lua_tostring(m_state, -1);
            sandbox::modules::logs::error(m_world, "ScriptsModule: Error executing '{}': {}", function_name, error_msg);
            lua_pop(m_state, 1);
        }
    }

    flecs::entity ScriptsModule::create_script_entity(const char* function_name, spectre_script_trigger_t trigger) {
        flecs::entity e = m_world.entity();
        spectre_script_t script = {};
        script.target_entity = (uint32_t)e.id();
        if (function_name) {
            script.function_name = ecs_os_strdup(function_name);
        }
        e.set<spectre_script_t>(script);
        
        switch (trigger) {
            case SPECTRE_SCRIPT_TRIGGER_ON_ENTER: e.add<spectre_script_on_enter_t>(); break;
            case SPECTRE_SCRIPT_TRIGGER_ON_EXIT: e.add<spectre_script_on_exit_t>(); break;
            case SPECTRE_SCRIPT_TRIGGER_ON_CREATE: e.add<spectre_script_on_create_t>(); break;
            case SPECTRE_SCRIPT_TRIGGER_ON_DELETE: e.add<spectre_script_on_delete_t>(); break;
            case SPECTRE_SCRIPT_TRIGGER_ON_UPDATE: e.add<spectre_script_on_update_t>(); break;
            default: break;
        }
        return e;
    }

} // namespace spectre::module

// ─────────────────────────────────────────────────────────────────────────────
// C-ABI BRIDGE
// ─────────────────────────────────────────────────────────────────────────────

static spectre::module::ScriptsModule* get_scripts_module(ecs_world_t* ecs) {
    if (!ecs) return nullptr;
    if (const auto* wrapper = flecs::world(ecs).try_get<spectre::module::ScriptsModulePtr>()) {
        return wrapper->ptr;
    }
    return nullptr;
}

static ecs_entity_t abi_create_script_entity(ecs_world_t* ecs, const char* function_name, spectre_script_trigger_t trigger) {
    if (auto* module = get_scripts_module(ecs)) {
        return module->create_script_entity(function_name, trigger).id();
    }
    return 0;
}

static void abi_load_script(ecs_world_t* ecs, const char* path) {
    if (auto* module = get_scripts_module(ecs)) {
        module->load_script(path);
    }
}

static void abi_execute_script(ecs_world_t* ecs, const char* function_name, ecs_entity_t target_entity) {
    if (auto* module = get_scripts_module(ecs)) {
        module->execute_script(function_name, flecs::world(ecs).entity(target_entity));
    }
}

static spectre_scripts_api_t g_scripts_api = {
    .create_script_entity = abi_create_script_entity,
    .load_script = abi_load_script,
    .execute_script = abi_execute_script
};

extern "C" {
SANDBOX_DEFINE_SERVICE(spectre_scripts_service_t, spectre_scripts_api_t, &g_scripts_api)
}

namespace spectre::module {

    static sandbox_requirement_info_t scripts_requirements[] = {
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "logs",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "filesystem",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(ScriptsModule, {
        .name          = "scripts",
        .description   = "Lua Scripting module for execution and C-ABI interfacing.",
        .architecture  = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service       = &spectre_scripts_service_t_info,
        .requirements  = scripts_requirements,
        .requirement_count = 2
    })

}
