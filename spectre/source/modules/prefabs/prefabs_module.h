#pragma once
#include "flecs/addons/cpp/entity.hpp"
#include "sandbox/sdk/detail/bootstrapper.inl"
#include "sandbox/sdk/properties.hpp"
#include <flecs.h>

namespace spectre::modules {
class prefabs_module_t {
  public:
    explicit prefabs_module_t(flecs::world& world);
    ~prefabs_module_t();

    prefabs_module_t(const prefabs_module_t&) = delete;
    prefabs_module_t& operator=(const prefabs_module_t&) = delete;
    prefabs_module_t(prefabs_module_t&&) = delete;
    prefabs_module_t& operator=(prefabs_module_t&&) = delete;

    sandbox::properties serialize_entity(flecs::entity entity);

    flecs::entity deserialize_entity(flecs::entity target_entity, sandbox::properties properties);

    void register_prefab(std::string_view name, sandbox::properties props);
    bool has_prefab(std::string_view name) const;
    bool is_prefab(flecs::entity entity) const;
    flecs::entity find_prefab(std::string_view name);

    std::vector<flecs::entity> list_prefabs() const;

    flecs::entity create_entity(sandbox::properties props);
    flecs::entity create_entity(flecs::entity prefab);
    flecs::entity create_entity(std::string_view name);

    void import_configuration(std::string_view directory_path);

    void export_configuration(std::string_view directory_path);

  private:
    flecs::world m_world;
    flecs::entity m_prefabs_root;
    flecs::entity m_entity_prefab;
    flecs::entity m_entity_serializer;
    flecs::entity m_script_args_serializer;
};
} // namespace spectre::modules
