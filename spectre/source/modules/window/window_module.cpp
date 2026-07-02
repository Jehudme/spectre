#include "window_module.h"
#include <spectre/abi/window_service.h>
#include <sandbox/sdk/configuration.hpp>
#include <sandbox/sdk/logs.hpp>
#include <sandbox/sdk/runtime.hpp>
#include <raylib.h>
#include <stdexcept>
#include <string>

// ─────────────────────────────────────────────────────────────────────────────
// Macro helpers
// ─────────────────────────────────────────────────────────────────────────────

/**
 * @brief Wraps a C-ABI call on a mutable WindowModule instance.
 *        Silently returns on null or exception.
 */
#define WINDOW_CALL_MUT(inst, expr) \
    do { \
        if (!(inst)) return; \
        static_cast<WindowModule*>(inst)->expr; \
    } while (0)

/**
 * @brief Wraps a C-ABI call on a const WindowModule instance and returns a value.
 *        Returns `default_val` on null or exception.
 */
#define WINDOW_CALL_CONST(inst, expr, default_val) \
    [&]() { \
        if (!(inst)) return (default_val); \
        return static_cast<WindowModule*>(const_cast<void*>(inst))->expr; \
    }()

namespace spectre::module {

// ─────────────────────────────────────────────────────────────────────────────
// Config helpers  (declared here, implementation at bottom of file)
// ─────────────────────────────────────────────────────────────────────────────

    template<typename T>
    inline T read_config_or_warn(flecs::world& world, const char* key, T default_value, const char* default_label);

// ─────────────────────────────────────────────────────────────────────────────
// MODULE LIFECYCLE
// ─────────────────────────────────────────────────────────────────────────────

    WindowModule::WindowModule(flecs::world& world) : m_entity_world(world) {
        sandbox::modules::logs::trace(m_entity_world, "Window Module: Initializing...");

        const uint32_t width  = read_config_or_warn<uint32_t>(m_entity_world, "window/width",  1280, "1280");
        const uint32_t height = read_config_or_warn<uint32_t>(m_entity_world, "window/height",  720, "720");
        const uint32_t fps    = read_config_or_warn<uint32_t>(m_entity_world, "window/fps",      60, "60");
        const std::string title = read_config_or_warn<std::string>(m_entity_world, "window/title",
                                                                    "Spectre Window", "\"Spectre Window\"");

        sandbox::modules::logs::info(m_entity_world,
            "Window Module: Creating {}x{} window titled '{}'", width, height, title);

        InitWindow(static_cast<int>(width), static_cast<int>(height), title.c_str());
        SetTargetFPS(static_cast<int>(fps));

        if (!IsWindowReady()) {
            sandbox::modules::logs::error(m_entity_world,
                "Window Module: InitWindow() succeeded but IsWindowReady() is false — aborting.");
            throw std::runtime_error("Window Module: Raylib window did not initialize correctly.");
        }

        // Register the per-frame system that detects close requests.
        m_window_system = m_entity_world.system("WindowSystem")
            .kind(flecs::OnUpdate)
            .run([this](flecs::iter& /*iter*/) {
                if (WindowShouldClose()) {
                    sandbox::modules::logs::info(m_entity_world,
                        "Window Module: Close requested — signalling runtime stop.");
                    sandbox::modules::runtime::stop(m_entity_world);
                }
            });

        sandbox::modules::logs::info(m_entity_world, "Window Module: Successfully initialized raylib window.");
    }

    WindowModule::~WindowModule() {
        if (m_window_system.is_valid()) {
            m_window_system.destruct();
        }

        if (IsWindowReady()) {
            sandbox::modules::logs::trace(m_entity_world, "Window Module: Closing raylib window.");
            CloseWindow();
        }
    }

// ─────────────────────────────────────────────────────────────────────────────
// PROPERTY ACCESSORS (thin inline wrappers — declared in header)
// ─────────────────────────────────────────────────────────────────────────────

    bool     WindowModule::should_close()  const { return WindowShouldClose(); }
    void     WindowModule::set_should_close(bool /*should_close*/) { /* unsupported in raylib */ }

