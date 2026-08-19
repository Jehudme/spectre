# Runtime Module

## Module Overview

The `runtime` module is the core orchestrator of the Spectre Game Engine. Its primary purpose is to drive the engine's main loop and manage its initialization and execution state. It is responsible for loading the engine configuration (resources, prefabs, scenes, components, renderer, and window), establishing the default state stack, and updating the ECS world continuously. 

The runtime can be executed either synchronously (blocking the current thread) or asynchronously (spawning a dedicated background thread). It implements the `sandbox_runtime_service_t` and `sandbox_runtime_api_t` interface for seamless integration.

## Table of Contents

- [Module Overview](#module-overview)
- [C++ SDK API Reference](#c-sdk-api-reference)
  - [`sandbox::modules::runtime::run`](#sandboxmodulesruntimerun)
  - [`sandbox::modules::runtime::start`](#sandboxmodulesruntimestart)
  - [`sandbox::modules::runtime::stop`](#sandboxmodulesruntimestop)
  - [`sandbox::modules::runtime::pause`](#sandboxmodulesruntimepause)
  - [`sandbox::modules::runtime::resume`](#sandboxmodulesruntimeresume)
- [C API Reference](#c-api-reference)
- [Lua Bindings Reference](#lua-bindings-reference)
  - [`sandbox.runtime.run`](#sandboxruntimerun)
  - [`sandbox.runtime.start`](#sandboxruntimestart)
  - [`sandbox.runtime.stop`](#sandboxruntimestop)
  - [`sandbox.runtime.pause`](#sandboxruntimepause)
  - [`sandbox.runtime.resume`](#sandboxruntimeresume)
- [Warnings & Best Practices](#warnings--best-practices)

---

## C++ SDK API Reference

Included via `<sandbox/sdk/runtime.hpp>`.

### `sandbox::modules::runtime::run`

```cpp
static void run(flecs::world& entity_world);
```
**Description:** Initializes the runtime configuration and runs the engine loop synchronously. This function will block the current thread until the loop is stopped or the ECS world finishes its progression.
- **Arguments:**
  - `entity_world`: The `flecs::world` instance representing the active entity world.
- **Return Value:** None.

### `sandbox::modules::runtime::start`

```cpp
static void start(flecs::world& entity_world);
```
**Description:** Initializes the runtime configuration and starts the engine loop asynchronously in a new background thread.
- **Arguments:**
  - `entity_world`: The `flecs::world` instance representing the active entity world.
- **Return Value:** None.

### `sandbox::modules::runtime::stop`

```cpp
static void stop(flecs::world& entity_world);
```
**Description:** Signals the runtime loop to stop. Quits the `flecs` world, resumes execution if it was paused, and joins the background thread if the runtime was started asynchronously.
- **Arguments:**
  - `entity_world`: The `flecs::world` instance running the simulation.
- **Return Value:** None.

### `sandbox::modules::runtime::pause`

```cpp
static void pause(flecs::world& entity_world);
```
**Description:** Pauses the execution of the main loop. The engine thread will safely wait on a condition variable, avoiding busy-waiting and saving CPU resources.
- **Arguments:**
  - `entity_world`: The `flecs::world` instance.
- **Return Value:** None.

### `sandbox::modules::runtime::resume`

```cpp
static void resume(flecs::world& entity_world);
```
**Description:** Resumes the execution of a paused engine loop, waking up the suspended background thread.
- **Arguments:**
  - `entity_world`: The `flecs::world` instance.
- **Return Value:** None.

---

## C API Reference

Included via `<sandbox/services/runtime_service.h>`.

The C API exposes a set of wrapper functions around the internal runtime logic:

```c
SANDBOX_API void sandbox_runtime_run(ecs_world_t* ecs);
SANDBOX_API void sandbox_runtime_start(ecs_world_t* ecs);
SANDBOX_API void sandbox_runtime_stop(ecs_world_t* ecs);
SANDBOX_API void sandbox_runtime_pause(ecs_world_t* ecs);
SANDBOX_API void sandbox_runtime_resume(ecs_world_t* ecs);
```
These functions behave exactly like their C++ counterparts, operating on the raw `ecs_world_t*` pointers.

---

## Lua Bindings Reference

Available in the `sandbox.runtime` table within Lua scripts. 

### `sandbox.runtime.run`
```lua
sandbox.runtime.run(world)
```
**Description:** Runs the runtime synchronously.
- **Arguments:** 
  - `world`: The `ecs_world_t` pointer or a wrapped table with a `.ptr` field.

### `sandbox.runtime.start`
```lua
sandbox.runtime.start(world)
```
**Description:** Starts the runtime asynchronously in a background thread.
- **Arguments:** 
  - `world`: The `ecs_world_t` pointer or a wrapped table with a `.ptr` field.

### `sandbox.runtime.stop`
```lua
sandbox.runtime.stop(world)
```
**Description:** Stops the currently executing runtime.
- **Arguments:** 
  - `world`: The `ecs_world_t` pointer or a wrapped table with a `.ptr` field.

### `sandbox.runtime.pause`
```lua
sandbox.runtime.pause(world)
```
**Description:** Pauses the active runtime loop.
- **Arguments:** 
  - `world`: The `ecs_world_t` pointer or a wrapped table with a `.ptr` field.

### `sandbox.runtime.resume`
```lua
sandbox.runtime.resume(world)
```
**Description:** Resumes a paused runtime loop.
- **Arguments:** 
  - `world`: The `ecs_world_t` pointer or a wrapped table with a `.ptr` field.

---

## Warnings & Best Practices

> [!WARNING]  
> **Thread Safety and Double Starting:** Do not call `run()` or `start()` if the runtime is already active. Attempting to start the runtime while it is already running will trigger a warning and exit early without launching a new loop.

> [!CAUTION]
> **Lifetime Management:** If you use `start()` to spawn a background thread, ensure that `stop()` is always called during shutdown to properly join the thread. Dropping the world without stopping the runtime may result in deadlocks or dangling threads accessing freed resources.

> [!TIP]
> **Pause Mechanism:** Using `pause()` efficiently suspends the background loop using a standard `std::condition_variable` wait. This is highly recommended to throttle CPU usage if your application enters the background or loses window focus.

> [!IMPORTANT]
> **Configuration Loading:** Both `run()` and `start()` implicitly load all engine configurations before starting the loop. Ensure your `app://configs/window.json`, `app://configs/renderer.json`, and `app://configs/startup.json` files are properly formatted, as the module relies on them for initialization.
