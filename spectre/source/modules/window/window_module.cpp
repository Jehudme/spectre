#include "window_module.h"
#include <spectre/abi/window_service.h>
#include <sandbox/sdk/configuration.hpp>
#include <sandbox/sdk/logs.hpp>
#include <sandbox/sdk/runtime.hpp>
#include <stdexcept>
#include <raylib.h>
#include <string>

namespace spectre::module {

    WindowModule::WindowModule(flecs::world& world) : m_entity_world(world) {
        try {
            sandbox::modules::logs::trace(m_entity_world, "Window Module: Initializing...");

            // Fetch configuration with sensible defaults
            uint32_t width = sandbox::modules::configuration::get<uint32_t>(m_entity_world, "window/width").value_or(1280);
            uint32_t height = sandbox::modules::configuration::get<uint32_t>(m_entity_world, "window/height").value_or(720);
            std::string title = sandbox::modules::configuration::get<std::string>(m_entity_world, "window/title").value_or("Spectre Window");
            uint32_t fps = sandbox::modules::configuration::get<uint32_t>(m_entity_world, "window/fps").value_or(60);

            sandbox::modules::logs::info(m_entity_world, "Window Module: Creating {}x{} window titled '{}'", width, height, title);
            
            InitWindow(static_cast<int>(width), static_cast<int>(height), title.c_str());
            SetTargetFPS(static_cast<int>(fps));
            
            m_window_system = m_entity_world.system("WindowSystem")
                .kind(flecs::OnUpdate)
                .run([this](flecs::iter& it) {
                    if (WindowShouldClose()) {
                        sandbox::modules::logs::info(m_entity_world, "Window Module: Close requested, stopping runtime.");
                        sandbox::modules::runtime::stop(m_entity_world);
                    }
                });
            
            sandbox::modules::logs::info(m_entity_world, "Window Module: Successfully initialized raylib window.");
        } catch (const std::exception& e) {
            sandbox::modules::logs::error(m_entity_world, "Window Module: Exception during initialization - {}", e.what());
            throw;
        }
    }
    
    WindowModule::~WindowModule() {
        if (m_window_system.is_valid()) {
            m_window_system.destruct();
        }
        
        if (IsWindowReady()) {
            sandbox::modules::logs::trace(m_entity_world, "Window Module: Closing window...");
            CloseWindow();
        }
    }

    bool WindowModule::should_close() const { return WindowShouldClose(); }
    void WindowModule::set_should_close(bool close) {} // unsupported directly in raylib
    void WindowModule::set_size(uint32_t width, uint32_t height) { SetWindowSize(static_cast<int>(width), static_cast<int>(height)); }
    void WindowModule::set_position(int32_t x, int32_t y) { SetWindowPosition(x, y); }
    void WindowModule::set_size_limits(uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
        SetWindowMinSize(static_cast<int>(min_width), static_cast<int>(min_height));
        SetWindowMaxSize(static_cast<int>(max_width), static_cast<int>(max_height));
    }
    uint32_t WindowModule::get_width() const { return static_cast<uint32_t>(GetScreenWidth()); }
    uint32_t WindowModule::get_height() const { return static_cast<uint32_t>(GetScreenHeight()); }
    float WindowModule::get_aspect_ratio() const { return (float)GetScreenWidth() / (float)GetScreenHeight(); }
    int32_t WindowModule::get_position_x() const { return GetWindowPosition().x; }
    int32_t WindowModule::get_position_y() const { return GetWindowPosition().y; }
    void WindowModule::set_title(const char* title) { if (title) SetWindowTitle(title); }
    void WindowModule::set_vsync(bool enabled) { if (enabled) SetWindowState(FLAG_VSYNC_HINT); else ClearWindowState(FLAG_VSYNC_HINT); }
    void WindowModule::set_fullscreen(bool fullscreen) {
        if ((fullscreen && !IsWindowFullscreen()) || (!fullscreen && IsWindowFullscreen())) {
            ToggleFullscreen();
        }
    }
    void WindowModule::set_borderless(bool borderless) { if (borderless) SetWindowState(FLAG_WINDOW_UNDECORATED); else ClearWindowState(FLAG_WINDOW_UNDECORATED); }
    void WindowModule::set_resizable(bool resizable) { if (resizable) SetWindowState(FLAG_WINDOW_RESIZABLE); else ClearWindowState(FLAG_WINDOW_RESIZABLE); }
    void WindowModule::set_always_on_top(bool always_on_top) { if (always_on_top) SetWindowState(FLAG_WINDOW_TOPMOST); else ClearWindowState(FLAG_WINDOW_TOPMOST); }
    const char* WindowModule::get_title() const { return "Spectre Window"; } // Raylib lacks a getter for window title
    bool WindowModule::is_vsync() const { return IsWindowState(FLAG_VSYNC_HINT); }
    bool WindowModule::is_fullscreen() const { return IsWindowFullscreen(); }
    bool WindowModule::is_borderless() const { return IsWindowState(FLAG_WINDOW_UNDECORATED); }
    bool WindowModule::is_resizable() const { return IsWindowState(FLAG_WINDOW_RESIZABLE); }
    void WindowModule::minimize() { MinimizeWindow(); }
    void WindowModule::maximize() { MaximizeWindow(); }
    void WindowModule::restore() { RestoreWindow(); }
    void WindowModule::show() { ClearWindowState(FLAG_WINDOW_HIDDEN); }
    void WindowModule::hide() { SetWindowState(FLAG_WINDOW_HIDDEN); }
    void WindowModule::request_attention() {}
    bool WindowModule::is_minimized() const { return IsWindowMinimized(); }
    bool WindowModule::is_maximized() const { return IsWindowMaximized(); }
    bool WindowModule::is_visible() const { return !IsWindowHidden(); }
    bool WindowModule::is_focused() const { return IsWindowFocused(); }
    bool WindowModule::is_hovered() const { return IsWindowFocused(); }
    void WindowModule::set_cursor_visible(bool visible) { if (visible) ShowCursor(); else HideCursor(); }
    void WindowModule::set_cursor_locked(bool locked) { if (locked) DisableCursor(); else EnableCursor(); }
    bool WindowModule::is_cursor_visible() const { return !IsCursorHidden(); }
    bool WindowModule::is_cursor_locked() const { return IsCursorHidden(); }
    void* WindowModule::get_native_handle() const { return GetWindowHandle(); }
}