    void     WindowModule::set_size(uint32_t width, uint32_t height) {
        SetWindowSize(static_cast<int>(width), static_cast<int>(height));
    }
    void     WindowModule::set_position(int32_t x, int32_t y) { SetWindowPosition(x, y); }
    void     WindowModule::set_size_limits(uint32_t min_width, uint32_t min_height,
                                           uint32_t max_width, uint32_t max_height) {
        SetWindowMinSize(static_cast<int>(min_width),  static_cast<int>(min_height));
        SetWindowMaxSize(static_cast<int>(max_width),  static_cast<int>(max_height));
    }

    uint32_t WindowModule::get_width()        const { return static_cast<uint32_t>(GetScreenWidth()); }
    uint32_t WindowModule::get_height()       const { return static_cast<uint32_t>(GetScreenHeight()); }
    float    WindowModule::get_aspect_ratio() const {
        return static_cast<float>(GetScreenWidth()) / static_cast<float>(GetScreenHeight());
    }
    int32_t  WindowModule::get_position_x()   const { return static_cast<int32_t>(GetWindowPosition().x); }
    int32_t  WindowModule::get_position_y()   const { return static_cast<int32_t>(GetWindowPosition().y); }

    void     WindowModule::set_title(const char* title) { if (title) SetWindowTitle(title); }
    void     WindowModule::set_vsync(bool enabled) {
        if (enabled) SetWindowState(FLAG_VSYNC_HINT); else ClearWindowState(FLAG_VSYNC_HINT);
    }
    void     WindowModule::set_fullscreen(bool fullscreen) {
        if (fullscreen != IsWindowFullscreen()) ToggleFullscreen();
    }
    void     WindowModule::set_borderless(bool borderless) {
        if (borderless) SetWindowState(FLAG_WINDOW_UNDECORATED); else ClearWindowState(FLAG_WINDOW_UNDECORATED);
    }
    void     WindowModule::set_resizable(bool resizable) {
        if (resizable) SetWindowState(FLAG_WINDOW_RESIZABLE); else ClearWindowState(FLAG_WINDOW_RESIZABLE);
    }
    void     WindowModule::set_always_on_top(bool on_top) {
        if (on_top) SetWindowState(FLAG_WINDOW_TOPMOST); else ClearWindowState(FLAG_WINDOW_TOPMOST);
    }

    // Raylib has no get_title() — return a static sentinel so callers know it is unsupported.
    const char* WindowModule::get_title()      const { return ""; }
    bool        WindowModule::is_vsync()       const { return IsWindowState(FLAG_VSYNC_HINT); }
    bool        WindowModule::is_fullscreen()  const { return IsWindowFullscreen(); }
    bool        WindowModule::is_borderless()  const { return IsWindowState(FLAG_WINDOW_UNDECORATED); }
    bool        WindowModule::is_resizable()   const { return IsWindowState(FLAG_WINDOW_RESIZABLE); }

    void        WindowModule::minimize()         { MinimizeWindow(); }
    void        WindowModule::maximize()         { MaximizeWindow(); }
    void        WindowModule::restore()          { RestoreWindow(); }
    void        WindowModule::show()             { ClearWindowState(FLAG_WINDOW_HIDDEN); }
    void        WindowModule::hide()             { SetWindowState(FLAG_WINDOW_HIDDEN); }
    void        WindowModule::request_attention(){ /* not supported by raylib */ }

    bool        WindowModule::is_minimized()   const { return IsWindowMinimized(); }
    bool        WindowModule::is_maximized()   const { return IsWindowMaximized(); }
    bool        WindowModule::is_visible()     const { return !IsWindowHidden(); }
    bool        WindowModule::is_focused()     const { return IsWindowFocused(); }
    bool        WindowModule::is_hovered()     const { return IsWindowFocused(); }

    void        WindowModule::set_cursor_visible(bool visible) {
        if (visible) ShowCursor(); else HideCursor();
    }
    void        WindowModule::set_cursor_locked(bool locked) {
        if (locked) DisableCursor(); else EnableCursor();
    }
    bool        WindowModule::is_cursor_visible() const { return !IsCursorHidden(); }
    bool        WindowModule::is_cursor_locked()  const { return IsCursorHidden(); }
    void*       WindowModule::get_native_handle() const { return GetWindowHandle(); }

// ─────────────────────────────────────────────────────────────────────────────
// HELPERS (implementations)
// ─────────────────────────────────────────────────────────────────────────────

