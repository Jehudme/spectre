#include <spectre/abi/window_service.h>
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/sdk/logs.hpp>
#include <flecs.h>
#include <raylib.h>
#include <cstring>

#include "window_module.h"

namespace spectre::module {

WindowModule::WindowModule(flecs::world& ecs) : m_entity_world(ecs) {
    sandbox::modules::logs::trace(m_entity_world, "WindowModule: Initializing raylib window...");
    
    InitWindow(1280, 720, m_title);
    SetTargetFPS(60);

    m_entity_world.component<spectre_input_state_t>("spectre_input_state_t");
    m_entity_world.set<spectre_input_state_t>({});

    m_window_system = m_entity_world.system<spectre_input_state_t>()
        .kind(flecs::PreUpdate)
        .each([this](flecs::entity e, spectre_input_state_t& state) {
            std::memset(state.keys_pressed, 0, sizeof(state.keys_pressed));
            std::memset(state.keys_released, 0, sizeof(state.keys_released));
            std::memset(state.keys_down, 0, sizeof(state.keys_down));

            for (int key = 0; key < 512; ++key) {
                int bucket = key / 64;
                uint64_t bitmask = 1ULL << (key % 64);
                
                if (IsKeyDown(key)) {
                    state.keys_down[bucket] |= bitmask;
                }
                if (IsKeyPressed(key)) {
                    state.keys_pressed[bucket] |= bitmask;
                }
                if (IsKeyReleased(key)) {
                    state.keys_released[bucket] |= bitmask;
                }
            }

            auto mouse_pos = GetMousePosition();
            state.mouse_position[0] = mouse_pos.x;
            state.mouse_position[1] = mouse_pos.y;

            auto mouse_delta = GetMouseDelta();
            state.mouse_delta[0] = mouse_delta.x;
            state.mouse_delta[1] = mouse_delta.y;
        });
}

WindowModule::~WindowModule() {
    CloseWindow();
}

bool WindowModule::should_close() const { return WindowShouldClose(); }
void WindowModule::set_should_close(bool close) { /* Not supported natively by Raylib to force close flag other than calling CloseWindow, handled outside */ }

void WindowModule::set_size(uint32_t width, uint32_t height) { SetWindowSize(width, height); }
void WindowModule::set_position(int32_t x, int32_t y) { SetWindowPosition(x, y); }
void WindowModule::set_size_limits(uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) { SetWindowMinSize(min_width, min_height); SetWindowMaxSize(max_width, max_height); }

uint32_t WindowModule::get_width() const { return GetScreenWidth(); }
uint32_t WindowModule::get_height() const { return GetScreenHeight(); }
float WindowModule::get_aspect_ratio() const { return (float)GetScreenWidth() / (float)GetScreenHeight(); }
int32_t WindowModule::get_position_x() const { return GetWindowPosition().x; }
int32_t WindowModule::get_position_y() const { return GetWindowPosition().y; }

void WindowModule::set_title(const char* title) { m_title = title; SetWindowTitle(title); }
void WindowModule::set_vsync(bool enabled) { if (enabled && !is_vsync()) SetWindowState(FLAG_VSYNC_HINT); else if (!enabled && is_vsync()) ClearWindowState(FLAG_VSYNC_HINT); }
void WindowModule::set_fullscreen(bool fullscreen) { if (fullscreen != is_fullscreen()) ToggleFullscreen(); }
void WindowModule::set_borderless(bool borderless) { if (borderless) SetWindowState(FLAG_WINDOW_UNDECORATED); else ClearWindowState(FLAG_WINDOW_UNDECORATED); }
void WindowModule::set_resizable(bool resizable) { if (resizable) SetWindowState(FLAG_WINDOW_RESIZABLE); else ClearWindowState(FLAG_WINDOW_RESIZABLE); }
void WindowModule::set_always_on_top(bool always_on_top) { if (always_on_top) SetWindowState(FLAG_WINDOW_TOPMOST); else ClearWindowState(FLAG_WINDOW_TOPMOST); }

const char* WindowModule::get_title() const { return m_title; }
bool WindowModule::is_vsync() const { return IsWindowState(FLAG_VSYNC_HINT); }
bool WindowModule::is_fullscreen() const { return IsWindowFullscreen(); }
bool WindowModule::is_borderless() const { return IsWindowState(FLAG_WINDOW_UNDECORATED); }
bool WindowModule::is_resizable() const { return IsWindowState(FLAG_WINDOW_RESIZABLE); }

void WindowModule::minimize() { MinimizeWindow(); }
void WindowModule::maximize() { MaximizeWindow(); }
void WindowModule::restore() { RestoreWindow(); }
void WindowModule::show() { ClearWindowState(FLAG_WINDOW_HIDDEN); }
void WindowModule::hide() { SetWindowState(FLAG_WINDOW_HIDDEN); }
void WindowModule::request_attention() { /* Raylib doesn't natively have request attention, no-op */ }

bool WindowModule::is_minimized() const { return IsWindowMinimized(); }
bool WindowModule::is_maximized() const { return IsWindowMaximized(); }
bool WindowModule::is_visible() const { return !IsWindowHidden(); }
bool WindowModule::is_focused() const { return IsWindowFocused(); }
bool WindowModule::is_hovered() const { return IsCursorOnScreen(); }

void WindowModule::set_cursor_visible(bool visible) { if (visible) ShowCursor(); else HideCursor(); }
void WindowModule::set_cursor_locked(bool locked) { if (locked) DisableCursor(); else EnableCursor(); }
bool WindowModule::is_cursor_visible() const { return !IsCursorHidden(); }
bool WindowModule::is_cursor_locked() const { return !IsCursorHidden(); /* Raylib doesn't expose locked explicitly, usually DisableCursor hides it */ }

void* WindowModule::get_native_handle() const { return GetWindowHandle(); }

} // namespace spectre::module

