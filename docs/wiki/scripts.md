# Scripts Module

## Overview
The Scripts module is a powerful system that bridges Flecs entities with Lua scripting in the Spectre Game Engine. It allows developers to attach Lua logic directly to entities and trigger them at specific points during their lifecycle (e.g., `on_create`, `on_update`, `on_enter`, etc.). The module automatically manages the lifecycle of the Lua state, exposes a custom VFS loader, and processes argument parsing and JSON-based serialization/deserialization for component-based script properties.

Script parameters can be automatically resolved, mapped to native engine types, or dynamically sourced from contextual string tokens like `%self%`, `%scene%`, and `%state%`.

## Table of Contents
- [Scripts Module](#scripts-module)
  - [Overview](#overview)
  - [Table of Contents](#table-of-contents)
  - [API Reference](#api-reference)
    - [Defining Scripts in Lua](#defining-scripts-in-lua)
    - [C++ SDK / C-API Reference](#c-sdk--c-api-reference)
      - [`include_code`](#spectremodulesscriptsinclude_code)
      - [`has_script`](#spectremodulesscriptshas_script)
      - [`is_script`](#spectremodulesscriptsis_script)
      - [`find_script`](#spectremodulesscriptsfind_script)
      - [`list_scripts`](#spectremodulesscriptslist_scripts)
      - [`execute_script`](#spectremodulesscriptsexecute_script)
      - [Entity Execution Lifecycle API](#entity-execution-lifecycle-api)
      - [Serialization API](#serialization-api)
  - [Warnings, Cautions & Best Practices](#warnings-cautions--best-practices)

## API Reference

### Defining Scripts in Lua
Scripts in Spectre are written in Lua and are structured to export functions alongside their type definitions. Typically, a Lua script uses a wrapper function (like `ecs.Script.define`) to export a list of typed arguments to the engine.

**Argument Types Supported:** `nil`, `boolean`, `number`, `integer`, `string`, `table`, `userdata`.

**Example (`test_script.lua`):**
```lua
local ecs = require "ecs"

local Test = {}

function Test.on_update(entity_id, velocity, active)
    if active then
        print("Updating entity: " .. tostring(entity_id) .. " with velocity: " .. tostring(velocity))
    end
end

-- Export mapping function back to the engine
return {
    on_update = ecs.Script.define(Test.on_update, "velocity:number", "active:boolean")
}
```

### C++ SDK / C-API Reference

The core API is provided in `spectre/include/spectre/sdk/scripts.hpp`. You can use the static methods on `spectre::modules::scripts`.

#### `spectre::modules::scripts::include_code`
Evaluates a Lua script file, loads it into the Lua state, and registers its defined functions.
- **C-API:** `spectre_scripts_include_code(ecs_world_t* world, const char* path)`
- **Arguments:**
  - `entity_world`: Reference to the `flecs::world`.
  - `path`: The file path (using the Sandbox VFS prefix, e.g., `app://...` or an absolute path).

#### `spectre::modules::scripts::has_script`
Checks whether a script function matching a specific name and argument signature is currently loaded.
- **C-API:** `spectre_scripts_has_script(...)`
- **Arguments:**
  - `entity_world`: The `flecs::world`.
  - `function_name`: The name of the Lua function.
  - `arg_types`: A `std::vector` of `spectre_script_argument_type_t` describing the signature.
- **Returns:** `bool` - True if the script is loaded and registered.

#### `spectre::modules::scripts::is_script`
Checks if an entity is recognized as a registered script prefab entity inside the internal script tree (`::scripts::...`).
- **C-API:** `spectre_scripts_is_script(...)`
- **Arguments:**
  - `entity_world`: The `flecs::world`.
  - `entity`: The Flecs entity id to check.
- **Returns:** `bool`

#### `spectre::modules::scripts::find_script`
Returns the `flecs::entity` handle representing the internally registered script object.
- **C-API:** `spectre_scripts_find_script(...)`
- **Arguments:**
  - `entity_world`: The `flecs::world`.
  - `function_name`: The name of the script.
- **Returns:** `flecs::entity` - An entity containing `spectre_script_t` data, or a null entity if not found.

#### `spectre::modules::scripts::list_scripts`
Retrieves a list of all dynamically registered script entities.
- **C-API:** `spectre_scripts_list_scripts(...)`
- **Arguments:**
  - `entity_world`: The `flecs::world`.
- **Returns:** `std::vector<flecs::entity>`

#### `spectre::modules::scripts::execute_script`
Manually calls a Lua script function and passes arbitrary arguments directly from C++.
- **C-API:** `spectre_scripts_execute_script(...)`
- **Arguments:**
  - `entity_world`: The `flecs::world`.
  - `function_name`: The registered name of the script.
  - `args`: A reference to a `std::vector<spectre_script_argument_t>`.

#### Entity Execution Lifecycle API
Entities that hold relationships (e.g. `UseScriptOnUpdateRelation`) to scripts can be fired dynamically based on their lifecycle events. **Note: The first argument passed to the Lua script is *always* the `entity_id` triggering it.**
- **`execute_on_create(flecs::entity entity)`**: Calls all scripts linked with `UseScriptOnCreateRelation`.
- **`execute_on_destroy(flecs::entity entity)`**: Calls all scripts linked with `UseScriptOnDestroyRelation`.
- **`execute_on_update(flecs::entity entity)`**: Calls all scripts linked with `UseScriptOnUpdateRelation`.
- **`execute_on_enter(flecs::entity entity)`**: Calls all scripts linked with `UseScriptOnEnterRelation` (also checks prefab inheritance recursively).
- **`execute_on_exit(flecs::entity entity)`**: Calls all scripts linked with `UseScriptOnExitRelation` (also checks prefab inheritance recursively).

#### Serialization API
Allows serializing/deserializing script components and relations into `sandbox::properties` (JSON data format).

- **`serialize_scripts(flecs::world, flecs::entity entity)`**: Returns `sandbox::properties`.
- **`deserialize_scripts(flecs::world, ecs_entity_t target, sandbox_properties_handle_t props)`**: Deserializes relations onto `target`. 
  - *Context Tokens:* String arguments inside the JSON properties map can explicitly resolve variables based on hierarchy:
    - `%self%`: Replaced with the target entity's ID.
    - `%scene%`: Traverses parent entities to locate the nearest entity possessing `spectre_scene_context_t` and yields its ID.
    - `%state%`: Traverses parent entities to locate the nearest entity possessing `spectre_state_context_t` and yields its ID.

## Warnings, Cautions & Best Practices

- **The Entity ID First Argument Limitation**: Every lifecycle script trigger (e.g., executed by `execute_on_update`) automatically supplies the target entity ID as its absolute first argument in Lua. *Always verify your Lua functions declare the entity ID as the first parameter before any custom relation arguments.*
- **String Memory Ownership**: String arguments within script relations are copied/duplicated, and internal memory cleanup happens automatically via Flecs removal callbacks (`on_remove` hooks) upon entity or relation destruction. Do not manually `free()` strings linked inside `spectre_script_argument_t` structures, or you will cause double-free faults.
- **Lua Threading Issues**: The module employs a standard `lua_State*`. Attempting to modify or execute scripts from multiple threads without locks will cause severe race conditions. Ensure script loading and lifecycle execution strictly happen on the main ECS tick thread.
- **Context Tokens Traversal Overhead**: Generating values for the `%scene%` and `%state%` tokens triggers a recursive hierarchical parent traversal. Be mindful that in heavily nested prefabs, resolving these tokens over a vast number of elements continuously during map loading could incur an initialization performance cost.
- **Sandbox VFS Context**: The `require` statement in Lua routes through a custom C-loader attached to Sandbox's filesystem. Attempting to bypass standard module loading rules natively (like `io.open`) is restricted and may result in loading failures under strict packaged sandboxing.
- **Strict Argument Matching**: Errors derived from an incorrect number of arguments, or improperly typed arguments inside relation declarations (e.g., passing a string relation to a script that expects a number) will gracefully abort execution via `sandbox::modules::logs::error`. Actively monitor standard output and engine logs when a script inexplicably "does nothing" to catch type-mismatch complaints.
