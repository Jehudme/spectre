# Module: Renderer

## Module Overview
The `renderer` module in the Spectre Game Engine provides core ECS-based 2D rendering capabilities. It manages visual entities through ECS components, offering abstractions for rendering shapes, textures, and text. The module handles global rendering state (e.g., background color) and the scene rendering passes (`begin_frame`, `render_frame`, and `render`) by querying and processing renderable objects. It also offers mechanisms for serialization and configuration importing/exporting from JSON.

## Table of Contents
- [Components](#components)
- [C++ API Reference](#c-api-reference)
- [Lua API Reference](#lua-api-reference)
- [Warnings & Best Practices](#warnings--best-practices)

## Components
The module exposes the following standard components to define rendering behavior in the ECS:

- `spectre_2D_transform_component_t`: Controls the position, scale, origin, and rotation of a 2D object.
- `spectre_rectange_renderable_t`: Renders a simple colored/outlined rectangle.
- `spectre_circle_renderable_t`: Renders a simple colored/outlined circle.
- `spectre_polygone_renderable_t`: Renders a regular polygon.
- `spectre_custom_polygone_renderable_t`: Renders a custom polygon given an explicit list of vertices.
- `spectre_ligne_renderable_t`: Renders a line between two coordinate points.
- `spectre_texture_renderable_t`: Renders a 2D texture, supporting tinting, source rectangle clipping, and flipping.
- `spectre_text_renderable_t`: Renders text with a specific font size, spacing, tint, and styles (bold, italic).
- `spectre_material_component_t`: Assigns a specific shader resource to an entity.
- `spectre_renderable_t`: A tag component marking an entity as renderable.

## C++ API Reference

### `spectre::modules::renderer`
The core C++ wrapper provides methods to interact with the renderer's properties and serialization.

#### `deserialize_renderer`
```cpp
static void deserialize_renderer(const flecs::world& entity_world, ecs_entity_t target, sandbox_properties_handle_t props);
```
**Description:** Deserializes renderer properties (like background color) from a properties structure and applies them to the target entity.

#### `serialize_renderer`
```cpp
static sandbox_properties_handle_t serialize_renderer(const flecs::world& entity_world, ecs_entity_t renderer);
```
**Description:** Serializes the current renderer settings into a properties structure.

#### `register_renderer`
```cpp
static void register_renderer(const flecs::world& entity_world, sandbox_properties_handle_t props);
```
**Description:** Registers the renderer in the ECS world with initial properties.

#### `is_renderer`
```cpp
static bool is_renderer(const flecs::world& entity_world);
```
**Description:** Checks if the given world possesses a valid renderer subsystem.

#### `import_configuration`
```cpp
static void import_configuration(const flecs::world& entity_world, const char* path);
```
**Description:** Imports renderer configuration from a JSON file at the specified path.

#### `export_configuration`
```cpp
static void export_configuration(const flecs::world& entity_world, const char* path);
```
**Description:** Exports the current renderer configuration to a JSON file at the specified path.

## Lua API Reference

The `spectre.renderer` global table provides Lua bindings to interact with the renderer via the engine's FFI capabilities.

### `spectre.renderer.deserialize_renderer`
```lua
function spectre.renderer.deserialize_renderer(world, target, props)
```
**Description**: Applies settings from the `props` sandbox property handle to the `target` entity's renderer setup.

### `spectre.renderer.serialize_renderer`
```lua
function spectre.renderer.serialize_renderer(world, renderer)
```
**Description**: Returns a `sandbox_properties_handle_t` containing serialized data of the `renderer` entity.

### `spectre.renderer.register_renderer`
```lua
function spectre.renderer.register_renderer(world, props)
```
**Description**: Registers the renderer in the `world` with the provided `props`.

### `spectre.renderer.is_renderer`
```lua
function spectre.renderer.is_renderer(world)
```
**Description**: Returns a boolean indicating if the given ECS world has a configured renderer.

## Warnings & Best Practices
- **Memory Management for Dynamic Renderables:** `spectre_custom_polygone_renderable_t` uses raw pointers (`float* vertices`) and `spectre_text_renderable_t` uses dynamically allocated strings (`char* content`). Modifying these components directly requires careful memory management to prevent memory leaks or use-after-free errors. Ensure the lifecycle of allocated strings and arrays safely outlives their use by the renderer, and free them when the component is removed.
- **Resource Loading:** When assigning textures (`spectre_texture_renderable_t`) or fonts (`spectre_text_renderable_t`), ensure the corresponding resources have been loaded correctly via the `spectre_resources` subsystem before the frame renders.
- **Transform Hierarchy:** Make sure an entity has a `spectre_2D_transform_component_t` in addition to its renderable component for correct positioning, scaling, and rotation. Uninitialized scale values might cause objects to render with a `(0, 0)` scale, making them invisible.
- **Lua Configuration:** In the Lua API, `import_configuration` and `export_configuration` are currently not exposed natively via FFI. To load/save configuration in Lua scripts, read files directly using the `sandbox.filesystem` module and parse strings into `sandbox.Properties` (as implemented in the editor UI scripts).
