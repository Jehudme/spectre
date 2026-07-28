sed -i '/void window_module_t::export_configuration/d' spectre/source/modules/window/window_module.cpp || true
sed -i '/void window_module_t::import_configuration/d' spectre/source/modules/window/window_module.cpp || true

cat << 'EOF2' >> spectre/source/modules/window/window_module.cpp

void window_module_t::import_configuration(std::string_view file_path) {
    if (sandbox::modules::filesystem::exists(m_world, std::string(file_path).c_str())) {
        std::string content = sandbox::modules::filesystem::read_all_text(m_world, std::string(file_path).c_str());
        sandbox::properties props(content, sandbox::properties::Format::JSON);
        register_window(props);
    } else {
        sandbox::modules::logs::warn(m_world, "[Window Module] Window configuration missing at {}", file_path);
    }
}
void window_module_t::export_configuration(std::string_view file_path) {
    sandbox::properties props = serialize_window(m_window_entity);
    std::string content = props.dump(sandbox::properties::Format::JSON);
    sandbox::modules::filesystem::write_all(m_world, std::string(file_path).c_str(), content.c_str(), content.size(), true);
}
EOF2

sed -i '/ecs_entity_t (\*get_window_entity)(ecs_world_t\* world);/a \    void (*import_configuration)(ecs_world_t* world, const char* file_path);\n    void (*export_configuration)(ecs_world_t* world, const char* file_path);' spectre/include/spectre/services/window_service.h

sed -i '/ecs_entity_t spectre_window_get_window_entity(ecs_world_t\* world);/a \SANDBOX_API\nvoid spectre_window_import_configuration(ecs_world_t* world, const char* file_path);\nSANDBOX_API\nvoid spectre_window_export_configuration(ecs_world_t* world, const char* file_path);\n' spectre/include/spectre/services/window_service.h

sed -i '/static ecs_entity_t get_window_entity(const flecs::world& entity_world);/a \    static void import_configuration(const flecs::world& entity_world, const char* file_path);\n    static void export_configuration(const flecs::world& entity_world, const char* file_path);' spectre/include/spectre/sdk/window.hpp

sed -i '/\.get_window_entity = window_get_window_entity,/a \    .import_configuration = window_import_configuration,\n    .export_configuration = window_export_configuration,' spectre/source/modules/window/window_service.cpp

sed -i '/static ecs_entity_t window_get_window_entity(ecs_world_t\* entity_world);/a \
static void window_import_configuration(ecs_world_t* entity_world, const char* file_path);\nstatic void window_export_configuration(ecs_world_t* entity_world, const char* file_path);\n' spectre/source/modules/window/window_service.cpp

sed -i '/static ecs_entity_t window_get_window_entity(ecs_world_t\* entity_world) {/i \
static void window_import_configuration(ecs_world_t* entity_world, const char* file_path) {\n    if (!entity_world) return;\n    flecs::world flecs_world(entity_world);\n    auto* module = flecs_world.lookup("spectre::modules::window_module_t").is_valid() ? flecs_world.try_get_mut<spectre::modules::window_module_t>() : nullptr;\n    if (module) module->import_configuration(file_path);\n}\n\nstatic void window_export_configuration(ecs_world_t* entity_world, const char* file_path) {\n    if (!entity_world) return;\n    flecs::world flecs_world(entity_world);\n    auto* module = flecs_world.lookup("spectre::modules::window_module_t").is_valid() ? flecs_world.try_get_mut<spectre::modules::window_module_t>() : nullptr;\n    if (module) module->export_configuration(file_path);\n}\n\nvoid spectre_window_import_configuration(ecs_world_t* world, const char* file_path) {\n#ifdef __cplusplus\n    flecs::world flecs_world(world);\n    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();\n#else\n    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_get(world, ecs_id(spectre_window_service_t));\n#endif\n    if (service && service->api && service->api->import_configuration) {\n        service->api->import_configuration(world, file_path);\n    }\n}\n\nvoid spectre_window_export_configuration(ecs_world_t* world, const char* file_path) {\n#ifdef __cplusplus\n    flecs::world flecs_world(world);\n    const spectre_window_service_t* service = flecs_world.try_get<spectre_window_service_t>();\n#else\n    const spectre_window_service_t* service = (const spectre_window_service_t*)ecs_get(world, ecs_id(spectre_window_service_t));\n#endif\n    if (service && service->api && service->api->export_configuration) {\n        service->api->export_configuration(world, file_path);\n    }\n}\n' spectre/source/modules/window/window_service.cpp

sed -i '/ecs_entity_t window::get_window_entity(const flecs::world& entity_world) {/i \
void window::import_configuration(const flecs::world& entity_world, const char* file_path) {\n    spectre_window_import_configuration(entity_world.c_ptr(), file_path);\n}\n\nvoid window::export_configuration(const flecs::world& entity_world, const char* file_path) {\n    spectre_window_export_configuration(entity_world.c_ptr(), file_path);\n}\n' spectre/source/modules/window/window_service.cpp