// ─────────────────────────────────────────────────────────────────────────────
// C-ABI Implementation
// ─────────────────────────────────────────────────────────────────────────────

extern "C" {
    static bool abi_is_key_down(const spectre_input_state_t* state, int key) {
        if (!state || key < 0 || key >= 512) return false;
        return (state->keys_down[key / 64] & (1ULL << (key % 64))) != 0;
    }

    static bool abi_is_key_pressed(const spectre_input_state_t* state, int key) {
        if (!state || key < 0 || key >= 512) return false;
        return (state->keys_pressed[key / 64] & (1ULL << (key % 64))) != 0;
    }

    static bool abi_is_key_released(const spectre_input_state_t* state, int key) {
        if (!state || key < 0 || key >= 512) return false;
        return (state->keys_released[key / 64] & (1ULL << (key % 64))) != 0;
    }

    static void abi_get_mouse_position(const spectre_input_state_t* state, float* x, float* y) {
        if (state) {
            if (x) *x = state->mouse_position[0];
            if (y) *y = state->mouse_position[1];
        }
    }

    static void abi_get_mouse_delta(const spectre_input_state_t* state, float* dx, float* dy) {
        if (state) {
            if (dx) *dx = state->mouse_delta[0];
            if (dy) *dy = state->mouse_delta[1];
        }
    }

    static bool abi_should_window_close(void) { return WindowShouldClose(); }
    static void abi_set_should_close(bool close) { /* N/A */ }
    static void abi_set_size(uint32_t width, uint32_t height) { SetWindowSize(width, height); }
    static void abi_set_position(int32_t x, int32_t y) { SetWindowPosition(x, y); }
    static void abi_set_size_limits(uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) { SetWindowMinSize(min_width, min_height); SetWindowMaxSize(max_width, max_height); }
    static uint32_t abi_get_width(void) { return GetScreenWidth(); }
    static uint32_t abi_get_height(void) { return GetScreenHeight(); }
    static float abi_get_aspect_ratio(void) { return (float)GetScreenWidth() / (float)GetScreenHeight(); }
    static int32_t abi_get_position_x(void) { return GetWindowPosition().x; }
    static int32_t abi_get_position_y(void) { return GetWindowPosition().y; }
    static void abi_set_title(const char* title) { SetWindowTitle(title); }
    static void abi_set_vsync(bool enabled) { if (enabled && !IsWindowState(FLAG_VSYNC_HINT)) SetWindowState(FLAG_VSYNC_HINT); else if (!enabled && IsWindowState(FLAG_VSYNC_HINT)) ClearWindowState(FLAG_VSYNC_HINT); }
    static void abi_set_fullscreen(bool fullscreen) { if (fullscreen != IsWindowFullscreen()) ToggleFullscreen(); }
    static void abi_set_borderless(bool borderless) { if (borderless) SetWindowState(FLAG_WINDOW_UNDECORATED); else ClearWindowState(FLAG_WINDOW_UNDECORATED); }
    static void abi_set_resizable(bool resizable) { if (resizable) SetWindowState(FLAG_WINDOW_RESIZABLE); else ClearWindowState(FLAG_WINDOW_RESIZABLE); }
    static void abi_set_always_on_top(bool always_on_top) { if (always_on_top) SetWindowState(FLAG_WINDOW_TOPMOST); else ClearWindowState(FLAG_WINDOW_TOPMOST); }
    static const char* abi_get_title(void) { return "Spectre Engine"; } // Raylib doesn't expose get title trivially, return default or empty
    static bool abi_is_vsync(void) { return IsWindowState(FLAG_VSYNC_HINT); }
    static bool abi_is_fullscreen(void) { return IsWindowFullscreen(); }
    static bool abi_is_borderless(void) { return IsWindowState(FLAG_WINDOW_UNDECORATED); }
    static bool abi_is_resizable(void) { return IsWindowState(FLAG_WINDOW_RESIZABLE); }
    static void abi_minimize(void) { MinimizeWindow(); }
    static void abi_maximize(void) { MaximizeWindow(); }
    static void abi_restore(void) { RestoreWindow(); }
    static void abi_show(void) { ClearWindowState(FLAG_WINDOW_HIDDEN); }
    static void abi_hide(void) { SetWindowState(FLAG_WINDOW_HIDDEN); }
    static void abi_request_attention(void) { /* N/A */ }
    static bool abi_is_minimized(void) { return IsWindowMinimized(); }
    static bool abi_is_maximized(void) { return IsWindowMaximized(); }
    static bool abi_is_visible(void) { return !IsWindowHidden(); }
    static bool abi_is_focused(void) { return IsWindowFocused(); }
    static bool abi_is_hovered(void) { return IsCursorOnScreen(); }
    static void abi_set_cursor_visible(bool visible) { if (visible) ShowCursor(); else HideCursor(); }
    static void abi_set_cursor_locked(bool locked) { if (locked) DisableCursor(); else EnableCursor(); }
    static bool abi_is_cursor_visible(void) { return !IsCursorHidden(); }
    static bool abi_is_cursor_locked(void) { return !IsCursorHidden(); }
    static void* abi_get_native_handle(void) { return GetWindowHandle(); }
}

