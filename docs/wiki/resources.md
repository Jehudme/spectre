# Resources Module

## Module Overview
The **Resources Module** (`spectre::modules::resource_module_t`) manages resource lifecycles within the Spectre Game Engine. It operates tightly with Flecs (ECS) by treating both resources and resource loaders as entities. The module provides functionality to register custom loaders for specific resource types (e.g., textures, audio, fonts) and manages the configuration, loading, unloading, serialization, and deserialization of resource objects.

Resources are typically defined in configuration files (like JSON) using `sandbox::properties` and can be imported or exported dynamically. 

**Key Responsibilities**:
- **Resource Loaders**: Register custom handlers to manage loading/freeing logic per resource type.
- **Resource Management**: Lazy-loading and memory management of resources as ECS entities.
- **Configuration Parsing**: Import and export properties-based configurations defining engine resources.
- **Serialization**: Seamlessly integrate resources into Spectre's serialization workflows.

---

## Table of Contents
- [Components](#components)
- [C++ SDK Reference](#c-sdk-reference)
- [C-API & Lua Bindings](#c-api--lua-bindings)
- [Best Practices & Warnings](#best-practices--warnings)

---

## Components

### `spectre_resource_component_t`
Represents an instance of a resource.
```c
typedef struct spectre_resource_component_t {
    const char* path;
    void* instance; // Raw pointer to the loaded resource
    sandbox_properties_handle_t properties_handle;
} spectre_resource_component_t;
```

### `spectre_resource_loader_component_t`
Stores function pointers to handle the actual loading and freeing of a resource.
```c
typedef struct spectre_resource_loader_component_t {
    void (*load_fn)(ecs_world_t* world, spectre_resource_component_t* resource);
    void (*free_fn)(ecs_world_t* world, spectre_resource_component_t* resource);
} spectre_resource_loader_component_t;
```

---

## C++ SDK Reference

Include: `<spectre/sdk/resources.hpp>`  
Namespace: `spectre::modules::resources`

### `register_resource_loader`
```cpp
static void register_resource_loader(const flecs::world& entity_world, const char* type, spectre_resource_loader_component_t loader);
```
Registers a new resource loader for a given string `type` (e.g. `"texture"`).

### `register_resource`
```cpp
static void register_resource(const flecs::world& entity_world, sandbox_properties_handle_t props);
```
Registers a new resource entity from sandbox properties.

### `load_resource` / `free_resource`
```cpp
static void load_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity);
static void free_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity);
```
Triggers the associated loader to load or free the specified resource entity.

### `get_resource`
```cpp
static void* get_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity);
```
Returns the raw pointer to the loaded resource. Ensure the resource is loaded (`is_resource_loaded`) before accessing.

### `is_resource_loaded`
```cpp
static bool is_resource_loaded(const flecs::world& entity_world, ecs_entity_t resource);
```
Returns `true` if the resource has been loaded into memory (i.e. has the `spectre_resource_flag_t` flag).

### `has_resource` / `has_resource_loader`
```cpp
static bool has_resource(const flecs::world& entity_world, const char* name);
static bool has_resource_loader(const flecs::world& entity_world, const char* type);
```
Checks if a resource or loader exists by name/type.

### `find_resource` / `find_resource_loader`
```cpp
static ecs_entity_t find_resource(const flecs::world& entity_world, const char* name);
static ecs_entity_t find_resource_loader(const flecs::world& entity_world, const char* type);
```
Looks up and returns the underlying ECS entity ID for a resource or loader.

### `list_resources` / `list_resource_loaders`
```cpp
static std::vector<flecs::entity> list_resources(const flecs::world& entity_world);
static std::vector<flecs::entity> list_resource_loaders(const flecs::world& entity_world);
```
Returns a list of all current resource or loader entities in the world.

### `serialize_resource` / `deserialize_resource`
```cpp
static sandbox_properties_handle_t serialize_resource(const flecs::world& entity_world, ecs_entity_t resourceEntity);
static void deserialize_resource(const flecs::world& entity_world, ecs_entity_t target, sandbox_properties_handle_t props);
```
Handles properties-based serialization for a resource entity.

### `import_configuration` / `export_configuration`
```cpp
static void import_configuration(const flecs::world& entity_world, const char* path);
static void export_configuration(const flecs::world& entity_world, const char* path);
```
Imports or exports resources definitions via JSON/properties files.

---

## C-API & Lua Bindings

The C API functions follow the pattern `spectre_resources_<function_name>`. These are also bound to Lua via LuaJIT's FFI in the `spectre.resources` table.

### Lua Examples

```lua
local spectre = require("spectre")

-- Find a resource entity by its name
local my_texture_entity = spectre.resources.find_resource(world, "hero_sprite")

-- Check if it's loaded, and load it if not
if not spectre.resources.is_resource_loaded(world, my_texture_entity) then
    spectre.resources.load_resource(world, my_texture_entity)
end

-- Get the raw pointer to the resource
local raw_texture_ptr = spectre.resources.get_resource(world, my_texture_entity)

-- Free the resource when done
spectre.resources.free_resource(world, my_texture_entity)
```

Listing resources and loaders in Lua:
```lua
-- Retrieve all available resource loaders
local loaders = spectre.resources.list_resource_loaders(world)
-- Note: Lua bindings for list returns may require FFI size handling.
```

---

## Best Practices & Warnings

> [!WARNING]
> **Memory Management**: Resources returned by `get_resource()` are raw `void*` pointers. You MUST explicitly cast them to your target underlying type based on the loader used (e.g., raylib `Texture2D*` for textures). Be mindful of manually calling `free_resource()` when a resource is no longer needed to prevent memory leaks, as automatic garbage collection is not provided.

> [!CAUTION]
> **Unloaded Resources**: Always verify that a resource is loaded via `is_resource_loaded()` before invoking `get_resource()`. Using a null pointer returned by an unloaded resource will likely cause fatal memory access violation crashes.

> [!IMPORTANT]
> **ECS Integration**: Resources in Spectre are tightly coupled ECS Entities within Flecs. Modifying standard properties or removing relational components (like `spectre_use_loader_relation_t`) on a Resource Entity will trigger engine-wide undefined behaviors. Do not strip standard resource relationships or components unless writing a replacement core loader/manager module.

> [!TIP]
> **Lazy Loading**: Avoid aggressively loading all resources eagerly unless absolutely necessary. Defer `load_resource` until the specific resource is strictly required in the active scene or rendering phase to conserve memory footprint. Utilize `import_configuration` selectively.
