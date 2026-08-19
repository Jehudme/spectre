# Scenes Module

## Module Overview

The `scenes` module provides state and scene management for the Spectre Engine. It allows developers to organize the application into distinct, high-level modes (States) that manage collections of entities (Scenes). 

- **States** act as high-level application modes (e.g., "Main Menu", "In Game", "Loading Screen"). They are managed via a stack system. When a state is pushed to the top of the stack, it is instantiated and becomes the active state, triggering script lifecycle events (`on_enter`, `on_update`, etc.).
- **Scenes** are collections of entities. States can reference and instantiate scenes. 

Internally, states and scenes are stored as Flecs prefabs (under the roots `::states` and `::scenes` respectively). When a state is pushed onto the stack, an instance of the state prefab is created, along with instances of any attached scenes.

## Table of Contents
- [State Stack Management](#state-stack-management)
- [Querying & Inspection](#querying--inspection)
- [Registration & Serialization](#registration--serialization)
- [Configuration](#configuration)
- [Execution](#execution)
- [Warnings, Cautions & Best Practices](#warnings--cautions--best-practices)

## API Reference

The APIs are available in C++ via `spectre::modules::scenes` and in Lua via `spectre.scenes`.

### State Stack Management

#### `push_state`
Pushes a state prefab onto the active state stack. This creates a new instance of the state, calls `on_exit` on the previous top state (while disabling its rendering), and recursively calls `on_enter` on the newly instantiated state and its children.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `state` (`ecs_entity_t`): The state prefab to push.
- **Return Value:** None.

**Lua Example:**
```lua
local state_prefab = spectre.scenes.find_state(g_world, "InGame")
spectre.scenes.push_state(g_world, state_prefab)
```

#### `pop_state`
Pops the current active state instance from the stack. Destructs the state instance after recursively calling `on_exit` and `on_destroy`. It then resumes rendering and calls `on_enter` on the newly exposed top state.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
- **Return Value:** None.

**Lua Example:**
```lua
spectre.scenes.pop_state(g_world)
```

### Querying & Inspection

#### `find_current_state`
Retrieves the currently active instantiated state at the top of the stack.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
- **Return Value:** `ecs_entity_t` - The active state instance, or `0` if the stack is empty.

#### `find_state`
Retrieves a state prefab by name from the registry.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `name` (`const char*` / `string`): The name of the state.
- **Return Value:** `ecs_entity_t` - The state prefab, or `0` if not found.

#### `find_scene`
Retrieves a scene prefab by name from the registry.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `name` (`const char*` / `string`): The name of the scene.
- **Return Value:** `ecs_entity_t` - The scene prefab, or `0` if not found.

#### `find_current_scenes`
Retrieves an ECS query iterating over the active instantiated scenes attached to the current active state.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
- **Return Value:** `ecs_query_t*` - The ECS query.

#### `has_state` / `has_scene`
Checks if a state or scene prefab exists by name.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `name` (`const char*` / `string`): The name to check.
- **Return Value:** `bool` - `true` if it exists, `false` otherwise.

#### `is_state` / `is_scene`
Checks if an entity possesses the `spectre_state_t` or `spectre_scene_t` component.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `entity` (`ecs_entity_t`): The entity to check.
- **Return Value:** `bool` - `true` if the entity is a state/scene, `false` otherwise.

#### `list_states`
Returns a list of all registered state prefabs. (Available in C++).

- **Arguments:**
  - `world` (`flecs::world`): The ECS world.
- **Return Value:** `std::vector<flecs::entity>` - A vector of state prefabs.

### Registration & Serialization

#### `register_state` / `register_scene`
Registers or updates a state/scene prefab using a `sandbox::properties` object. If the prefab does not exist, it creates it under the respective root (`::states` or `::scenes`).

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `props` (`sandbox_properties_handle_t`): The properties object containing the definition.
- **Return Value:** None.

#### `serialize_state` / `serialize_scene`
Serializes a state/scene entity into a properties object. For states, this includes its name, scripts, and referenced scenes. For scenes, this serializes the scene and all of its entities.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `entity` (`ecs_entity_t`): The state or scene to serialize.
- **Return Value:** `sandbox_properties_handle_t` - The serialized properties.

#### `deserialize_state` / `deserialize_scene`
Deserializes properties into an existing state or scene entity.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `target` (`ecs_entity_t`): The target entity to populate.
  - `props` (`sandbox_properties_handle_t`): The properties to deserialize.
- **Return Value:** None.

### Configuration

#### `import_configuration` / `export_configuration`
Imports or exports the states and scenes configurations from/to a specific directory path.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `directory_path` (`const char*` / `string`): The filesystem path.
- **Return Value:** None.

### Execution

#### `execute_recursive`
Recursively executes a callback on a parent entity and all of its children, bypassing the internal "scripts" child container.

- **Arguments:**
  - `world` (`flecs::world` / `ecs_world_t*`): The ECS world.
  - `entity` (`ecs_entity_t`): The root entity.
  - `callback` (`spectre_recursive_callback_t`): The C function callback to invoke on each entity.
  - `payload` (`void*`): User data passed to the callback.
- **Return Value:** None.

## Warnings, Cautions & Best Practices

- **Instance vs. Prefab Confusion:** `find_state` returns the state **prefab** (the blueprint), while `find_current_state` returns the active **instance** of the state. You must pass a state **prefab** to `push_state`.
- **Entity Destruction on Pop:** When `pop_state` is called, the current state instance and *all* of its instantiated child entities (including attached scenes and their contents) are permanently destructed. Do not retain handles to these entities across state transitions, as they will point to invalid memory or stale IDs.
- **Rendering & Memory Footprint:** Pushing a new state does not destroy the previous state; it remains on the stack. The previous state is given a `DisableRendering` component, and its `on_update` scripts are paused. However, all of its entities and assets remain loaded in memory. Be mindful of memory consumption when using deeply nested state stacks with heavy scenes.
- **Valid Names:** Attempting to register a state or scene with an empty name will result in an error. Always ensure properties have a valid `name` field during registration.
- **Script Callbacks:** When a state is pushed, `on_exit` is called on the active state (if any), followed by `on_enter` on the new state instance. Conversely, popping a state triggers `on_exit` and `on_destroy` on the popped state, and then `on_enter` on the state that resumes execution. Plan your state transitions assuming these exact hook calls.
