#include "components_module.h"
#include <sandbox/sdk/logs.hpp>
#include <spectre/sdk/serializer.hpp>

namespace spectre::modules {

static sandbox_requirement_info_t components_requirements[] = {{.kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
                                                                .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
                                                                .name = "logs",
                                                                .architecture = "sandbox",
                                                                .version_major = 1,
                                                                .version_minor = 0,
                                                                .version_patch = -1}};

SANDBOX_DECLARE_MODULE(components_module_t, {.name = "components",
                                             .description = "Components Module",
                                             .architecture = "spectre",
                                             .version_major = 1,
                                             .version_minor = 0,
                                             .version_patch = 0,
                                             .service = &spectre_components_service_t_info,
                                             .requirements = components_requirements,
                                             .requirement_count = 1})

components_module_t::components_module_t(flecs::world& world) : m_world(world) {
    m_components_root = m_world.entity("components");
    sandbox::modules::logs::trace(m_world, "[Components Module] Initializing...");
    
    sandbox::modules::logs::info(m_world, "[Components Module] Initialized successfully.");
}

components_module_t::~components_module_t() {
}

void components_module_t::register_component(std::string_view name, spectre_component_registration_fn_t registration_fn, spectre_serializer_component serializer) {
    if (name.empty()) {
        sandbox::modules::logs::error(m_world, "[Components Module] Cannot register a component with an empty name.");
        return;
    }

    if (has_component(name)) {
        sandbox::modules::logs::trace(m_world, "[Components Module] Component '{}' is already registered. Overriding.", name.data());
    }

    ecs_entity_t comp_id = registration_fn(m_world.c_ptr());
    flecs::entity comp(m_world, comp_id);
    comp.child_of(m_components_root);
    comp.set<spectre_serializer_component>(serializer);
    
    spectre::modules::serializer::register_serializer(m_world, name.data(), &serializer);

    sandbox::modules::logs::trace(m_world, "[Components Module] Registered component '{}'.", name.data());
}

flecs::entity components_module_t::find_component(std::string_view name) const {
    if (name.empty()) return flecs::entity::null();
    flecs::entity comp = m_components_root.lookup(name.data());
    if (!comp.is_valid()) {
        sandbox::modules::logs::error(m_world, "[Components Module] Could not find component '{}'.", name.data());
    }
    return comp;
}

bool components_module_t::has_component(std::string_view name) const {
    if (name.empty()) return false;
    return m_components_root.lookup(name.data()).is_valid();
}

bool components_module_t::is_component(flecs::entity entity) const {
    return entity.has(flecs::ChildOf, m_components_root);
}

} // namespace spectre::modules