static spectre_window_input_api_t g_window_input_api = {
    .is_key_down = abi_is_key_down,
    .is_key_pressed = abi_is_key_pressed,
    .is_key_released = abi_is_key_released,
    .get_mouse_position = abi_get_mouse_position,
    .get_mouse_delta = abi_get_mouse_delta,
    .should_window_close = abi_should_window_close,
    .set_should_close = abi_set_should_close,
    .set_size = abi_set_size,
    .set_position = abi_set_position,
    .set_size_limits = abi_set_size_limits,
    .get_width = abi_get_width,
    .get_height = abi_get_height,
    .get_aspect_ratio = abi_get_aspect_ratio,
    .get_position_x = abi_get_position_x,
    .get_position_y = abi_get_position_y,
    .set_title = abi_set_title,
    .set_vsync = abi_set_vsync,
    .set_fullscreen = abi_set_fullscreen,
    .set_borderless = abi_set_borderless,
    .set_resizable = abi_set_resizable,
    .set_always_on_top = abi_set_always_on_top,
    .get_title = abi_get_title,
    .is_vsync = abi_is_vsync,
    .is_fullscreen = abi_is_fullscreen,
    .is_borderless = abi_is_borderless,
    .is_resizable = abi_is_resizable,
    .minimize = abi_minimize,
    .maximize = abi_maximize,
    .restore = abi_restore,
    .show = abi_show,
    .hide = abi_hide,
    .request_attention = abi_request_attention,
    .is_minimized = abi_is_minimized,
    .is_maximized = abi_is_maximized,
    .is_visible = abi_is_visible,
    .is_focused = abi_is_focused,
    .is_hovered = abi_is_hovered,
    .set_cursor_visible = abi_set_cursor_visible,
    .set_cursor_locked = abi_set_cursor_locked,
    .is_cursor_visible = abi_is_cursor_visible,
    .is_cursor_locked = abi_is_cursor_locked,
    .get_native_handle = abi_get_native_handle
};

SANDBOX_DEFINE_SERVICE(spectre_window_input_service_t, spectre_window_input_api_t, &g_window_input_api)

namespace spectre::module {

    static sandbox_requirement_info_t window_requirements[] = {
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "logs",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(WindowModule, {
        .name          = "window",
        .description   = "Creates application window and provides unified bitwise input state.",
        .architecture  = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service       = &spectre_window_input_service_t_info,
        .requirements  = window_requirements,
        .requirement_count = 1
    })

} // namespace spectre::module
