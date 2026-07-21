#pragma once
#include <functional>
#include <string_view>

#include "flecs.h"
#include "sandbox/sdk/detail/bootstrapper.inl"
#include "sandbox/sdk/properties.hpp"
#include "spectre/spectre.h"

namespace spectre::modules {
using ResourceLoader = spectre_resource_loader_component_t;
using Resource = spectre_resource_component_t;

class resource_module_t {
public:
  explicit resource_module_t(flecs::world &world);
  ~resource_module_t();

  resource_module_t(const resource_module_t &) = delete;
  resource_module_t &operator=(const resource_module_t &) = delete;
  resource_module_t(resource_module_t &&) = delete;
  resource_module_t &operator=(resource_module_t &&) = delete;

  void register_resource_loader(std::string_view type, ResourceLoader loader);
  void register_resource(const sandbox::properties &props);

  void deserialize_resource(flecs::entity resource_entity,
                            const sandbox::properties &props);
  sandbox::properties serialize_resource(flecs::entity entity);

  bool has_resource_loader(std::string_view type) const;
  bool has_resource(std::string_view name) const;
  bool is_resource(flecs::entity entity) const;

  flecs::entity find_resource_loader(std::string_view type);
  flecs::entity find_resource(std::string_view name);

  bool is_resource_loaded(flecs::entity resource) const;

  void load_resource(flecs::entity resourceEntity);
  void free_resource(flecs::entity resourceEntity);
  void *get_resource(flecs::entity resourceEntity);

private:
  flecs::world m_world;
  flecs::entity m_resources_root;
  flecs::entity m_loaders_root;
  flecs::entity m_resource_prefab;
  flecs::entity m_resources_serializer;
};

} // namespace spectre::modules
