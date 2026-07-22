#pragma once
#include "spectre/components/serializer_component.h"
#include "spectre/services/components_service.h"
#include <flecs.h>
#include <string>

namespace spectre::modules {
class components_module_t {
  public:
    explicit components_module_t(flecs::world& world);
    ~components_module_t();

    components_module_t(const components_module_t&) = delete;
    components_module_t& operator=(const components_module_t&) = delete;

    // NOTE: the compoenent name is the same as the serializer name
    void register_component(std::string_view name, spectre_component_registration_fn_t registration_fn,
                            spectre_serializer_component serializer);

    flecs::entity find_component(std::string_view name) const;
    bool has_component(std::string_view name) const;
    bool is_component(flecs::entity entity) const;

  private:
    flecs::world m_world;
    flecs::entity m_components_root;
};

} // namespace spectre::modules
