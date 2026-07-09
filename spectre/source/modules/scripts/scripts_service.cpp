#include "spectre/services/scripts_service.h"
#include "scripts_module.h"
#include <flecs.h>
#include <vector>
#include <string>

static bool scripts_has_script(ecs_world_t* world, const char* function_name, const spectre_script_argument_type_t* arg_types, size_t arg_count) {
    if (!world || !function_name) return false;
    flecs::world fw(world);
    auto* module = fw.try_get_mut<spectre::modules::script_module_t>();
    if (!module) return false;
    
    std::vector<spectre_script_argument_type_t> types(arg_types, arg_types + arg_count);
    return module->has_script(function_name, types);
}

static bool scripts_is_script(ecs_world_t* world, ecs_entity_t entity) {
    if (!world || !entity) return false;
    flecs::world fw(world);
    auto* module = fw.try_get_mut<spectre::modules::script_module_t>();
    if (!module) return false;
    return module->is_script(fw.entity(entity));
}

static ecs_entity_t scripts_find_script(ecs_world_t* world, const char* function_name) {
    if (!world || !function_name) return 0;
    flecs::world fw(world);
    auto* module = fw.try_get_mut<spectre::modules::script_module_t>();
    if (!module) return 0;
    return module->find_script(function_name).id();
}

static void scripts_include_code(ecs_world_t* world, const char* path) {
    if (!world || !path) return;
    flecs::world fw(world);
    auto* module = fw.try_get_mut<spectre::modules::script_module_t>();
    if (module) module->include_code(path);
}

static void scripts_execute_script(ecs_world_t* world, const char* function_name, spectre_script_argument_t* args, size_t arg_count) {
    if (!world || !function_name) return;
    flecs::world fw(world);
    auto* module = fw.try_get_mut<spectre::modules::script_module_t>();
    if (module) {
        spectre::modules::script_arguments_t v_args(args, args + arg_count);
        module->execute_script(function_name, v_args);
    }
}

static spectre_scripts_api_t api = {
    scripts_has_script,
    scripts_is_script,
    scripts_find_script,
    scripts_include_code,
    scripts_execute_script
};

SANDBOX_DEFINE_SERVICE(spectre_scripts_service_t, spectre_scripts_api_t, &api)

bool spectre_scripts_has_script(ecs_world_t* world, const char* function_name, const spectre_script_argument_type_t* arg_types, size_t arg_count) {
    return scripts_has_script(world, function_name, arg_types, arg_count);
}
bool spectre_scripts_is_script(ecs_world_t* world, ecs_entity_t entity) {
    return scripts_is_script(world, entity);
}
ecs_entity_t spectre_scripts_find_script(ecs_world_t* world, const char* function_name) {
    return scripts_find_script(world, function_name);
}
void spectre_scripts_include_code(ecs_world_t* world, const char* path) {
    scripts_include_code(world, path);
}
void spectre_scripts_execute_script(ecs_world_t* world, const char* function_name, spectre_script_argument_t* args, size_t arg_count) {
    scripts_execute_script(world, function_name, args, arg_count);
}