    template<typename T>
    inline T read_config_or_warn(flecs::world& world, const char* key, T default_value, const char* default_label) {
        auto result = sandbox::modules::configuration::get<T>(world, key);
        if (!result.has_value()) {
            sandbox::modules::logs::warn(world,
                "Window Module: '{}' not found in configuration — using default: {}", key, default_label);
        }
        return result.value_or(default_value);
    }

} // namespace spectre::module


// ─────────────────────────────────────────────────────────────────────────────
// C-ABI BRIDGE
// Each function validates the instance pointer and delegates to the C++ module.
// ─────────────────────────────────────────────────────────────────────────────

using spectre::module::WindowModule;

static bool     abi_should_close(const void* inst)               { return WINDOW_CALL_CONST(inst, should_close(), false); }
static void     abi_set_should_close(void* inst, bool value)     { WINDOW_CALL_MUT(inst, set_should_close(value)); }
static void     abi_set_size(void* inst, uint32_t w, uint32_t h) { WINDOW_CALL_MUT(inst, set_size(w, h)); }
static void     abi_set_position(void* inst, int32_t x, int32_t y)       { WINDOW_CALL_MUT(inst, set_position(x, y)); }
static void     abi_set_size_limits(void* inst, uint32_t min_w, uint32_t min_h,
                                    uint32_t max_w, uint32_t max_h)       { WINDOW_CALL_MUT(inst, set_size_limits(min_w, min_h, max_w, max_h)); }
static uint32_t abi_get_width(const void* inst)        { return WINDOW_CALL_CONST(inst, get_width(),        0u); }
static uint32_t abi_get_height(const void* inst)       { return WINDOW_CALL_CONST(inst, get_height(),       0u); }
static float    abi_get_aspect_ratio(const void* inst) { return WINDOW_CALL_CONST(inst, get_aspect_ratio(), 0.0f); }
static int32_t  abi_get_position_x(const void* inst)  { return WINDOW_CALL_CONST(inst, get_position_x(),   0); }
static int32_t  abi_get_position_y(const void* inst)  { return WINDOW_CALL_CONST(inst, get_position_y(),   0); }
static void     abi_set_title(void* inst, const char* title)     { WINDOW_CALL_MUT(inst, set_title(title)); }
static void     abi_set_vsync(void* inst, bool enabled)          { WINDOW_CALL_MUT(inst, set_vsync(enabled)); }
static void     abi_set_fullscreen(void* inst, bool fullscreen)  { WINDOW_CALL_MUT(inst, set_fullscreen(fullscreen)); }
static void     abi_set_borderless(void* inst, bool borderless)  { WINDOW_CALL_MUT(inst, set_borderless(borderless)); }
static void     abi_set_resizable(void* inst, bool resizable)    { WINDOW_CALL_MUT(inst, set_resizable(resizable)); }
static void     abi_set_always_on_top(void* inst, bool on_top)   { WINDOW_CALL_MUT(inst, set_always_on_top(on_top)); }
static const char* abi_get_title(const void* inst)   { return WINDOW_CALL_CONST(inst, get_title(),     ""); }
static bool     abi_is_vsync(const void* inst)       { return WINDOW_CALL_CONST(inst, is_vsync(),       false); }
static bool     abi_is_fullscreen(const void* inst)  { return WINDOW_CALL_CONST(inst, is_fullscreen(),  false); }
static bool     abi_is_borderless(const void* inst)  { return WINDOW_CALL_CONST(inst, is_borderless(),  false); }
static bool     abi_is_resizable(const void* inst)   { return WINDOW_CALL_CONST(inst, is_resizable(),   false); }
static void     abi_minimize(void* inst)          { WINDOW_CALL_MUT(inst, minimize()); }
static void     abi_maximize(void* inst)          { WINDOW_CALL_MUT(inst, maximize()); }
static void     abi_restore(void* inst)           { WINDOW_CALL_MUT(inst, restore()); }
static void     abi_show(void* inst)              { WINDOW_CALL_MUT(inst, show()); }
static void     abi_hide(void* inst)              { WINDOW_CALL_MUT(inst, hide()); }
static void     abi_request_attention(void* inst) { WINDOW_CALL_MUT(inst, request_attention()); }
static bool     abi_is_minimized(const void* inst)   { return WINDOW_CALL_CONST(inst, is_minimized(),  false); }
static bool     abi_is_maximized(const void* inst)   { return WINDOW_CALL_CONST(inst, is_maximized(),  false); }
static bool     abi_is_visible(const void* inst)     { return WINDOW_CALL_CONST(inst, is_visible(),    false); }
static bool     abi_is_focused(const void* inst)     { return WINDOW_CALL_CONST(inst, is_focused(),    false); }
static bool     abi_is_hovered(const void* inst)     { return WINDOW_CALL_CONST(inst, is_hovered(),    false); }
static void     abi_set_cursor_visible(void* inst, bool visible) { WINDOW_CALL_MUT(inst, set_cursor_visible(visible)); }
static void     abi_set_cursor_locked(void* inst, bool locked)   { WINDOW_CALL_MUT(inst, set_cursor_locked(locked)); }
static bool     abi_is_cursor_visible(const void* inst) { return WINDOW_CALL_CONST(inst, is_cursor_visible(), false); }
static bool     abi_is_cursor_locked(const void* inst)  { return WINDOW_CALL_CONST(inst, is_cursor_locked(),  false); }
static void*    abi_get_native_handle(const void* inst) { return WINDOW_CALL_CONST(inst, get_native_handle(), static_cast<void*>(nullptr)); }

