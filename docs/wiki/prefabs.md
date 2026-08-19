# Prefabs Module

## Module Overview

The `prefabs` module provides an entity serialization and templating system for the Spectre Game Engine, built on top of Flecs ECS and the generic property system (`sandbox::properties`). It allows developers to define, register, serialize, deserialize, and instantiate complex entity hierarchies using structured data formats like JSON. 

A "prefab" acts as a reusable blueprint for an entity, encapsulating its components, scripts, child hierarchies, and relations. Prefabs can be registered into the engine at runtime, queried, and used to spawn new instances dynamically. This is a critical backbone for data-driven game design, allowing the Spectre Editor and gameplay scripts to manage scenes and entities abstractly.

## Table of Contents

1. [C++ API Reference](#c-api-reference)
   - [serialize_entity](#serialize_entity)
   - [deserialize_entity](#deserialize_entity)
   - [register_prefab](#register_prefab)
   - [has_prefab](#has_prefab)
   - [is_prefab](#is_prefab)
   - [find_prefab](#find_prefab)
   - [list_prefabs](#list_prefabs)
   - [create_entity_from_props](#create_entity_from_props)
   - [create_entity_from_prefab](#create_entity_from_prefab)
   - [create_entity_from_name](#create_entity_from_name)
   - [import_configuration](#import_configuration)
   - [export_configuration](#export_configuration)
2. [Serialization Format](#serialization-format)
3. [Lua API / Editor Interaction](#lua-api--editor-interaction)
4. [Warnings / Cautions / Best Practices](#warnings--cautions--best-practices)

---

## C++ API Reference

The primary C++ interface is exposed via the `spectre::modules::prefabs` SDK wrapper. This interacts with the underlying `spectre_prefabs_service_t` C service.

Make sure to include `#include <spectre/sdk/prefabs.hpp>` to use this API.

### `serialize_entity`
```cpp
static sandbox_properties_handle_t serialize_entity(const flecs::world& entity_world, ecs_entity_t entity);
```
**Description**: Serializes an existing entity (and its entire hierarchy/components) into a property map tree.
**Arguments**:
- `entity_world`: The current Flecs ECS world.
- `entity`: The entity ID to serialize.
**Returns**: A raw handle to the generated `sandbox::properties`. *Warning*: You must manage the lifecycle of this handle or wrap it in a `sandbox::properties` object.

### `deserialize_entity`
```cpp
static void deserialize_entity(const flecs::world& entity_world, ecs_entity_t target, sandbox_properties_handle_t props);
```
**Description**: Populates an existing entity using data from a property map.
**Arguments**:
- `entity_world`: The current Flecs ECS world.
- `target`: The entity to apply the properties to.
- `props`: The properties tree containing component/hierarchy definitions.

### `register_prefab`
```cpp
static void register_prefab(const flecs::world& entity_world, const char* name, sandbox_properties_handle_t props);
```
**Description**: Registers a new prefab blueprint from a properties map. Internally parses the properties, creates a Flecs prefab entity, and applies the components.
**Arguments**:
- `entity_world`: The current Flecs ECS world.
- `name`: The unique string name to assign to the prefab.
- `props`: The properties tree defining the prefab layout.

### `has_prefab`
```cpp
static bool has_prefab(const flecs::world& entity_world, const char* name);
```
**Description**: Checks if a prefab with the given name is currently registered.

### `is_prefab`
```cpp
static bool is_prefab(const flecs::world& entity_world, ecs_entity_t entity);
```
**Description**: Returns `true` if the specific entity handle represents a blueprint/prefab entity (as opposed to an instantiated runtime entity).

### `find_prefab`
```cpp
static ecs_entity_t find_prefab(const flecs::world& entity_world, const char* name);
```
**Description**: Looks up and returns the entity ID of a registered prefab by its string name. Returns 0 if not found.

### `list_prefabs`
```cpp
static std::vector<flecs::entity> list_prefabs(const flecs::world& entity_world);
```
**Description**: Returns a standard vector containing `flecs::entity` handles to all registered prefabs in the world.

### `create_entity_from_props`
```cpp
static ecs_entity_t create_entity_from_props(const flecs::world& entity_world, sandbox_properties_handle_t props);
```
**Description**: Spawns an entirely new entity and immediately deserializes the provided property tree onto it.

### `create_entity_from_prefab`
```cpp
static ecs_entity_t create_entity_from_prefab(const flecs::world& entity_world, ecs_entity_t prefab);
```
**Description**: Spawns a new entity that leverages an existing prefab as a template (`flecs::IsA` relationship).

### `create_entity_from_name`
```cpp
static ecs_entity_t create_entity_from_name(const flecs::world& entity_world, const char* name);
```
**Description**: Convenience function to lookup a prefab by name and instantiate it.

### `import_configuration`
```cpp
static void import_configuration(const flecs::world& entity_world, const char* directory_path);
```
**Description**: Bulk loads and registers prefabs from a target folder path (typically JSON files).

### `export_configuration`
```cpp
static void export_configuration(const flecs::world& entity_world, const char* directory_path);
```
**Description**: Dumps all currently registered prefabs into the target directory, storing them as serialized files.

---

## Serialization Format

The property tree structure (often mapped to JSON for storage) generally looks like this:

```json
{
    "name": "MyPrefabName",
    "prefabs": [ "BasePrefabName" ],
    "components": {
        "spectre_2D_transform_component_t": {
            "x": 100.0,
            "y": 50.0
        },
        "scripts": {
            "on_create": {
                "0": {
                    "function": "my_lua_callback",
                    "arguments": { "value": 42.0 }
                }
            }
        }
    },
    "children": {
        "ChildEntityName": {
            "components": {
                "spectre_2D_transform_component_t": {
                    "x": 0.0,
                    "y": 0.0
                }
            }
        }
    }
}
```

- `name`: Used primarily when the property represents a scene entity, or internally to name the entity.
- `prefabs`: Array of strings pointing to parent prefabs this entity inherits from.
- `components`: Dictionary of component names mapping to their serialized fields. Component serialization logic depends on the specific component's `spectre_serializer_component` registration.
- `children`: Recursively defines nested entities.
- `scripts`: A special section mapped to script relationships (e.g., `spectre_use_script_on_create_relation_t`), parsing out function targets and arguments natively into ECS relations.

---

## Lua API / Editor Interaction

In Lua and the Spectre Editor, prefabs are heavily integrated with file I/O and properties logic. Prefabs typically live in `"project://scenes/prefabs/MyPrefab.json"`. 

You can read or instantiate them by loading the file content into a `sandbox.Properties` object:
```lua
local sandbox = require("sandbox")

-- Loading a property
local props = sandbox.Properties.new()
props:load(json_content_from_disk, 0)

-- Example editor structure format:
-- {"entities": {"PrefabName": { "components": { ... }}}}
```

The Spectre Editor `prefabs.lua` interface leverages raw property accessors (`props:set_string`, `props:clear`, `props:has`) to visually construct prefab properties and writes them immediately to disk using asynchronous `WriteFileAction`. The actual instantiation into ECS at runtime happens via the C++ backend or C-ABI.

---

## Warnings / Cautions / Best Practices

> [!WARNING]
> **Property Handles**: Many C++ API methods return or accept a `sandbox_properties_handle_t`. If an API creates a property map (like `serialize_entity`), **you are responsible for freeing it** or wrapping it in the C++ `sandbox::properties` wrapper class which handles RAII destruction. Otherwise, memory leaks will occur.

> [!CAUTION]
> **Orphaned Hierarchy References**: When deleting or replacing entities inside a prefab's children hierarchy, be sure to keep the property tree perfectly synced before invoking `export_configuration`. Corrupted trees can lead to failure when deserializing later.

> [!TIP]
> **Component Serializers**: The Prefabs module cannot auto-serialize native C++ structs. A component MUST have a `spectre_serializer_component` registered with the `serializer_module` for it to be safely written to or read from a property map. If a component disappears across a save/load cycle, check its serializer!

> [!IMPORTANT]
> **Flecs IsA Semantics**: `create_entity_from_prefab` uses Flecs' native `IsA` relationship feature. Overriding a component on an instance overrides it just for that instance, while sharing non-overridden components directly from the prefab. This limits memory footprint but requires you to understand Flecs instancing behavior when modifying shared component data.
