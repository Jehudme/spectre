# Components Module

## Module Overview
The `components` module is a core part of the Spectre Game Engine, responsible for managing the registry and lifecycle of both static (C++) and dynamic (Lua/scripted) components within the engine's Entity Component System (ECS). 

It acts as an abstraction layer over Flecs, giving developers the ability to:
- Register C++ structures as static components with custom serializers.
- Register dynamic components at runtime via Data-Driven schemas (`sandbox::properties`), heavily utilized by scripting and editor tools.
- Query and retrieve component definitions dynamically by name.
- Import and export component configurations to and from disk.
- Differentiate between statically defined and dynamically created components.

## Table of Contents
- [Module Overview](#module-overview)
- [C++ API Reference](#c-api-reference)
  - [Component Registration](#component-registration)
  - [Component Querying](#component-querying)
  - [Configuration Management](#configuration-management)
- [Lua API Reference](#lua-api-reference)
  - [Component Registration (Lua)](#component-registration-lua)
  - [Component Querying (Lua)](#component-querying-lua)
- [Best Practices & Warnings](#best-practices--warnings)

## C++ API Reference

The C++ SDK for components is available under the `spectre::modules::components` namespace, defined in `<spectre/sdk/components.hpp>`.

### Component Registration

#### `register_component` (Static)
Registers a C++ static component with the engine.
```cpp
static void register_component(
    const flecs::world& entity_world, 
    const char* name,
    spectre_component_registration_fn_t registration_fn,
    spectre_serializer_component serializer = {}
);
```
- **`entity_world`**: The Flecs world instance.
- **`name`**: Unique string identifier for the component.
- **`registration_fn`**: A function pointer that registers the component with Flecs and returns its ECS entity ID (`ecs_entity_t (*)(ecs_world_t*)`).
- **`serializer`**: Optional struct containing serialization/deserialization callbacks.

#### `register_component` (Dynamic)
Registers a dynamic component at runtime using a property schema. These are particularly useful for components defined via Lua scripts or data files.
```cpp
static void register_component(
    const flecs::world& entity_world, 
    const char* name, 
    sandbox::properties properties
);
```
- **`entity_world`**: The Flecs world instance.
- **`name`**: Unique string identifier for the dynamic component.
- **`properties`**: The schema defining the structure of the dynamic component.

### Component Querying

#### `find_component`
Finds a component's entity ID by its registered name.
```cpp
static ecs_entity_t find_component(const flecs::world& entity_world, const char* name);
```
- **Returns**: The component's entity ID if found, or `0` if it does not exist.

#### `has_component`
Checks if a component with a given name is registered.
```cpp
static bool has_component(const flecs::world& entity_world, const char* name);
```
- **Returns**: `true` if registered, `false` otherwise.

#### `is_component`
Checks if an entity ID belongs to a registered component type.
```cpp
static bool is_component(const flecs::world& entity_world, flecs::entity entity);
```
- **Returns**: `true` if the entity represents a component type.

#### `list_components`
Retrieves a list of all registered component entities.
```cpp
static std::vector<flecs::entity> list_components(const flecs::world& entity_world);
```

#### `is_static`
Determines if a component was registered statically via C++.
```cpp
static bool is_static(const flecs::world& entity_world, const char* name);
```

#### `find_schema`
Retrieves the data-driven schema associated with a dynamic component.
```cpp
static sandbox::properties find_schema(const flecs::world& entity_world, const char* name);
```

### Configuration Management

#### `import_configuration`
Imports component configurations from a directory path.
```cpp
static void import_configuration(const flecs::world& entity_world, const char* directory_path);
```

#### `export_configuration`
Exports component configurations to a directory path.
```cpp
static void export_configuration(const flecs::world& entity_world, const char* directory_path);
```

## Lua API Reference

The Lua bindings expose equivalent functionality under the `spectre.components` module. 

### Component Registration (Lua)

#### `spectre.components.register_component`
Registers either a static or dynamic component. If `schema_properties` is provided, it handles dynamic registration.
```lua
spectre.components.register_component(world, name, registration_fn, serializer, schema_properties)
```
- **`world`**: The ECS world.
- **`name`**: String name of the component.
- **`registration_fn`**: C function pointer for static component registration (use `nil` for dynamic).
- **`serializer`**: Serializer struct (use `nil` for dynamic).
- **`schema_properties`**: A `sandbox.Properties` object or handle defining the component schema (required for dynamic registration).

**Example**:
```lua
local dyn_props = sandbox.Properties.create()
dyn_props:set_int("health", 100)
spectre.components.register_component(world, "PlayerStats", nil, nil, dyn_props)
```

### Component Querying (Lua)

#### `spectre.components.find_component(world, name)`
Returns the component entity ID. Returns `0` if not found.

#### `spectre.components.has_component(world, name)`
Returns `true` if the component is registered.

#### `spectre.components.is_component(world, entity)`
Returns `true` if the provided entity is a component type.

#### `spectre.components.is_static(world, name)`
Returns `true` if the component is statically defined in C++.

#### `spectre.components.find_schema(world, name)`
Returns the `sandbox.Properties` schema for the specified component if it has one, or `nil`.

**Example**:
```lua
if spectre.components.has_component(world, "Transform2D") then
    local comp_id = spectre.components.find_component(world, "Transform2D")
    local schema = spectre.components.find_schema(world, "Transform2D")
end
```

## Best Practices & Warnings

> [!CAUTION]
> **Thread Safety**: Flecs is generally not thread-safe for structural changes unless operating within a deferred context. Do not register new components or modify component schemas asynchronously outside the main thread or properly synchronized phases.

> [!WARNING]
> **Schema Lifecycles**: When dealing with `sandbox::properties` (in C++) and `sandbox.Properties` (in Lua), ensure that properties handle memory lifetimes correctly. Dynamic schemas passed into registration are held by the engine; modifying them post-registration may lead to unexpected behavior.

> [!TIP]
> **Static vs Dynamic**: Prefer static C++ components for high-performance and frequently accessed data like `Transform2D` or physics bodies. Reserve dynamic components for gameplay scripts, tags, or editor-driven metadata where performance overhead is negligible but flexibility is key.

> [!NOTE]
> **Reserved Names**: Avoid generic or easily conflicting component names (e.g. `Position`, `Tag`). Prefix them or use domains when working heavily with dynamic scripting to avoid collisions with internal or static components.
