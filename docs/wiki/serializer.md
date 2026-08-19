# Serializer Module

## Module Overview
The `serializer` module in the Spectre Game Engine provides a central registry and service for serializing and deserializing entity data. It allows different engine modules to define custom serialization logic for their components and register them under string-based type identifiers (e.g., `"scripts"`). These serializers use `sandbox::properties` to store and retrieve data, enabling a generic, data-driven workflow for saving scenes, loading prefabs, or synchronizing network state. Under the hood, serializers are represented as entities within the Flecs ECS, ensuring they integrate seamlessly into the engine's architecture.

## Table of Contents
- [Components](#components)
  - [spectre_serializer_component](#spectre_serializer_component)
- [C++ API Reference](#c-api-reference)
  - [spectre::modules::serializer](#spectremodulesserializer)
- [C-API Reference](#c-api-reference-1)
  - [spectre_serializer_service_t](#spectre_serializer_service_t)
- [Warnings & Best Practices](#warnings--best-practices)

## Components

### `spectre_serializer_component`
The core component attached to a serializer entity. It holds the function pointers that execute the actual serialization logic.

```c
typedef struct spectre_serializer_component {
    void (*deserialize)(ecs_world_t* world, ecs_entity_t serializer_entity, ecs_entity_t entity, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize)(ecs_world_t* world, ecs_entity_t serializer_entity, ecs_entity_t entity);
} spectre_serializer_component;
```
- **`deserialize`**: Reads data from `props` and applies it to the target `entity`.
- **`serialize`**: Reads data from the target `entity` and returns a newly created `sandbox_properties_handle_t` containing the serialized state.

## C++ API Reference

### `spectre::modules::serializer`
The C++ SDK wrapper providing static methods to interact with the serializer service. Header: `spectre/sdk/serializer.hpp`

#### `register_serializer`
```cpp
static void register_serializer(const flecs::world& entity_world, const char* type, const spectre_serializer_component* serializer);
```
Registers a new serializer component under a string `type`.
- **`entity_world`**: The Flecs world.
- **`type`**: A unique string identifier for this serializer.
- **`serializer`**: A pointer to the populated `spectre_serializer_component`.

#### `has_serializer`
```cpp
static bool has_serializer(const flecs::world& entity_world, const char* type);
```
Checks if a serializer is registered for the given `type`. Returns `true` if it exists and is a valid serializer entity.

#### `is_serializer`
```cpp
static bool is_serializer(const flecs::world& entity_world, flecs::entity entity);
```
Checks if the provided `entity` is a valid serializer (i.e., instantiated from the `::serializers::prefab`).

#### `find_serializer`
```cpp
static ecs_entity_t find_serializer(const flecs::world& entity_world, const char* type);
```
Finds and returns the ECS entity ID associated with the registered `type`. Returns 0 (null entity) if not found.

#### `serialize_entity`
```cpp
static sandbox_properties_handle_t serialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, ecs_entity_t entity);
```
Executes the serialization logic of the specified `serializer` on the target `entity`. Returns a handle to the populated properties.

#### `deserialize_entity`
```cpp
static void deserialize_entity(const flecs::world& entity_world, ecs_entity_t serializer, ecs_entity_t target, sandbox_properties_handle_t props);
```
Executes the deserialization logic of the specified `serializer` to apply the data from `props` to the `target` entity.

## C-API Reference

The module exposes a C-API service `spectre_serializer_service_t` (and ABI struct `spectre_serializer_api_t`) along with direct function exports. Header: `spectre/services/serializer_service.h`

```c
SPECTRE_API void spectre_serializer_register_serializer(ecs_world_t* world, const char* type, const spectre_serializer_component* serializer);

SPECTRE_API bool spectre_serializer_has_serializer(ecs_world_t* world, const char* type);

SPECTRE_API bool spectre_serializer_is_serializer(ecs_world_t* world, ecs_entity_t entity);

SPECTRE_API ecs_entity_t spectre_serializer_find_serializer(ecs_world_t* world, const char* type);

SPECTRE_API sandbox_properties_handle_t spectre_serializer_serialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity);

SPECTRE_API void spectre_serializer_deserialize_entity(ecs_world_t* world, ecs_entity_t serializer, ecs_entity_t entity, sandbox_properties_handle_t props);
```

## Warnings & Best Practices

- **Valid Function Pointers**: When registering a serializer, both `serialize` and `deserialize` function pointers in the `spectre_serializer_component` **must not be null**. Attempting to register a null function pointer will fail and log an error.
- **Empty Type Identifiers**: Registering or querying a serializer with an empty string (`""`) is invalid and will safely fail with a logged error/warning. Always use clear, descriptive string identifiers (e.g., `"transform"`, `"scripts"`).
- **Target Entity Validation**: Ensure the `target_entity` passed to `serialize_entity` and `deserialize_entity` is valid. The module includes internal guards that return empty properties or abort the operation if the entity is invalid, but relying on these guards is poor practice.
- **ECS Structure Awareness**: Serializers are created as entities under the `::serializers` hierarchy in Flecs. Modifying or deleting these entities manually can break serialization for associated types.
- **Property Memory Management**: The `serialize` callback creates and returns a `sandbox_properties_handle_t`. Depending on your usage of the `sandbox::properties` API, make sure handles are appropriately managed/freed when no longer needed to prevent memory leaks.
