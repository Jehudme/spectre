sed -i '/void prefabs_module_t::export_configuration/i \
std::vector<flecs::entity> prefabs_module_t::list_prefabs() const {\n    std::vector<flecs::entity> list;\n    m_prefabs_root.children([&](flecs::entity e) {\n        if (is_prefab(e)) {\n            list.push_back(e);\n        }\n    });\n    return list;\n}\n' spectre/source/modules/prefabs/prefabs_module.cpp

sed -i '/bool (\*is_prefab)(ecs_world_t\* world, ecs_entity_t entity);/a \    ecs_entity_t* (*list_prefabs)(ecs_world_t* world, size_t* count);' spectre/include/spectre/services/prefabs_service.h

sed -i '/bool spectre_prefabs_is_prefab(ecs_world_t\* world, ecs_entity_t entity);/a \SANDBOX_API\necs_entity_t* spectre_prefabs_list_prefabs(ecs_world_t* world, size_t* count);' spectre/include/spectre/services/prefabs_service.h

sed -i '/static bool is_prefab(const flecs::world& entity_world, flecs::entity entity);/a \    static std::vector<flecs::entity> list_prefabs(const flecs::world& entity_world);' spectre/include/spectre/sdk/prefabs.hpp

sed -i '/\.is_prefab = prefabs_is_prefab,/a \    .list_prefabs = prefabs_list_prefabs,' spectre/source/modules/prefabs/prefabs_service.cpp

sed -i '/static ecs_entity_t prefabs_find_prefab/i \
static ecs_entity_t* prefabs_list_prefabs(ecs_world_t* entity_world, size_t* count) {\n    static std::vector<ecs_entity_t> result;\n    result.clear();\n    *count = 0;\n    if (!entity_world) return nullptr;\n    flecs::world flecs_world(entity_world);\n    auto* module = flecs_world.lookup("spectre::modules::prefabs_module_t").is_valid() ? flecs_world.try_get_mut<spectre::modules::prefabs_module_t>() : nullptr;\n    if (module) {\n        auto entities = module->list_prefabs();\n        for (auto& e : entities) result.push_back(e.id());\n    }\n    *count = result.size();\n    return result.empty() ? nullptr : result.data();\n}\n\necs_entity_t* spectre_prefabs_list_prefabs(ecs_world_t* world, size_t* count) {\n#ifdef __cplusplus\n    flecs::world flecs_world(world);\n    const spectre_prefabs_service_t* service = flecs_world.try_get<spectre_prefabs_service_t>();\n#else\n    const spectre_prefabs_service_t* service = (const spectre_prefabs_service_t*)ecs_get(world, ecs_id(spectre_prefabs_service_t));\n#endif\n    if (service && service->api && service->api->list_prefabs) {\n        return service->api->list_prefabs(world, count);\n    }\n    *count = 0;\n    return nullptr;\n}\n' spectre/source/modules/prefabs/prefabs_service.cpp

sed -i '/void prefabs::export_configuration(const flecs::world& entity_world, const char\* path) {/i \
std::vector<flecs::entity> prefabs::list_prefabs(const flecs::world& entity_world) {\n    size_t count = 0;\n    ecs_entity_t* entities = spectre_prefabs_list_prefabs(entity_world.c_ptr(), &count);\n    std::vector<flecs::entity> list;\n    if (entities && count > 0) {\n        list.reserve(count);\n        for (size_t i = 0; i < count; ++i) {\n            list.push_back(entity_world.entity(entities[i]));\n        }\n    }\n    return list;\n}\n' spectre/source/modules/prefabs/prefabs_service.cpp

