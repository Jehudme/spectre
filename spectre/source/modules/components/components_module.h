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

    // TODO: create generic component serializer/deserializer static function for dynamic components
    // TODO: create an pure c enum for all possible types and use it in the dynamic component registration function,
    // int, float, double, string, array of int, array of float, array of double, array of string, etc.
    // TODO: When registering and dynamic component, store the properties as an component of an entity in
    // m_components_properties_root.
    // TODO: then create an serializer with the generic component serializer/deserializer static function, when
    // deserializing use the properties, then save an copy in the component, so that it know how to serialize
    // TODO: so the generic component serializer/deserializer static function just have to check he is link to wich
    // TODO: create an flag spectre_component_dynamic_flag_t to mark the component as dynamic, and use it in the generic
    // component
    // TODO: Store the component in m_components_root like the other components, so that we can find it later
    void register_component(std::string_view name, sandbox::properties properties);

    flecs::entity find_component(std::string_view name) const;
    bool has_component(std::string_view name) const;
    bool is_component(flecs::entity entity) const;

    void import_configuration(std::string_view directory_path);
    void export_configuration(std::string_view directory_path);

  private:
    flecs::world m_world;
    flecs::entity m_components_root;
};

} // namespace spectre::modules
