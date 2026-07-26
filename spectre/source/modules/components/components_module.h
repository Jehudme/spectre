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

    // NOTE: the compoenent name is the same as the serializer name
    void register_component(std::string_view name, spectre_component_registration_fn_t registration_fn,
                            spectre_serializer_component serializer);

    // TODO: move this in components_service.h
    typedef enum spectre_dynamic_type_t {
        SPECTRE_DYNAMIC_TYPE_UNKNOWN,
        SPECTRE_DYNAMIC_TYPE_INT,
        SPECTRE_DYNAMIC_TYPE_FLOAT,
        SPECTRE_DYNAMIC_TYPE_DOUBLE,
        SPECTRE_DYNAMIC_TYPE_BOOL,
        SPECTRE_DYNAMIC_TYPE_STRING,
        SPECTRE_DYNAMIC_TYPE_ARRAY_INT,
        SPECTRE_DYNAMIC_TYPE_ARRAY_FLOAT,
        SPECTRE_DYNAMIC_TYPE_ARRAY_DOUBLE,
        SPECTRE_DYNAMIC_TYPE_ARRAY_STRING
    } spectre_dynamic_type_t;

    struct spectre_component_dynamic_flag_t {
        char dummy;
    };

    void register_component(std::string_view name, sandbox::properties properties);

    flecs::entity find_component(std::string_view name) const;
    bool has_component(std::string_view name) const;
    bool is_component(flecs::entity entity) const;

    // TODO: Implement this function to return a list of all registered components.
    // TODO: Make an service function, an sdk function and an lua script wrapper function.
    void list_components() const;

    // TODO: Make an service function, an sdk function and an lua script wrapper function.
    void import_configuration(std::string_view directory_path);

    // TODO: Make an service function, an sdk function and an lua script wrapper function.
    void export_configuration(std::string_view directory_path);

  private:
    flecs::world m_world;
    flecs::entity m_components_root;
};

} // namespace spectre::modules
