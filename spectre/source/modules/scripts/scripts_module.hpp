#pragma once
#include <string>

#include "flecs.h"

struct lua_State; // Clean forward declaration to keep compile times fast

namespace spectre::module {

    struct Script { //
        const char* function_name;
    };

    class ScriptsModule {
    public:
        ScriptsModule(flecs::world& ecs); // Load all scripts from "app://state/scripts"
        ~ScriptsModule();

        void load_script(const char* path); // Use filesystem virtual paths; default entry point signature: function_name(world, entity, payload)
        void execute_script(const char* function_name, flecs::entity entity);

    private:
        flecs::world m_world;
        lua_State* m_state; // Fixed pointer type signature
    };
}
