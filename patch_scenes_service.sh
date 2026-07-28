sed -i '/static ecs_entity_t scenes_find_current_state/i \
static ecs_entity_t* scenes_list_states(ecs_world_t* entity_world, size_t* count) {\n    static std::vector<ecs_entity_t> result;\n    result.clear();\n    *count = 0;\n    if (!entity_world) return nullptr;\n    flecs::world flecs_world(entity_world);\n    auto* module = flecs_world.lookup("spectre::modules::scenes_module_t").is_valid() ? flecs_world.try_get_mut<spectre::modules::scenes_module_t>() : nullptr;\n    if (module) {\n        auto entities = module->list_states();\n        for (auto& e : entities) result.push_back(e.id());\n    }\n    *count = result.size();\n    return result.empty() ? nullptr : result.data();\n}\n' spectre/source/modules/scenes/scenes_service.cpp

sed -i '/.is_scene = scenes_is_scene,/a \    .list_states = scenes_list_states,' spectre/source/modules/scenes/scenes_service.cpp

sed -i '/ecs_entity_t spectre_scenes_find_current_state(ecs_world_t\* world) {/i \
ecs_entity_t* spectre_scenes_list_states(ecs_world_t* world, size_t* count) {\n#ifdef __cplusplus\n    flecs::world flecs_world(world);\n    const spectre_scenes_service_t* service = flecs_world.try_get<spectre_scenes_service_t>();\n#else\n    const spectre_scenes_service_t* service = (const spectre_scenes_service_t*)ecs_get(world, ecs_id(spectre_scenes_service_t));\n#endif\n    if (service && service->api->list_states) {\n        return service->api->list_states(world, count);\n    }\n    *count = 0;\n    return nullptr;\n}\n' spectre/source/modules/scenes/scenes_service.cpp

cat << 'EOF2' >> spectre/source/modules/scenes/scenes_service.cpp

std::vector<flecs::entity> scenes::list_states(const flecs::world& entity_world) {
    size_t count = 0;
    ecs_entity_t* entities = spectre_scenes_list_states(entity_world.c_ptr(), &count);
    std::vector<flecs::entity> list;
    if (entities && count > 0) {
        list.reserve(count);
        for (size_t i = 0; i < count; ++i) {
            list.push_back(entity_world.entity(entities[i]));
        }
    }
    return list;
}
EOF2
