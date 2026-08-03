#pragma once
#include "sandbox/sdk/properties.hpp"
#include "spectre/components/serializer_component.h"
#include "spectre/services/components_service.h"
#include <flecs.h>
#include <string>
#include <unordered_map>

namespace spectre::modules {
class components_module_t {
  public:
    explicit components_module_t(flecs::world& world);
    ~components_module_t();

    components_module_t(const components_module_t&) = delete;
    components_module_t& operator=(const components_module_t&) = delete;

    // TODO: In
    void register_component(std::string_view name, spectre_component_registration_fn_t registration_fn,
                            spectre_serializer_component serializer);

    bool is_static(std::string_view name) const;
    sandbox::properties find_schema(std::string_view name) const;

    struct spectre_component_dynamic_flag_t {
        char dummy;
    };

    void register_component(std::string_view name, sandbox::properties properties);

    flecs::entity find_component(std::string_view name) const;
    bool has_component(std::string_view name) const;
    bool is_component(flecs::entity entity) const;
    std::vector<flecs::entity> list_components() const;

    void import_configuration(std::string_view directory_path);
    void export_configuration(std::string_view directory_path);

  private:
    flecs::world m_world;
    flecs::entity m_components_root;
};

} // namespace spectre::modules