static spectre_window_api_t g_window_api = {
    .instance           = nullptr,
    .should_close       = abi_should_close,
    .set_should_close   = abi_set_should_close,
    .set_size           = abi_set_size,
    .set_position       = abi_set_position,
    .set_size_limits    = abi_set_size_limits,
    .get_width          = abi_get_width,
    .get_height         = abi_get_height,
    .get_aspect_ratio   = abi_get_aspect_ratio,
    .get_position_x     = abi_get_position_x,
    .get_position_y     = abi_get_position_y,
    .set_title          = abi_set_title,
    .set_vsync          = abi_set_vsync,
    .set_fullscreen     = abi_set_fullscreen,
    .set_borderless     = abi_set_borderless,
    .set_resizable      = abi_set_resizable,
    .set_always_on_top  = abi_set_always_on_top,
    .get_title          = abi_get_title,
    .is_vsync           = abi_is_vsync,
    .is_fullscreen      = abi_is_fullscreen,
    .is_borderless      = abi_is_borderless,
    .is_resizable       = abi_is_resizable,
    .minimize           = abi_minimize,
    .maximize           = abi_maximize,
    .restore            = abi_restore,
    .show               = abi_show,
    .hide               = abi_hide,
    .request_attention  = abi_request_attention,
    .is_minimized       = abi_is_minimized,
    .is_maximized       = abi_is_maximized,
    .is_visible         = abi_is_visible,
    .is_focused         = abi_is_focused,
    .is_hovered         = abi_is_hovered,
    .set_cursor_visible = abi_set_cursor_visible,
    .set_cursor_locked  = abi_set_cursor_locked,
    .is_cursor_visible  = abi_is_cursor_visible,
    .is_cursor_locked   = abi_is_cursor_locked,
    .get_native_handle  = abi_get_native_handle,
};

SANDBOX_DEFINE_SERVICE(spectre_window_service_t, spectre_window_api_t, &g_window_api)

// ─────────────────────────────────────────────────────────────────────────────
// MODULE REGISTRATION
// ─────────────────────────────────────────────────────────────────────────────

namespace spectre::module {

    static sandbox_requirement_info_t window_requirements[] = {
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "configuration",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "logs",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        },
        {
            .kind          = SANDBOX_REQUIREMENT_KIND_MODULE,
            .strictness    = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
            .name          = "runtime",
            .architecture  = "sandbox",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    static void init_window_service_instance(WindowModule* module_ptr) {
        g_window_api.instance = module_ptr;
    }

    SANDBOX_DECLARE_MODULE(WindowModule, {
        .name          = "window",
        .description   = "Raylib Window Module",
        .architecture  = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service       = &spectre_window_service_t_info,
        .requirements  = window_requirements,
        .requirement_count = 3
    })

} // namespace spectre::module