// ----------------------------------------------------------------------------
// C-ABI Implementations (Bridge to C++)
// ----------------------------------------------------------------------------
using spectre::module::WindowModule;

static bool should_close_impl(const void* inst) {
    if (!inst) return false;
    try {
        return static_cast<WindowModule*>(const_cast<void*>(inst))->should_close();
    } catch (...) { return false; }
}

static void set_should_close_impl(void* inst, bool close) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_should_close(close); } catch (...) {}
}

static void set_size_impl(void* inst, uint32_t width, uint32_t height) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_size(width, height); } catch (...) {}
}

static void set_position_impl(void* inst, int32_t x, int32_t y) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_position(x, y); } catch (...) {}
}

static void set_size_limits_impl(void* inst, uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_size_limits(min_width, min_height, max_width, max_height); } catch (...) {}
}

static uint32_t get_width_impl(const void* inst) {
    if (!inst) return 0;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->get_width(); } catch (...) { return 0; }
}

static uint32_t get_height_impl(const void* inst) {
    if (!inst) return 0;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->get_height(); } catch (...) { return 0; }
}

static float get_aspect_ratio_impl(const void* inst) {
    if (!inst) return 0.0f;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->get_aspect_ratio(); } catch (...) { return 0.0f; }
}

static int32_t get_position_x_impl(const void* inst) {
    if (!inst) return 0;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->get_position_x(); } catch (...) { return 0; }
}

static int32_t get_position_y_impl(const void* inst) {
    if (!inst) return 0;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->get_position_y(); } catch (...) { return 0; }
}

static void set_title_impl(void* inst, const char* title) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_title(title); } catch (...) {}
}

static void set_vsync_impl(void* inst, bool enabled) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_vsync(enabled); } catch (...) {}
}

static void set_fullscreen_impl(void* inst, bool fullscreen) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_fullscreen(fullscreen); } catch (...) {}
}

static void set_borderless_impl(void* inst, bool borderless) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_borderless(borderless); } catch (...) {}
}

static void set_resizable_impl(void* inst, bool resizable) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_resizable(resizable); } catch (...) {}
}

static void set_always_on_top_impl(void* inst, bool always_on_top) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_always_on_top(always_on_top); } catch (...) {}
}

static const char* get_title_impl(const void* inst) {
    if (!inst) return "";
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->get_title(); } catch (...) { return ""; }
}

