#include "spectre/services/scripts_service.h"
#include "scripts_module.h"

static sandbox_properties_handle_t scripts_serialize_script(ecs_world_t* world, ecs_entity_t script);
static ecs_entity_t                scripts_deserialize_script(ecs_world_t* world, sandbox_properties_handle_t props);
static void                         scripts_register_script(ecs_world_t* world, sandbox_properties_handle_t props);
static bool                         scripts_has_script(ecs_world_t* world, const char* name);
static bool                         scripts_is_script(ecs_world_t* world, ecs_entity_t entity);
static ecs_entity_t                scripts_find_script(ecs_world_t* world, const char* name);
static void                         scripts_include_code(ecs_world_t* world, const char* path);
static void                         scripts_execute_script(ecs_world_t* world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count);

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

static sandbox_properties_handle_t scripts_serialize_script(ecs_world_t* world, ecs_entity_t script) {
    if (!world) return {0};
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::script_module_t>();
    if (m) return m->serialize_script(w.entity(script)).get_raw();
    return {0};
}

static ecs_entity_t scripts_deserialize_script(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::script_module_t>();
    if (m) return m->deserialize_script(sandbox::properties(props)).id();
    return 0;
}

static void scripts_register_script(ecs_world_t* world, sandbox_properties_handle_t props) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::script_module_t>();
    if (m) m->register_script(sandbox::properties(props));
}

static bool scripts_has_script(ecs_world_t* world, const char* name) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::script_module_t>();
    if (m) return m->has_script(name);
    return false;
}

static bool scripts_is_script(ecs_world_t* world, ecs_entity_t entity) {
    if (!world) return false;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::script_module_t>();
    if (m) return m->is_script(w.entity(entity));
    return false;
}

static ecs_entity_t scripts_find_script(ecs_world_t* world, const char* name) {
    if (!world) return 0;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::script_module_t>();
    if (m) return m->find_script(name).id();
    return 0;
}

static void scripts_include_code(ecs_world_t* world, const char* path) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::script_module_t>();
    if (m) m->include_code(path);
}

static void scripts_execute_script(ecs_world_t* world, ecs_entity_t script_entity, const spectre_script_argument_t* args, size_t arg_count) {
    if (!world) return;
    flecs::world w(world);
    auto* m = w.try_get_mut<spectre::modules::script_module_t>();
    if (m) {
        spectre::modules::script_arguments_t cpp_args(args, args + arg_count);
        m->execute_script(w.entity(script_entity), std::move(cpp_args));
    }
}
