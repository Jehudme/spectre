# Window Module

## Overview

The **Window Module** in the Spectre Game Engine provides a unified interface for managing the main application window and querying core user input (keyboard and mouse). It abstracts away the underlying platform-specific windowing system, allowing you to easily control window dimensions, display modes (fullscreen, borderless, maximized), title, vsync, and cursor behavior.

Additionally, this module provides polling-based input queries for keyboard keys and mouse movement, which are synchronized per-frame.

The window state is deeply integrated into Spectre's ECS, allowing it to be represented and serialized via standard entity components (like `spectre_window_component_t`).

---

## Table of Contents

1. [Best Practices & Warnings](#best-practices--warnings)
2. [C++ SDK API Reference](#c-sdk-api-reference)
3. [Lua API Reference](#lua-api-reference)
4. [Component Reference](#component-reference)

---

## Best Practices & Warnings

> [!WARNING]
> **Thread Safety:** Most window manipulation functions (resizing, title changing) and input querying functions interact with the underlying OS windowing system. They should typically be called from the **main thread** or during safe ECS phases to prevent race conditions or undefined behavior.

> [!CAUTION]
> **Cursor Locking:** When using `set_cursor_locked(true)` to hide and capture the mouse (e.g., for a first-person camera), ensure that you provide a mechanism (such as pressing `ESC`) to unlock the cursor. Otherwise, the user will be trapped within the game window.

> [!TIP]
> **Component Synchronization:** While the window state is mirrored in a `spectre_window_component_t`, prefer using the C++ SDK or Lua API to modify window properties rather than mutating the component directly. The API ensures that the native platform window is updated synchronously.

---

## C++ SDK API Reference

The C++ API is accessible via the `spectre::modules::window` class, which requires a reference to the flecs `world` for most operations.

### Window Management

#### `void set_title(const flecs::world& world, const char* title)`
Changes the text displayed in the title bar of the window.
- **`world`**: The ECS world.
- **`title`**: A null-terminated C-string representing the new title.

#### `void set_size(const flecs::world& world, uint32_t width, uint32_t height)`
Resizes the window's client area to the specified dimensions.
- **`world`**: The ECS world.
- **`width`**: The desired width in pixels.
- **`height`**: The desired height in pixels.

#### `void set_size_limits(const flecs::world& world, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height)`
Restricts the window from being resized beyond the specified bounds.

#### `uint32_t get_width(const flecs::world& world)` / `uint32_t get_height(const flecs::world& world)`
Returns the current width or height of the window.

#### `float get_aspect_ratio(const flecs::world& world)`
Returns the width divided by the height.

#### `void set_position(const flecs::world& world, int32_t x, int32_t y)`
Moves the window to the specified screen coordinates.

#### `int32_t get_position_x(const flecs::world& world)` / `int32_t get_position_y(const flecs::world& world)`
Returns the current X or Y screen coordinate of the window.

#### `bool should_close(const flecs::world& world)`
Returns `true` if the OS has requested the window to close (e.g., the user clicked the 'X' button).

#### `void set_should_close(const flecs::world& world, bool close)`
Forcefully sets or clears the close request flag.

### Display Modes

#### `void set_fullscreen(const flecs::world& world, bool fullscreen)`
Toggles exclusive fullscreen mode.

#### `void set_borderless(const flecs::world& world, bool borderless)`
Toggles borderless windowed mode.

#### `void set_vsync(const flecs::world& world, bool enabled)`
Enables or disables Vertical Synchronization (VSync) to cap the framerate to the monitor's refresh rate.

#### `void set_resizable(const flecs::world& world, bool resizable)`
Enables or disables the user's ability to drag the window edges to resize it.

#### `void set_always_on_top(const flecs::world& world, bool always_on_top)`
Forces the window to stay above all other application windows.

#### `void minimize(const flecs::world& world)` / `void maximize(const flecs::world& world)` / `void restore(const flecs::world& world)`
Controls the window's minimized/maximized state. `restore` returns the window to its normal state.

### Cursor Management

#### `void set_cursor_visible(const flecs::world& world, bool visible)`
Shows or hides the system mouse cursor when over the window.

#### `void set_cursor_locked(const flecs::world& world, bool locked)`
Locks the cursor to the center of the window (ideal for 3D camera controls) and prevents it from leaving the client area.

### Input Querying

> Note: Keycodes typically follow standard ASCII or framework-specific definitions (e.g., Raylib keys).

#### `bool is_key_down(const flecs::world& world, int keycode)`
Returns `true` if the key is currently being held down.

#### `bool is_key_pressed(const flecs::world& world, int keycode)`
Returns `true` only during the exact frame the key was initially pressed.

#### `bool is_key_released(const flecs::world& world, int keycode)`
Returns `true` only during the exact frame the key was released.

#### `float get_mouse_x(const flecs::world& world)` / `float get_mouse_y(const flecs::world& world)`
Returns the current X and Y coordinates of the mouse relative to the window.

#### `float get_mouse_delta_x(const flecs::world& world)` / `float get_mouse_delta_y(const flecs::world& world)`
Returns the difference in mouse position since the last frame. Essential for camera rotation.

#### `void* get_native_handle(const flecs::world& world)`
Returns an opaque pointer to the underlying OS window handle (e.g., `HWND` on Windows, `GLFWwindow*` on Linux).

### Example (C++)

```cpp
#include <spectre/sdk/window.hpp>

void MyGameplaySystem(flecs::iter& it) {
    auto world = it.world();

    // Check if the user pressed Escape to close the game
    if (spectre::modules::window::is_key_pressed(world, 256 /* KEY_ESCAPE */)) {
        spectre::modules::window::set_should_close(world, true);
    }

    // Toggle fullscreen with F11
    if (spectre::modules::window::is_key_pressed(world, 300 /* KEY_F11 */)) {
        bool is_fs = spectre::modules::window::is_fullscreen(world);
        spectre::modules::window::set_fullscreen(world, !is_fs);
    }

    // Process mouse movement
    float dx = spectre::modules::window::get_mouse_delta_x(world);
    float dy = spectre::modules::window::get_mouse_delta_y(world);
    
    if (dx != 0.0f || dy != 0.0f) {
        // Rotate camera...
    }
}
```

---

## Lua API Reference

The Window module is heavily bound to Lua under the `spectre.window` namespace. All methods mirror the C++ behavior and take the ECS world as the first argument.

### Lua Methods Overview
- `spectre.window.set_title(world, title)`
- `spectre.window.set_size(world, width, height)`
- `spectre.window.get_width(world)` / `spectre.window.get_height(world)`
- `spectre.window.get_aspect_ratio(world)`
- `spectre.window.set_position(world, x, y)`
- `spectre.window.should_close(world)`
- `spectre.window.set_should_close(world, close)`
- `spectre.window.set_fullscreen(world, fullscreen)`
- `spectre.window.set_vsync(world, enabled)`
- `spectre.window.set_cursor_locked(world, locked)`
- `spectre.window.set_cursor_visible(world, visible)`
- `spectre.window.is_key_pressed(world, keycode)`
- `spectre.window.is_key_down(world, keycode)`
- `spectre.window.get_mouse_delta_x(world)` / `spectre.window.get_mouse_delta_y(world)`
- *And all other methods exposed in the C++ API.*

### Example (Lua)

```lua
local function OnUpdate(world, delta_time)
    -- Poll for input
    local KEY_SPACE = 32
    if spectre.window.is_key_pressed(world, KEY_SPACE) then
        print("Player jumped!")
    end

    -- Dynamically update title based on FPS
    local title = string.format("My Game - FPS: %d", 1.0 / delta_time)
    spectre.window.set_title(world, title)

    -- Mouse looking
    local dx = spectre.window.get_mouse_delta_x(world)
    if dx ~= 0.0 then
        -- rotate player...
    end
end
```

---

## Component Reference

The state of the window is tracked within the ECS via the `spectre_window_component_t` structure. This is particularly useful for serialization and editor inspection.

```c
typedef struct spectre_window_component_t {
    uint32_t width;
    uint32_t height;
    int32_t position_x;
    int32_t position_y;
    uint32_t min_width;
    uint32_t min_height;
    uint32_t max_width;
    uint32_t max_height;
    const char* title;
    bool vsync;
    bool fullscreen;
    bool borderless;
    bool resizable;
    bool always_on_top;
    bool minimized;
    bool maximized;
    bool visible;
    bool focused;
    bool hovered;
    bool cursor_visible;
    bool cursor_locked;
    void* native_handle;
} spectre_window_component_t;
```

Similarly, the raw input states are stored per-frame in `spectre_input_state_t`.

> **Note:** Modification of the underlying engine components directly is strongly discouraged unless you are implementing an engine subsystem. User logic should exclusively use the C++ `spectre::modules::window` or the Lua `spectre.window` wrapper API.
