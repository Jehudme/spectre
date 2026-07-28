sed -i '/std::vector<flecs::entity> scripts_module_t::list_scripts() const {/d' spectre/source/modules/scripts/scripts_module.cpp || true

sed -i '/void scripts_module_t::import_scripts/i \
std::vector<flecs::entity> scripts_module_t::list_scripts() const {\n    std::vector<flecs::entity> list;\n    m_scripts_root.children([&](flecs::entity e) {\n        list.push_back(e);\n    });\n    return list;\n}\n' spectre/source/modules/scripts/scripts_module.cpp

sed -i '/ecs_entity_t (\*find_script)(ecs_world_t\* world, const char\* name);/i \    ecs_entity_t* (*list_scripts)(ecs_world_t* world, size_t* count);' spectre/include/spectre/services/scripts_service.h

sed -i '/ecs_entity_t spectre_scripts_find_script/i \SANDBOX_API\necs_entity_t* spectre_scripts_list_scripts(ecs_world_t* world, size_t* count);\n' spectre/include/spectre/services/scripts_service.h

sed -i '/static ecs_entity_t find_script(const flecs::world& entity_world, const char\* name);/i \    static std::vector<flecs::entity> list_scripts(const flecs::world& entity_world);' spectre/include/spectre/sdk/scripts.hpp

sed -i '/\.is_script = scripts_is_script,/a \    .list_scripts = scripts_list_scripts,' spectre/source/modules/scripts/scripts_service.cpp

sed -i '/static ecs_entity_t scripts_find_script(ecs_world_t\* entity_world, const char\* name);/i \
static ecs_entity_t* scripts_list_scripts(ecs_world_t* entity_world, size_t* count);\n' spectre/source/modules/scripts/scripts_service.cpp

sed -i '/static ecs_entity_t scripts_find_script(ecs_world_t\* entity_world, const char\* name) {/i \
static ecs_entity_t* scripts_list_scripts(ecs_world_t* entity_world, size_t* count) {\n    static std::vector<ecs_entity_t> result;\n    result.clear();\n    *count = 0;\n    if (!entity_world) return nullptr;\n    flecs::world flecs_world(entity_world);\n    auto* module = flecs_world.lookup("spectre::modules::scripts_module_t").is_valid() ? flecs_world.try_get_mut<spectre::modules::scripts_module_t>() : nullptr;\n    if (module) {\n        auto entities = module->list_scripts();\n        for (auto& e : entities) result.push_back(e.id());\n    }\n    *count = result.size();\n    return result.empty() ? nullptr : result.data();\n}\n\necs_entity_t* spectre_scripts_list_scripts(ecs_world_t* world, size_t* count) {\n#ifdef __cplusplus\n    flecs::world flecs_world(world);\n    const spectre_scripts_service_t* service = flecs_world.try_get<spectre_scripts_service_t>();\n#else\n    const spectre_scripts_service_t* service = (const spectre_scripts_service_t*)ecs_get(world, ecs_id(spectre_scripts_service_t));\n#endif\n    if (service && service->api && service->api->list_scripts) {\n        return service->api->list_scripts(world, count);\n    }\n    *count = 0;\n    return nullptr;\n}\n' spectre/source/modules/scripts/scripts_service.cpp

sed -i '/void scripts::import_scripts(const flecs::world& entity_world, const char\* directory_path) {/i \
std::vector<flecs::entity> scripts::list_scripts(const flecs::world& entity_world) {\n    size_t count = 0;\n    ecs_entity_t* entities = spectre_scripts_list_scripts(entity_world.c_ptr(), &count);\n    std::vector<flecs::entity> list;\n    if (entities && count > 0) {\n        list.reserve(count);\n        for (size_t i = 0; i < count; ++i) {\n            list.push_back(entity_world.entity(entities[i]));\n        }\n    }\n    return list;\n}\n' spectre/source/modules/scripts/scripts_service.cpp

sed -i 's/void list_scripts() const;/std::vector<flecs::entity> list_scripts() const;/g' spectre/source/modules/scripts/scripts_module.h

