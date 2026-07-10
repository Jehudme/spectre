#include "window_module.h"
#include "spectre/services/window_service.h"
#include "sandbox/sdk/logs.hpp"
#include "sandbox/sdk/runtime.hpp"
#include "spectre/components.h"
#include <raylib.h>
#include <iostream>
#include <cstring>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(window_module_t, {
        .name = "window",
        .description = "Window Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_window_service_t_info,
        .requirements = nullptr,
        .requirement_count = 0
    })

    static ecs_entity_t deserialize_window_cb(ecs_world_t* world, sandbox_properties_handle_t props_handle) {
        flecs::world w(world);
        auto* mod = const_cast<window_module_t*>(w.try_get_mut<window_module_t>());
        if (mod) {
            sandbox::properties props(props_handle, false);
            return mod->deserialize_window(props).id();
        }
        return 0;
    }

    static sandbox_properties_handle_t serialize_window_cb(ecs_world_t* world, ecs_entity_t entity) {
        flecs::world w(world);
        auto* mod = const_cast<window_module_t*>(w.try_get_mut<window_module_t>());
        if (mod) {
            sandbox::properties props = mod->serialize_window(flecs::entity(w, entity));
            sandbox_properties_handle_t handle = props.get_raw();
            props.release();
            return handle;
        }
        return {0};
    }

    struct window_update_marker_t { char dummy; };
    window_module_t::window_module_t(flecs::world& world) : m_world(world) {
        spectre_serializer_component ser = {};
        ser.deserialize = deserialize_window_cb;
        ser.serialize = serialize_window_cb;
        m_world.entity("::spectre::serializers::window")
            .set<spectre_serializer_component>(ser);

        m_world.entity("window_input_dummy").add<window_update_marker_t>();

        m_world.system<window_update_marker_t>("Window Input Event")
            .kind(flecs::PreUpdate)
            .each([this](flecs::entity, window_update_marker_t&) {
                if (should_close()) {
                    sandbox::modules::runtime::stop(m_world);
                }
                begin_input_frame();
            });
    }
    
    window_module_t::~window_module_t() {
        if (IsWindowReady()) {
            CloseWindow();
        }
    }

    flecs::entity window_module_t::deserialize_window(const sandbox::properties& props) {
        if (!props.is_valid()) return flecs::entity::null();

        flecs::entity window_ent = m_world.entity("::window::main");

        spectre_window_component_t comp = {};
        comp.width = props.get<uint32_t>("width").value_or(800);
        comp.height = props.get<uint32_t>("height").value_or(600);
        
        std::string title = props.get<std::string>("title").value_or("Spectre Engine");
        char* title_copy = new char[title.size() + 1];
        std::copy(title.begin(), title.end(), title_copy);
        title_copy[title.size()] = '\0';
        comp.title = title_copy;

        comp.vsync = props.get<bool>("vsync").value_or(false);
        comp.fullscreen = props.get<bool>("fullscreen").value_or(false);
        comp.borderless = props.get<bool>("borderless").value_or(false);
        comp.resizable = props.get<bool>("resizable").value_or(true);
        comp.always_on_top = props.get<bool>("always_on_top").value_or(false);
        comp.min_width = props.get<uint32_t>("min_width").value_or(0);
        comp.min_height = props.get<uint32_t>("min_height").value_or(0);
        comp.max_width = props.get<uint32_t>("max_width").value_or(0);
        comp.max_height = props.get<uint32_t>("max_height").value_or(0);
        comp.position_x = props.get<int32_t>("position_x").value_or(-1);
        comp.position_y = props.get<int32_t>("position_y").value_or(-1);
        
        comp.minimized = props.get<bool>("minimized").value_or(false);
        comp.maximized = props.get<bool>("maximized").value_or(false);
        comp.visible = props.get<bool>("visible").value_or(true);
        comp.focused = props.get<bool>("focused").value_or(true);
        comp.hovered = props.get<bool>("hovered").value_or(true);
        comp.cursor_visible = props.get<bool>("cursor_visible").value_or(true);
        comp.cursor_locked = props.get<bool>("cursor_locked").value_or(false);
        comp.native_handle = nullptr;

        window_ent.set<spectre_window_component_t>(comp);
        return window_ent;
    }

    sandbox::properties window_module_t::serialize_window(flecs::entity window) {
        sandbox::properties props;
        if (!window.is_valid() || !window.has<spectre_window_component_t>()) return props;
        const auto* comp = window.try_get<spectre_window_component_t>();
        if (!comp) return props;

        props.set("width", comp->width);
        props.set("height", comp->height);
        props.set("title", std::string(comp->title ? comp->title : ""));
        props.set("vsync", comp->vsync);
        props.set("fullscreen", comp->fullscreen);
        props.set("borderless", comp->borderless);
        props.set("resizable", comp->resizable);
        props.set("always_on_top", comp->always_on_top);
        props.set("min_width", comp->min_width);
        props.set("min_height", comp->min_height);
        props.set("max_width", comp->max_width);
        props.set("max_height", comp->max_height);
        props.set("position_x", comp->position_x);
        props.set("position_y", comp->position_y);
        props.set("minimized", comp->minimized);
        props.set("maximized", comp->maximized);
        props.set("visible", comp->visible);
        props.set("focused", comp->focused);
        props.set("hovered", comp->hovered);
        props.set("cursor_visible", comp->cursor_visible);
        props.set("cursor_locked", comp->cursor_locked);

        return props;
    }

    void window_module_t::register_window(const sandbox::properties& props) {
        m_window_entity = deserialize_window(props);

        if (m_window_entity.is_valid()) {
            const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
            if (comp) {
                unsigned int flags = 0;
                if (comp->vsync) flags |= FLAG_VSYNC_HINT;
                if (comp->fullscreen) flags |= FLAG_FULLSCREEN_MODE;
                if (comp->borderless) flags |= FLAG_WINDOW_UNDECORATED;
                if (comp->resizable) flags |= FLAG_WINDOW_RESIZABLE;
                if (comp->always_on_top) flags |= FLAG_WINDOW_TOPMOST;
                if (comp->minimized) flags |= FLAG_WINDOW_MINIMIZED;
                if (comp->maximized) flags |= FLAG_WINDOW_MAXIMIZED;
                if (!comp->visible) flags |= FLAG_WINDOW_HIDDEN;
                
                SetConfigFlags(flags);

                InitWindow(comp->width, comp->height, comp->title ? comp->title : "Spectre Engine");
                
                if (comp->min_width > 0 && comp->min_height > 0) {
                    SetWindowMinSize(comp->min_width, comp->min_height);
                }
                if (comp->max_width > 0 && comp->max_height > 0) {
                    SetWindowMaxSize(comp->max_width, comp->max_height);
                }
                if (comp->position_x >= 0 && comp->position_y >= 0) {
                    SetWindowPosition(comp->position_x, comp->position_y);
                }

                spectre_window_component_t updated_comp = *comp;
                updated_comp.native_handle = GetWindowHandle();
                m_window_entity.set<spectre_window_component_t>(updated_comp);
            }
        }
    }

    bool window_module_t::should_close() const {
        return WindowShouldClose();
    }

    void window_module_t::set_should_close(bool close) {
        sandbox::modules::logs::error(m_world, "set_should_close is not supported natively in Raylib.");
    }

    void window_module_t::set_size(uint32_t width, uint32_t height) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->width = width;
            comp->height = height;
            if (IsWindowReady()) SetWindowSize(width, height);
        }
    }

    void window_module_t::set_position(int32_t x, int32_t y) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->position_x = x;
            comp->position_y = y;
            if (IsWindowReady()) SetWindowPosition(x, y);
        }
    }

    void window_module_t::set_size_limits(uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->min_width = min_width;
            comp->min_height = min_height;
            comp->max_width = max_width;
            comp->max_height = max_height;
            if (IsWindowReady()) {
                SetWindowMinSize(min_width, min_height);
                SetWindowMaxSize(max_width, max_height);
            }
        }
    }

    uint32_t window_module_t::get_width() const {
        if (!m_window_entity.is_valid()) return 0;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->width : 0;
    }

    uint32_t window_module_t::get_height() const {
        if (!m_window_entity.is_valid()) return 0;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->height : 0;
    }

    float window_module_t::get_aspect_ratio() const {
        if (!m_window_entity.is_valid()) return 0;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        if (comp && comp->height != 0) {
            return static_cast<float>(comp->width) / static_cast<float>(comp->height);
        }
        return 0.0f;
    }

    int32_t window_module_t::get_position_x() const {
        if (!m_window_entity.is_valid()) return 0;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->position_x : 0;
    }

    int32_t window_module_t::get_position_y() const {
        if (!m_window_entity.is_valid()) return 0;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->position_y : 0;
    }

    void window_module_t::set_title(const char* title) {
        if (!m_window_entity.is_valid() || !title) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            char* title_copy = new char[strlen(title) + 1];
            strcpy(title_copy, title);
            comp->title = title_copy;
            if (IsWindowReady()) SetWindowTitle(title);
        }
    }

    void window_module_t::set_vsync(bool enabled) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->vsync = enabled;
            if (IsWindowReady()) {
                if (enabled) SetWindowState(FLAG_VSYNC_HINT);
                else ClearWindowState(FLAG_VSYNC_HINT);
            }
        }
    }

    void window_module_t::set_fullscreen(bool fullscreen) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->fullscreen = fullscreen;
            if (IsWindowReady()) {
                if (fullscreen) SetWindowState(FLAG_FULLSCREEN_MODE);
                else ClearWindowState(FLAG_FULLSCREEN_MODE);
            }
        }
    }

    void window_module_t::set_borderless(bool borderless) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->borderless = borderless;
            if (IsWindowReady()) {
                if (borderless) SetWindowState(FLAG_WINDOW_UNDECORATED);
                else ClearWindowState(FLAG_WINDOW_UNDECORATED);
            }
        }
    }

    void window_module_t::set_resizable(bool resizable) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->resizable = resizable;
            if (IsWindowReady()) {
                if (resizable) SetWindowState(FLAG_WINDOW_RESIZABLE);
                else ClearWindowState(FLAG_WINDOW_RESIZABLE);
            }
        }
    }

    void window_module_t::set_always_on_top(bool always_on_top) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->always_on_top = always_on_top;
            if (IsWindowReady()) {
                if (always_on_top) SetWindowState(FLAG_WINDOW_TOPMOST);
                else ClearWindowState(FLAG_WINDOW_TOPMOST);
            }
        }
    }

    const char* window_module_t::get_title() const {
        if (!m_window_entity.is_valid()) return nullptr;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->title : nullptr;
    }

    bool window_module_t::is_vsync() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->vsync : false;
    }

    bool window_module_t::is_fullscreen() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->fullscreen : false;
    }

    bool window_module_t::is_borderless() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->borderless : false;
    }

    bool window_module_t::is_resizable() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->resizable : false;
    }

    void window_module_t::minimize() {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->minimized = true;
            if (IsWindowReady()) MinimizeWindow();
        }
    }

    void window_module_t::maximize() {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->maximized = true;
            if (IsWindowReady()) MaximizeWindow();
        }
    }

    void window_module_t::restore() {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->minimized = false;
            comp->maximized = false;
            if (IsWindowReady()) RestoreWindow();
        }
    }

    void window_module_t::show() {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->visible = true;
            if (IsWindowReady()) ClearWindowState(FLAG_WINDOW_HIDDEN);
        }
    }

    void window_module_t::hide() {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->visible = false;
            if (IsWindowReady()) SetWindowState(FLAG_WINDOW_HIDDEN);
        }
    }

    void window_module_t::request_attention() {
        sandbox::modules::logs::error(m_world, "request_attention is not supported natively in Raylib.");
    }

    bool window_module_t::is_minimized() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->minimized : false;
    }

    bool window_module_t::is_maximized() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->maximized : false;
    }

    bool window_module_t::is_visible() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->visible : false;
    }

    bool window_module_t::is_focused() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->focused : false;
    }

    bool window_module_t::is_hovered() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->hovered : false;
    }

    void window_module_t::set_cursor_visible(bool visible) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->cursor_visible = visible;
            if (IsWindowReady()) {
                if (visible) ShowCursor(); else HideCursor();
            }
        }
    }

    void window_module_t::set_cursor_locked(bool locked) {
        if (!m_window_entity.is_valid()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (comp) {
            comp->cursor_locked = locked;
            if (IsWindowReady()) {
                if (locked) DisableCursor(); else EnableCursor();
            }
        }
    }

    bool window_module_t::is_cursor_visible() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->cursor_visible : false;
    }

    bool window_module_t::is_cursor_locked() const {
        if (!m_window_entity.is_valid()) return false;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->cursor_locked : false;
    }

    void* window_module_t::get_native_handle() const {
        if (!m_window_entity.is_valid()) return nullptr;
        const auto* comp = m_window_entity.try_get<spectre_window_component_t>();
        return comp ? comp->native_handle : nullptr;
    }

    bool window_module_t::is_key_down(int keycode) const {
        if (IsWindowReady()) return IsKeyDown(keycode);
        return false;
    }

    bool window_module_t::is_key_pressed(int keycode) const {
        if (IsWindowReady()) return IsKeyPressed(keycode);
        return false;
    }

    bool window_module_t::is_key_released(int keycode) const {
        if (IsWindowReady()) return IsKeyReleased(keycode);
        return false;
    }

    float window_module_t::get_mouse_x() const {
        if (IsWindowReady()) return GetMouseX();
        return 0.0f;
    }

    float window_module_t::get_mouse_y() const {
        if (IsWindowReady()) return GetMouseY();
        return 0.0f;
    }

    float window_module_t::get_mouse_delta_x() const {
        if (IsWindowReady()) return GetMouseDelta().x;
        return 0.0f;
    }

    float window_module_t::get_mouse_delta_y() const {
        if (IsWindowReady()) return GetMouseDelta().y;
        return 0.0f;
    }

    void window_module_t::apply_window_change() {
        if (!m_window_entity.is_valid() || !IsWindowReady()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (!comp) return;

        // Apply all settings from component to raylib window instance
        SetWindowSize(comp->width, comp->height);
        SetWindowPosition(comp->position_x, comp->position_y);
        SetWindowMinSize(comp->min_width, comp->min_height);
        SetWindowMaxSize(comp->max_width, comp->max_height);
        if (comp->title) SetWindowTitle(comp->title);

        if (comp->vsync) SetWindowState(FLAG_VSYNC_HINT); else ClearWindowState(FLAG_VSYNC_HINT);
        if (comp->fullscreen) SetWindowState(FLAG_FULLSCREEN_MODE); else ClearWindowState(FLAG_FULLSCREEN_MODE);
        if (comp->borderless) SetWindowState(FLAG_WINDOW_UNDECORATED); else ClearWindowState(FLAG_WINDOW_UNDECORATED);
        if (comp->resizable) SetWindowState(FLAG_WINDOW_RESIZABLE); else ClearWindowState(FLAG_WINDOW_RESIZABLE);
        if (comp->always_on_top) SetWindowState(FLAG_WINDOW_TOPMOST); else ClearWindowState(FLAG_WINDOW_TOPMOST);
        if (comp->minimized) MinimizeWindow(); else if (comp->maximized) MaximizeWindow(); else RestoreWindow();
        if (comp->visible) ClearWindowState(FLAG_WINDOW_HIDDEN); else SetWindowState(FLAG_WINDOW_HIDDEN);
        
        if (comp->cursor_visible) ShowCursor(); else HideCursor();
        if (comp->cursor_locked) DisableCursor(); else EnableCursor();
    }

    void window_module_t::begin_input_frame() {
        if (!m_window_entity.is_valid() || !IsWindowReady()) return;
        auto* comp = m_window_entity.try_get_mut<spectre_window_component_t>();
        if (!comp) return;
        
        // Update states that can be changed externally by the user or OS
        comp->minimized = IsWindowState(FLAG_WINDOW_MINIMIZED);
        comp->maximized = IsWindowState(FLAG_WINDOW_MAXIMIZED);
        comp->visible = !IsWindowState(FLAG_WINDOW_HIDDEN);
        comp->focused = IsWindowFocused();
        // comp->hovered is not natively updated by Raylib here
        comp->width = GetScreenWidth();
        comp->height = GetScreenHeight();
        comp->position_x = GetWindowPosition().x;
        comp->position_y = GetWindowPosition().y;
    }

    void window_module_t::set_key_down(int keycode) {
        sandbox::modules::logs::error(m_world, "set_key_down is not supported. Input is managed by Raylib.");
    }

    void window_module_t::set_key_up(int keycode) {
        sandbox::modules::logs::error(m_world, "set_key_up is not supported. Input is managed by Raylib.");
    }

    void window_module_t::set_mouse_position(float x, float y) {
        if (IsWindowReady()) SetMousePosition(static_cast<int>(x), static_cast<int>(y));
    }

    void window_module_t::set_mouse_delta(float dx, float dy) {
        sandbox::modules::logs::error(m_world, "set_mouse_delta is not supported. Input is managed by Raylib.");
    }

}
