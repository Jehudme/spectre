#include "spectre/services/scripts_service.h"
#include "scripts_module.h"

static sandbox_properties_handle_t scripts_serialize_script(ecs_world_t* entity_world, ecs_entity_t script);
static ecs_entity_t                scripts_deserialize_script(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static void                         scripts_register_script(ecs_world_t* entity_world, sandbox_properties_handle_t props);
static bool                         scripts_has_script(ecs_world_t* entity_world, const char* name);
static bool                         scripts_is_script(ecs_world_t* entity_world, ecs_entity_t entity);
static ecs_entity_t                scripts_find_script(ecs_world_t* entity_world, const char* name);
static void                         scripts_include_code(ecs_world_t* entity_world, const char* path);
static void                         scripts_execute_script(ecs_world_t* entity_world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count);

spectre_scripts_api_t g_scripts_api = {
    .serialize_script   = scripts_serialize_script,
    .deserialize_script = scripts_deserialize_script,
    .register_script    = scripts_register_script,
    .has_script         = scripts_has_script,
    .is_script          = scripts_is_script,
    .find_script        = scripts_find_script,
    .include_code       = scripts_include_code,
    .execute_script     = scripts_execute_script,
};

SANDBOX_DEFINE_SERVICE(spectre_scripts_service_t, spectre_scripts_api_t, &g_scripts_api)

static sandbox_properties_handle_t scripts_serialize_script(ecs_world_t* entity_world, ecs_entity_t script) {
    if (!entity_world) return {0};
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::script_module_t>();
    if (module) return module->serialize_script(flecs_world.entity(script)).get_raw();
    return {0};
}

static ecs_entity_t scripts_deserialize_script(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::script_module_t>();
    if (module) return module->deserialize_script(sandbox::properties(props)).id();
    return 0;
}

static void scripts_register_script(ecs_world_t* entity_world, sandbox_properties_handle_t props) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::script_module_t>();
    if (module) module->register_script(sandbox::properties(props));
}

static bool scripts_has_script(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::script_module_t>();
    if (module) return module->has_script(name);
    return false;
}

static bool scripts_is_script(ecs_world_t* entity_world, ecs_entity_t entity) {
    if (!entity_world) return false;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::script_module_t>();
    if (module) return module->is_script(flecs_world.entity(entity));
    return false;
}

static ecs_entity_t scripts_find_script(ecs_world_t* entity_world, const char* name) {
    if (!entity_world) return 0;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::script_module_t>();
    if (module) return module->find_script(name).id();
    return 0;
}

static void scripts_include_code(ecs_world_t* entity_world, const char* path) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::script_module_t>();
    if (module) module->include_code(path);
}

static void scripts_execute_script(ecs_world_t* entity_world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count) {
    if (!entity_world) return;
    flecs::world flecs_world(entity_world);
    auto* module = flecs_world.try_get_mut<spectre::modules::script_module_t>();
    if (module) {
        spectre::modules::script_arguments_t cpp_args(args, args + arg_count);
        module->execute_script(flecs_world.entity(script_entity), std::move(cpp_args));
    }
}
