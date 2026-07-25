# Spectre Engine: Comprehensive Architecture & Editor-Readiness Review

This document outlines a complete review of all core engine modules (`spectre/source/modules/`). The analysis focuses on anti-patterns, performance bottlenecks, architectural inconsistencies, and critical gaps that must be addressed before developing a robust, real-time Editor UI.

---

## 1. Memory Management & Leaks (Critical)

While some modules (like `scripts_module`) demonstrate good memory management using Flecs `on_remove` hooks, many modules fail to clean up dynamically allocated memory, leading to severe leaks during entity destruction or modification.

*   **`renderer_module.cpp` (`TextRenderable`)**:
    *   **Issue**: During deserialization, `comp.content = strdup(content.c_str())` is called. However, there is no `on_remove` hook registered for `spectre_text_renderable_t` to `free()` this string. Every time a text entity is destroyed or modified, the string memory leaks.
*   **`window_module.cpp` (`WindowComponent`)**:
    *   **Issue**: `char* title_copy = new char[title.size() + 1];` is assigned to `component.title` without an `on_remove` hook to `delete[]` this array when the window entity is destroyed.
*   **`resources_module.cpp` (`Resource`)**:
    *   **Issue**: `char* path_copy = new char[path.size() + 1];` is allocated during resource deserialization, but never explicitly `delete[]`'d.
*   **`scripts_module.cpp` (Hot-Reloading Leak)**:
    *   **Issue**: When `eval_code` is called on an already existing script (e.g., during hot-reloading), it calls `script_entity.set<spectre_script_t>(script_data)`. Because `spectre_script_t` is a trivially copyable C-struct, Flecs simply overwrites the memory without calling `on_remove` for the old component, leaking `stable_names` and `stable_types` arrays.

**Recommendation**: 
Enforce a rigid rule in `components_module` to auto-generate `on_remove` cleanup hooks for string allocations, and always use `.modified()` or properly remove existing components before setting new ones with dynamic payloads.

---

## 2. Prefabs & Scene Instantiation (Critical Bug)

*   **`prefabs_module.cpp` (Global vs. Local Namespace lookups)**:
    *   **Issue**: In `deserialize_entity`, when instantiating children for a prefab or scene, the code attempts to preserve existing entities using `m_world.lookup(entity_name.c_str())`. Because this operates on the global `m_world` instead of locally on `target_entity`, a generic child name (like `"Sprite"`) will accidentally link to or overwrite a global `"Sprite"` entity rather than creating a new local child relative to the parent.
    *   **Impact**: Spawning two prefabs that share the same child names will break the hierarchy and merge the children into a single globally accessible entity.
    *   **Recommendation**: Change `m_world.lookup(...)` to `target_entity.lookup(...)` when resolving child entities during deserialization.

---

## 3. Performance & Rendering Bottlenecks

*   **`renderer_module.cpp` (Resource Property Binding)**:
    *   **Issue**: In `texture_load_fn` and `font_load_fn`, configurations like Wrap Mode and Filtering are fetched from `sandbox::properties` via string lookups. Currently, this happens at load-time (which is fine). However, if you plan to let the Editor tweak these settings in real-time, you cannot afford string lookups every frame.
    *   **Recommendation**: Extract Editor-tweakable settings (like `Filter` or `WrapMode`) into the `spectre_texture_renderable_t` component as integers/enums. Apply them during `render_frame()` using dirty flags to avoid redundant OpenGL state changes.

---

## 4. Editor-Thread Safety & Concurrency

*   **`runtime_module.cpp` (Background Threading)**:
    *   **Issue**: `runtime_module::start()` launches a `std::thread` that runs the engine tick (`entity_world.progress()`). Flecs is *not* entirely thread-safe when adding/removing entities and components unless specifically configured. 
    *   **Impact**: If an eventual Editor UI runs on the main thread (e.g., Dear ImGui) and interacts with the `entity_world` while the background thread is calling `.progress()`, it will cause severe race conditions, deadlocks, and crashes.
    *   **Recommendation**: For an Editor, you must shift to a single-threaded architecture where `editor_update()` and `engine.progress()` execute sequentially in the main thread, or wrap all editor-to-ECS operations in thread-safe command buffers.

---

## 5. Architecture & Dangling States

*   **`scenes_module.cpp` (Manual State Stack Management)**:
    *   **Issue**: The `m_state_stack` uses a raw `std::stack<flecs::entity>`. If an entity representing a State is destroyed arbitrarily (e.g., the user clicks "Delete" on the State entity in an Editor hierarchy), the `std::stack` will contain a dangling entity reference. Calling `pop_state()` will then crash the engine.
    *   **Recommendation**: Listen to the `flecs::OnRemove` event for `spectre_state_t`. If a state entity is destroyed, safely purge it from `m_state_stack`.

*   **Serialization of Custom Types**:
    *   **Issue**: `CustomPolygoneRenderable` is registered with an empty dummy serializer.
    *   **Impact**: Levels utilizing custom polygons will silently fail to save or load polygon vertices in the Editor. You must implement array serializers for standard component payloads.

*   **Hardcoded Serialization Defaults**:
    *   **Issue**: Defaults (like width `10.0f` or scale `1.0f`) are hardcoded directly into the JSON reading logic (`props.get<float>(...).value_or(10.0f);`).
    *   **Impact**: Creating entities programmatically via C++ or Lua bypasses the JSON layer, leaving components initialized to `0.0f`.
    *   **Recommendation**: Use Flecs' built-in component default hooks (`flecs::world::component<T>().add(...)`) so that native programmatic creation and JSON deserialization share the same base defaults.

---

## Conclusion & Next Steps

Before starting development on a visual Editor, the following structural fixes are strictly necessary:
1. Fix the global namespace collision bug in `prefabs_module.cpp`.
2. Clean up memory allocations (`strdup`, `new[]`) using `on_remove` hooks across all modules, and handle component overwrites properly.
3. Switch the `runtime` to a single-threaded loop (or synchronous lock-stepped threads) to prevent the Editor from causing data races with Flecs.
4. Add robust `OnRemove` event handlers to safely clean up internal systems like `scenes_module.cpp`'s state stack.