static bool is_vsync_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_vsync(); } catch (...) { return false; }
}

static bool is_fullscreen_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_fullscreen(); } catch (...) { return false; }
}

static bool is_borderless_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_borderless(); } catch (...) { return false; }
}

static bool is_resizable_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_resizable(); } catch (...) { return false; }
}

static void minimize_impl(void* inst) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->minimize(); } catch (...) {}
}

static void maximize_impl(void* inst) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->maximize(); } catch (...) {}
}

static void restore_impl(void* inst) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->restore(); } catch (...) {}
}

static void show_impl(void* inst) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->show(); } catch (...) {}
}

static void hide_impl(void* inst) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->hide(); } catch (...) {}
}

static void request_attention_impl(void* inst) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->request_attention(); } catch (...) {}
}

static bool is_minimized_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_minimized(); } catch (...) { return false; }
}

static bool is_maximized_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_maximized(); } catch (...) { return false; }
}

static bool is_visible_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_visible(); } catch (...) { return false; }
}

static bool is_focused_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_focused(); } catch (...) { return false; }
}

static bool is_hovered_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_hovered(); } catch (...) { return false; }
}

static void set_cursor_visible_impl(void* inst, bool visible) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_cursor_visible(visible); } catch (...) {}
}

static void set_cursor_locked_impl(void* inst, bool locked) {
    if (!inst) return;
    try { static_cast<WindowModule*>(inst)->set_cursor_locked(locked); } catch (...) {}
}

static bool is_cursor_visible_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_cursor_visible(); } catch (...) { return false; }
}

static bool is_cursor_locked_impl(const void* inst) {
    if (!inst) return false;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->is_cursor_locked(); } catch (...) { return false; }
}

static void* get_native_handle_impl(const void* inst) {
    if (!inst) return nullptr;
    try { return static_cast<WindowModule*>(const_cast<void*>(inst))->get_native_handle(); } catch (...) { return nullptr; }
}

static spectre_window_api_t g_window_api = {
    .instance = nullptr,
    .should_close = should_close_impl,
    .set_should_close = set_should_close_impl,
    .set_size = set_size_impl,
    .set_position = set_position_impl,
    .set_size_limits = set_size_limits_impl,
    .get_width = get_width_impl,
    .get_height = get_height_impl,
    .get_aspect_ratio = get_aspect_ratio_impl,
    .get_position_x = get_position_x_impl,
    .get_position_y = get_position_y_impl,
    .set_title = set_title_impl,
    .set_vsync = set_vsync_impl,
    .set_fullscreen = set_fullscreen_impl,
    .set_borderless = set_borderless_impl,
    .set_resizable = set_resizable_impl,
    .set_always_on_top = set_always_on_top_impl,
    .get_title = get_title_impl,
    .is_vsync = is_vsync_impl,
    .is_fullscreen = is_fullscreen_impl,
    .is_borderless = is_borderless_impl,
    .is_resizable = is_resizable_impl,
    .minimize = minimize_impl,
    .maximize = maximize_impl,
    .restore = restore_impl,
    .show = show_impl,
    .hide = hide_impl,
    .request_attention = request_attention_impl,
    .is_minimized = is_minimized_impl,
    .is_maximized = is_maximized_impl,
    .is_visible = is_visible_impl,
    .is_focused = is_focused_impl,
    .is_hovered = is_hovered_impl,
    .set_cursor_visible = set_cursor_visible_impl,
    .set_cursor_locked = set_cursor_locked_impl,
    .is_cursor_visible = is_cursor_visible_impl,
    .is_cursor_locked = is_cursor_locked_impl,
    .get_native_handle = get_native_handle_impl
};

SANDBOX_DEFINE_SERVICE(spectre_window_service_t, spectre_window_api_t, &g_window_api)

namespace spectre::module {

    static sandbox_requirement_info_t window_requirements[] = {
        {
            .kind = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name = "configuration",
            .architecture = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name = "logs",
            .architecture = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name = "runtime",
            .architecture = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    // Setup the service structure instance immediately prior to module initialization to be picked up
    static void init_window_service_instance(WindowModule* mod) {
        g_window_api.instance = mod;
    }

    SANDBOX_DECLARE_MODULE(WindowModule, {
        .name = "window",
        .description = "Raylib Window Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_window_service_t_info,
        .requirements = window_requirements,
        .requirement_count = 3
    })

}
