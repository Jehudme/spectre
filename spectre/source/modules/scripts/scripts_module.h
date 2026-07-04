#pragma once
#include <string>
#include "flecs.h"

#include <spectre/abi/scripts_components.h>
#include <spectre/abi/scripts_service.h>

struct lua_State; // Clean forward declaration to keep compile times fast

namespace spectre::module {

    class ScriptsModule {
    public:
        ScriptsModule(flecs::world& ecs);
        ~ScriptsModule();

        flecs::entity create_script_entity(const char* function_name, spectre_script_trigger_t trigger = SPECTRE_SCRIPT_TRIGGER_UNDEFINED);

        void load_script(const char* path);
        void execute_script(const char* function_name, flecs::entity entity);

    private:
        flecs::world m_world;
        lua_State* m_state;
    };
}
