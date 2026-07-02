#pragma once
#include "flecs.h"

namespace spectre::module {

    class ScriptsModule {
    public:
        ScriptsModule(flecs::world& ecs); // load all script in app::/state/scripts
        ~ScriptsModule();

        void load_script(const char* path); // use filesystem for path and use default argument list that should be use as entrypoint function_name(world, entity, void* paylod) payload in pointer handle that can be anything
        void execute_script(const char* function_name, flecs::entity entity);

    private:
        flecs::world m_world;
        //lua_state m_state;
    };

}
