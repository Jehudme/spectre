# Spectre Engine & Editor - Codebase Review & Action Plan

This document provides a comprehensive, professional technical review of the Spectre Engine Editor. Based on a deep-dive investigation of the C++ backend and Lua frontend, it outlines identified bugs, UX deficiencies, architectural inconsistencies, and requested features. 

To ensure a prioritized workflow, issues are divided into three phases: **Phase 1: Core Bugs, Leaks, & Data Integrity**, **Phase 2: Editor Workflow & UI Enhancements**, and **Phase 3: Engine Architecture & Lua Developer Experience (DX)**.

*(Note: The previously discussed Color Normalization and Project List VFS caching items were investigated and determined to be functioning correctly as intended by the engine's design, and have thus been omitted from this active roadmap).*

---

## Phase 1: Core Bugs, Leaks, & Data Integrity (Ordered by Severity)

### 1. File Browser Path Navigation Bug
**Severity**: Critical
**Status**: 🟢 Fixed
**Location**: `demo/spectre-editor/resources/scripts/utilities/filebrowser.lua` (`get_breadcrumb_parts`)
**Root Cause**: The string accumulator aggressively stripped trailing slashes from virtual paths (e.g., transforming `save://` into the invalid `save:/projects`), causing the VFS to reject the path and return an empty file list. This has been patched.

### 2. Window Settings Application Desync
**Severity**: Medium
**Status**: 🟢 Fixed
**Location**: `demo/spectre-editor/resources/scripts/interface/pages/modules/window.lua`
**Root Cause**: The UI script was serializing boolean fields as Strings (`"true"`) using `set_string()`, but the C++ backend (`window_module.cpp`) strictly parsed them using `.get<bool>()`. This type mismatch caused all window flags to silently fail and fall back to default during engine initialization. This has been patched to correctly write booleans while maintaining fallback compatibility.

### 3. C++ ECS Query Memory Leak (scenes_module_t)
**Severity**: Critical
**Location**: `spectre/source/modules/scenes/scenes_module.cpp` (`find_current_scenes`)
**Root Cause**: The method `find_current_scenes()` calls `m_world.query_builder<...>().build()` every single time it is invoked. In Flecs, creating a query dynamically without explicitly destroying it or caching it results in massive overhead and a permanent memory leak within the ECS world.
**Proposed Solution**: Initialize the query once during `scenes_module_t`'s constructor and store it as a class member variable (e.g., `m_current_scenes_query`), similar to how `renderer_module_t` caches `m_renderables_query`.

### 4. Lua UI Memory Leaks (sandbox.Properties:sub)
**Severity**: Critical
**Location**: `demo/spectre-editor/resources/scripts/interface/pages/modules/*.lua` & `externals/sandbox.lua`
**Root Cause**: The UI scripts extensively use `props:sub(...)` (e.g., in `scenes.lua`, `prefabs.lua`, `resources.lua`) to traverse JSON trees. In `sandbox.lua`, `sub()` creates a new C-struct handle that must be explicitly destroyed. The scripts completely fail to call `:destroy()` on these returned sub-objects, causing widespread memory leaks every time the user duplicates items or modifies properties.
**Proposed Solution**: 
1. Fix the immediate leaks by adding `:destroy()` calls to all `props:sub()` usages in the UI scripts.
2. Long-term fix: Implement a `__gc` metamethod in `sandbox.lua` for the `sandbox.Properties` metatable so that LuaJIT's garbage collector automatically frees the C handle when the Lua table falls out of scope.

### 5. Lua Editor Performance: 60FPS Disk I/O & JSON Parsing
**Severity**: Critical
**Location**: `demo/spectre-editor/resources/scripts/interface/pages/modules/components.lua` (`draw_dynamic_component`)
**Root Cause**: The function `load_schema()` reads the `.json` schema file from the disk and parses it into a `sandbox.Properties` object. However, `draw_dynamic_component()` invokes this function *every single frame* (60 FPS) when rendering dynamic components in the Prefab and Scene inspectors. This results in massive, constant Disk I/O and JSON parsing overhead on the main thread.
**Proposed Solution**: Cache the dynamically loaded schemas in a global Lua table and only reload them if the file is explicitly updated or modified.

### 6. C++ Engine Memory Leaks: Unfreed String Allocations
**Severity**: High
**Location**: `resources_module.cpp` (`deserialize_resource`) and `window_module.cpp` (`deserialize_window`)
**Root Cause**: Both modules allocate string buffers dynamically using `new char[size]` (e.g., `path_copy` and `title_copy`) to attach to `spectre_resource_component_t` and `spectre_window_component_t`. However, neither component has an `on_remove` observer registered in Flecs to free this memory, nor do they define C++ destructors. Consequently, whenever a window is destroyed or a resource is unloaded/reloaded, the string buffer leaks permanently.
**Proposed Solution**: Register a Flecs `on_remove` observer for both `spectre_resource_component_t` and `spectre_window_component_t` during module initialization to guarantee the `delete[]` cleanup of the allocated `char*` arrays.

### 7. ImGui Input Glitches & FFI Allocation Thrashing
**Severity**: High
**Location**: `components.lua`, `renderer.lua` (Various Drawers)
**Root Cause**: Text input buffers (e.g., `ffi.new("char[2048]")`) and numeric input buffers (e.g., `ffi.new("float[2]")`) are instantiated locally inside `on_render()`. Creating thousands of FFI cdata objects per second causes immense garbage collection thrashing in LuaJIT. Furthermore, recreating these buffers every frame passes a brand-new pointer to ImGui, which constantly breaks internal text cursor positioning and makes editing strings erratic or impossible.
**Proposed Solution**: Lift FFI buffer instantiations out of the render loop. Cache them as global or static state variables (e.g., `local temp_vec2 = ffi.new("float[2]")`) and simply overwrite their values using `ffi.copy` or assignment each frame before passing them to ImGui.

### 8. Memory Leaks: Unfreed Pointers on Empty File Reads
**Severity**: High
**Location**: `read_file` helper across `components.lua`, `prefabs.lua`, `renderer.lua`, `resources.lua`, `window.lua`
**Root Cause**: In the pattern used to read files, if `sandbox.filesystem.read_all_bytes(...)` returns true but the file is empty (`out_size == 0`), the condition `tonumber(out_size[0]) > 0` evaluates to false. This skips the `sandbox.filesystem.free_bytes()` block entirely, permanently leaking the C-buffer allocated for the empty file.
**Proposed Solution**: Ensure `free_bytes()` is always called if `out_data[0]` is not null, regardless of whether `out_size[0] > 0`.

### 9. Entity Duplication Fails to Deep Copy
**Severity**: High
**Location**: `demo/spectre-editor/resources/scripts/interface/pages/modules/prefabs.lua`
**Root Cause**: When duplicating an entity, the UI script simply executes `props:set_string(.../dummy, "0")` and immediately clears it. This hack initializes an empty JSON object (`{}`) instead of actually cloning the source entity's data.
**Proposed Solution**: Extract the source entity's properties via `props:sub(path)` and push the data to the new entity path using the `sandbox.Properties:merge()` C++ binding to ensure a flawless deep copy of all components and children.

### 10. Component Page Variable Deletion & Context Menus
**Severity**: High
**Location**: `demo/spectre-editor/resources/scripts/interface/pages/modules/components.lua`
**Root Cause**: 
- **Variable Deletion**: The script calls `table.remove(current_schema_keys, i)` while actively iterating over that exact table with `ipairs`. This shifts the array indices mid-loop, causing unpredictable skipping and UI glitches.
- **Component Title**: The main component title uses a static `imgui.Text()` element, preventing the user from right-clicking to manage the component itself.
**Proposed Solution**: Buffer variable deletion requests and process them *after* the `ipairs` loop finishes. Wrap the component title in a stylized `imgui.Selectable` or bounding box and attach a `BeginPopupContextItem` to expose Rename, Duplicate, and Delete actions for the component schema file.

### 11. Missing Broken Reference Validations
**Severity**: Medium
**Location**: `prefabs.lua`, `scripts.lua`, `scenes.lua`
**Root Cause**: If a dynamic component schema or Lua script is deleted/renamed from the filesystem, the editor's JSON still contains the stale string references. The UI blindly loops over these strings and fails silently (or displays raw text) when they don't resolve.
**Proposed Solution**: Actively validate script function names against the `get_available_scripts()` cache and component schemas against `list_dynamic_components()`. If a reference is broken, render a high-visibility red error icon (`⚠️ Missing Reference`) to alert the user. Extend this to entities that are actively referenced by other systems but have been deleted.

### 12. Resource Path Validation (Folder vs. File)
**Severity**: Low
**Location**: `demo/spectre-editor/resources/scripts/interface/pages/modules/resources.lua`
**Root Cause**: The resource path input field validates whether a path *exists*, but it does not restrict the path to being a file. Users can accidentally bind directories.
**Proposed Solution**: Utilize `sandbox.filesystem.is_directory(world, path)` during validation. If true, display a specific error: *"Path must point to a file, not a folder."*

---

## Phase 2: Editor Workflow & UI Enhancements

### 1. Component Management Ergonomics
- **Global Searchable Add Menu**: Add an `imgui.InputText` search bar at the top of the "Add Component" popup menus across all pages (Prefabs, Scenes, etc.) to allow rapid filtering of long component lists.
- **Component Sections/Folders**: Introduce the ability to categorize dynamic components into logical Sections/Folders in the UI, complete with section-level Add/Duplicate/Delete context menus.
- **Script Drawer Overhaul**: Redesign the script component drawer (`Drawers["scripts"]`). Standardize paddings, improve the visual hierarchy of lifecycle hooks, and replace the basic layout with a more polished, modern aesthetic.

### 2. Scene & Prefab Hierarchy Polish
- **Context-Menu Driven Creation**: Remove the standalone "New Scene" and "New State" buttons. Migrate creation actions to intuitive right-click context menus attached to the root nodes or subsections.
- **Hierarchy Root Node**: Inject a visual "Root" node at the very top of the Prefabs and Scene inspector boxes to clearly represent the top-level container.
- **Alphabetical Sorting**: Automatically execute `table.sort` on the Scene and State navigation lists.
- **Manual Ordering**: Implement ImGui drag-and-drop payload logic to allow manual reordering of items directly within the Scene and State inspectors.

### 3. Editor Configuration & Project Meta
- **Dedicated Info Page**: Create an "Info" module page to directly edit the project`s `configuration.json` file. This should expose high-level project metadata for editing, including the application Name, Version, Description, and required engine modules.
- **Arguments Page**: Implement an Arguments page to configure and pass command-line arguments to the launched project.

---

## Phase 3: Engine Architecture & Lua Developer Experience (DX)

### 1. Lua Scripting: Dynamic Component Access (DX Overhaul)
**Severity**: High (Developer Experience)
**Location**: `demo/spectre-editor/resources/scripts/externals/ecs.lua` & `spectre.lua`
**Problem**: Utilizing Dynamic Components inside Lua scripts is currently highly unergonomic and messy. Because dynamic components are generated at runtime in C++ using memory offsets (`spectre_dynamic_component_header_t` + raw data), Lua scripts are forced to rely on dangerous and ugly pointer arithmetic (`ffi.cast`) to read or write variables.
**Proposed Solution**: Implement a clean Lua abstraction wrapper. By leveraging Lua Metatables (`__index` and `__newindex`), we can seamlessly map C++ memory offsets to table keys. 
*Example implementation target:*
```lua
-- Messy current approach:
local ptr = ffi.cast("char*", entity:get(dyn_comp_id))
local val = ffi.cast("float*", ptr + offset)[0]

-- Proposed polished API:
local my_comp = entity:dynamic("MyCustomComponent")
my_comp.speed = 50.5
print(my_comp.health)
```

### 2. Lua Module Architecture Standardization
**Location**: `demo/spectre-editor/resources/scripts/interface/pages/modules/*.lua`
**Problem**: The codebase for Editor UI modules currently lacks structural consistency. Local variables, helper functions, and exported APIs are mixed throughout the files unpredictably. This makes the code difficult to read, navigate, and maintain across different module pages.
**Proposed Solution**: Standardize all `.lua` modules to follow a strict "Declaration/Implementation" architecture (similar to C++ headers/sources):
1. **Header/Declarations (Top)**: Place all module table definitions, required dependencies, forward declarations of local functions, and UI state variables at the absolute top of the file.
2. **Implementations (Bottom)**: Place the actual function bodies and complex logic at the bottom of the file. This allows developers to read the top of the file to instantly understand what the module does and what state it holds, without getting bogged down in ImGui rendering code.

### 3. LuaRocks / Vendor Dependency System
**Location**: Project Management UI
**Problem**: The engine lacks a streamlined way to manage external Lua packages.
**Proposed Solution**: Integrate a dependency management system in the Editor UI (likely on the new Info page) to resolve, vendor, and package LuaRocks dependencies directly into the project's virtual file system.

### 4. Built-In Script Editor
**Location**: New Editor Module
**Problem**: Users must rely on external IDEs to edit script logic, breaking the flow of using the Spectre Editor.
**Proposed Solution**: Embed a basic, robust code editor (e.g., using an ImGui color-text-edit extension) directly within the IDE for viewing and modifying `.lua` files on the fly.

### 5. Shape Texturing API
**Location**: `spectre/source/modules/renderer/renderer_module.cpp`
**Problem**: Fundamental rendering primitives (`CircleRenderable`, `RectangleRenderable`, `PolygoneRenderable`) do not currently support applying textures.
**Proposed Solution**: Enhance the C++ rendering backend to pass UV coordinates and bind texture uniforms when drawing shapes (e.g., utilizing `DrawTexturePoly` or custom shader bindings in Raylib), and expose these UV/Texture parameters to the respective UI drawers.

### 6. FFI Encapsulation & Lua Abstraction
**Location**: `demo/spectre-editor/resources/scripts/` vs `externals/`
**Problem**: Application and UI Lua scripts currently rely heavily on `require('ffi')` directly to interact with C-structs, allocate buffers, and call the engine ABI. This breaks abstraction boundaries, making high-level UI logic extremely verbose, unsafe, and fragile.
**Proposed Solution**: Enforce a strict architectural boundary: NO Lua file outside of the `externals/` directory should ever `require("ffi")`. The `externals/` scripts (such as `sandbox.lua`, `spectre.lua`, and `imgui.lua`) must provide clean, high-level wrapper objects and functions for all C-API interactions and memory allocations.

### 4. In-Editor Console & Log Viewer
**Location**: New Editor Module (e.g., `interface/pages/modules/console.lua`)
**Problem**: The editor currently relies entirely on the external terminal/stdout to display engine logs, warnings, and errors. This is poor UX, especially for debugging Lua scripts where users need to see immediate feedback.
**Proposed Solution**: Create an internal ImGui Console panel that captures and renders logs (`sandbox.logs.info`, `error`, etc.) directly within the Editor GUI, complete with severity filtering and search functionality.

### 5. Play / Pause / Stop Toolbar
**Location**: Main Editor Window (`interface/views/editor.lua`)
**Problem**: The "Run" function is hidden in the `File -> Run` dropdown menu, making rapid iterative testing cumbersome.
**Proposed Solution**: Implement a centered ImGui Toolbar at the top of the screen (similar to Unity/Godot) with dedicated Play, Pause, and Stop buttons to instantly launch and halt the active project.

### 6. Universal Copy / Paste
**Location**: Global Shortcut Interceptor (`editor.lua`)
**Problem**: While entities can be duplicated, they cannot be copied to a clipboard buffer and pasted across different scenes or prefabs.
**Proposed Solution**: Implement a global `Ctrl+C` and `Ctrl+V` serialization buffer. When an entity or component is selected, serialize its `sandbox.Properties` payload into a global table. When pasted, deserialize that payload into the current target node.

### 7. Visual Entity Parenting (Drag & Drop Hierarchy)
**Location**: `scenes.lua` and `prefabs.lua`
**Problem**: Flecs natively supports standard `(ChildOf, Parent)` entity hierarchies, but the UI completely lacks the ability to structure them via drag and drop.
**Proposed Solution**: Implement ImGui drag-and-drop payloads (`BeginDragDropSource`, `BeginDragDropTarget`) on entity tree nodes in the inspectors, allowing users to rapidly drag entities into each other to create parent-child relationships.
