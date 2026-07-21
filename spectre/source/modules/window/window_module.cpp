#include "window_module.h"
#include "spectre/sdk/serializer.hpp"
#include "spectre/services/window_service.h"

#include "sandbox/sdk/logs.hpp"
#include "sandbox/sdk/runtime.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <raylib.h>

namespace spectre::modules {

static void deserialize_window_cb(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t properties_handle);
static sandbox_properties_handle_t serialize_window_cb(ecs_world_t* world, ecs_entity_t entity_id);

// Component Registration Callbacks
static ecs_entity_t register_window_comp(ecs_world_t* world) {
    return flecs::world(world).component<spectre_window_component_t>().id();
}
static ecs_entity_t register_input_state_comp(ecs_world_t* world) {
    return flecs::world(world).component<spectre_input_state_t>().id();
}

struct spectre_on_event_phase_t {};

static sandbox_requirement_info_t window_requirements[] = {{.kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
                                                            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
                                                            .name = "logs",
                                                            .architecture = "sandbox",
                                                            .version_major = 1,
                                                            .version_minor = 0,
                                                            .version_patch = -1},
                                                           {.kind = SANDBOX_REQUIREMENT_KIND_SERVICE,
                                                            .strictness = SANDBOX_REQUIREMENT_STRICTNESS_REQUIRED,
                                                            .name = "runtime",
                                                            .architecture = "sandbox",
                                                            .version_major = 1,
                                                            .version_minor = 0,
                                                            .version_patch = -1}};

SANDBOX_DECLARE_MODULE(window_module_t, {.name = "window",
                                         .description = "Window Module",
                                         .architecture = "spectre",
                                         .version_major = 1,
                                         .version_minor = 0,
                                         .version_patch = 0,
                                         .service = &spectre_window_service_t_info,
                                         .requirements = window_requirements,
                                         .requirement_count = 2})

window_module_t::window_module_t(flecs::world& world) : m_world(world) {
    sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Window Module] Initializing...");

    // Register components using the SDK
    auto deserialize_empty = [](ecs_world_t*, ecs_entity_t, sandbox_properties_handle_t) {};
    auto serialize_empty = [](ecs_world_t*, ecs_entity_t) -> sandbox_properties_handle_t { return {0}; };
    spectre_serializer_component empty_serializer = {deserialize_empty, serialize_empty};

    register_window_comp(m_world.c_ptr());
    spectre::modules::serializer::register_serializer(m_world, "spectre_window_component_t", &empty_serializer);

    register_input_state_comp(m_world.c_ptr());
    spectre::modules::serializer::register_serializer(m_world, "spectre_input_state_t", &empty_serializer);

    spectre_serializer_component window_serializer = {};
    window_serializer.deserialize = deserialize_window_cb;
    window_serializer.serialize = serialize_window_cb;
    spectre::modules::serializer::register_serializer(m_world, "window", &window_serializer);

    // Create the on_event phase that runs before update
    m_world.entity<spectre_on_event_phase_t>("spectre_on_event_phase").add(flecs::Phase).depends_on(flecs::PreUpdate);

    m_world.entity(flecs::OnUpdate).depends_on<spectre_on_event_phase_t>();

    sandbox::modules::logs::info(const_cast<flecs::world&>(m_world), "[Window Module] Initialized successfully.");
}

window_module_t::~window_module_t() {
    if (IsWindowReady()) {
        CloseWindow();
    }
}

static void deserialize_window_cb(ecs_world_t* world, ecs_entity_t entity, sandbox_properties_handle_t properties_handle) {
    if (!world)
        return;
    flecs::world flecs_world(world);
    auto* module_instance = flecs_world.try_get_mut<window_module_t>();
    if (module_instance) {
        sandbox::properties parsed_properties(properties_handle, false);
        module_instance->deserialize_window(flecs_world.entity(entity), parsed_properties);
    }
}

static sandbox_properties_handle_t serialize_window_cb(ecs_world_t* world, ecs_entity_t entity_id) {
    if (!world || !entity_id)
        return {0};
    flecs::world flecs_world(world);
    auto* module_instance = flecs_world.try_get_mut<window_module_t>();
    if (module_instance) {
        sandbox::properties serialized_properties = module_instance->serialize_window(flecs_world.entity(entity_id));
        sandbox_properties_handle_t raw_handle = serialized_properties.get_raw();
        serialized_properties.release();
        return raw_handle;
    }
    return {0};
}

void window_module_t::deserialize_window(flecs::entity window_entity, const sandbox::properties& properties) {
    if (!properties.is_valid() || !window_entity.is_valid())
        return;

    spectre_window_component_t component = {};
    component.width = properties.get<uint32_t>("width").value_or(800);
    component.height = properties.get<uint32_t>("height").value_or(600);

    std::string title = properties.get<std::string>("title").value_or("Spectre Engine");
    char* title_copy = new char[title.size() + 1];
    std::copy(title.begin(), title.end(), title_copy);
    title_copy[title.size()] = '\0';
    component.title = title_copy;

    component.vsync = properties.get<bool>("vsync").value_or(false);
    component.fullscreen = properties.get<bool>("fullscreen").value_or(false);
    component.borderless = properties.get<bool>("borderless").value_or(false);
    component.resizable = properties.get<bool>("resizable").value_or(true);
    component.always_on_top = properties.get<bool>("always_on_top").value_or(false);
    component.min_width = properties.get<uint32_t>("min_width").value_or(0);
    component.min_height = properties.get<uint32_t>("min_height").value_or(0);
    component.max_width = properties.get<uint32_t>("max_width").value_or(0);
    component.max_height = properties.get<uint32_t>("max_height").value_or(0);
    component.position_x = properties.get<int32_t>("position_x").value_or(-1);
    component.position_y = properties.get<int32_t>("position_y").value_or(-1);

    component.minimized = properties.get<bool>("minimized").value_or(false);
    component.maximized = properties.get<bool>("maximized").value_or(false);
    component.visible = properties.get<bool>("visible").value_or(true);
    component.focused = properties.get<bool>("focused").value_or(true);
    component.hovered = properties.get<bool>("hovered").value_or(true);
    component.cursor_visible = properties.get<bool>("cursor_visible").value_or(true);
    component.cursor_locked = properties.get<bool>("cursor_locked").value_or(false);
    component.native_handle = nullptr;

    window_entity.set<spectre_window_component_t>(component);

    // Also add the input state component
    spectre_input_state_t input_state = {};
    window_entity.set<spectre_input_state_t>(input_state);
}

sandbox::properties window_module_t::serialize_window(flecs::entity window_entity) {
    sandbox::properties properties;
    if (!window_entity.is_valid() || !window_entity.has<spectre_window_component_t>())
        return properties;
    const auto* component = window_entity.try_get<spectre_window_component_t>();
    if (!component)
        return properties;

    properties.set("width", component->width);
    properties.set("height", component->height);
    properties.set("title", std::string(component->title ? component->title : ""));
    properties.set("vsync", component->vsync);
    properties.set("fullscreen", component->fullscreen);
    properties.set("borderless", component->borderless);
    properties.set("resizable", component->resizable);
    properties.set("always_on_top", component->always_on_top);
    properties.set("min_width", component->min_width);
    properties.set("min_height", component->min_height);
    properties.set("max_width", component->max_width);
    properties.set("max_height", component->max_height);
    properties.set("position_x", component->position_x);
    properties.set("position_y", component->position_y);
    properties.set("minimized", component->minimized);
    properties.set("maximized", component->maximized);
    properties.set("visible", component->visible);
    properties.set("focused", component->focused);
    properties.set("hovered", component->hovered);
    properties.set("cursor_visible", component->cursor_visible);
    properties.set("cursor_locked", component->cursor_locked);

    return properties;
}

void window_module_t::register_window(const sandbox::properties& properties) {
    m_window_entity = m_world.entity("::window::main");
    deserialize_window(m_window_entity, properties);

    if (m_window_entity.is_valid()) {
        const auto* component = m_window_entity.try_get<spectre_window_component_t>();
        if (component) {
            unsigned int flags = 0;
            if (component->vsync)
                flags |= FLAG_VSYNC_HINT;
            if (component->fullscreen)
                flags |= FLAG_FULLSCREEN_MODE;
            if (component->borderless)
                flags |= FLAG_WINDOW_UNDECORATED;
            if (component->resizable)
                flags |= FLAG_WINDOW_RESIZABLE;
            if (component->always_on_top)
                flags |= FLAG_WINDOW_TOPMOST;
            if (component->minimized)
                flags |= FLAG_WINDOW_MINIMIZED;
            if (component->maximized)
                flags |= FLAG_WINDOW_MAXIMIZED;
            if (!component->visible)
                flags |= FLAG_WINDOW_HIDDEN;

            SetConfigFlags(flags);

            InitWindow(component->width, component->height, component->title ? component->title : "Spectre Engine");

            if (component->min_width > 0 && component->min_height > 0) {
                SetWindowMinSize(component->min_width, component->min_height);
            }
            if (component->max_width > 0 && component->max_height > 0) {
                SetWindowMaxSize(component->max_width, component->max_height);
            }
            if (component->position_x >= 0 && component->position_y >= 0) {
                SetWindowPosition(component->position_x, component->position_y);
            }

            spectre_window_component_t updated_component = *component;
            updated_component.native_handle = GetWindowHandle();
            m_window_entity.set<spectre_window_component_t>(updated_component);

            auto sys = m_world.system<spectre_window_component_t>("Window Input Event")
                           .kind<spectre_on_event_phase_t>()
                           .each([this](flecs::entity, spectre_window_component_t&) {
                               if (should_close()) {
                                   sandbox::modules::logs::info(const_cast<flecs::world&>(m_world),
                                                                "[Window Module] Window requested close. Stopping "
                                                                "runtime.");
                                   sandbox::modules::runtime::stop(m_world);
                               }
                               begin_input_frame();
                           });
            sys.child_of(m_window_entity);

            sandbox::modules::logs::info(const_cast<flecs::world&>(m_world),
                                         "[Window Module] Window registered and created.");
        }
    }
}

bool window_module_t::should_close() const {
    return WindowShouldClose();
}

void window_module_t::set_should_close(bool close) {
    sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world),
                                 "[Window Module] set_should_close is not supported natively in Raylib.");
}

void window_module_t::set_size(uint32_t width, uint32_t height) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->width = width;
        component->height = height;
        if (IsWindowReady())
            SetWindowSize(width, height);
    }
}

void window_module_t::set_position(int32_t x, int32_t y) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->position_x = x;
        component->position_y = y;
        if (IsWindowReady())
            SetWindowPosition(x, y);
    }
}

void window_module_t::set_size_limits(uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->min_width = min_width;
        component->min_height = min_height;
        component->max_width = max_width;
        component->max_height = max_height;
        if (IsWindowReady()) {
            SetWindowMinSize(min_width, min_height);
            SetWindowMaxSize(max_width, max_height);
        }
    }
}

uint32_t window_module_t::get_width() const {
    if (!m_window_entity.is_valid())
        return 0;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->width : 0;
}

uint32_t window_module_t::get_height() const {
    if (!m_window_entity.is_valid())
        return 0;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->height : 0;
}

float window_module_t::get_aspect_ratio() const {
    if (!m_window_entity.is_valid())
        return 0;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    if (component && component->height != 0) {
        return static_cast<float>(component->width) / static_cast<float>(component->height);
    }
    return 0.0f;
}

int32_t window_module_t::get_position_x() const {
    if (!m_window_entity.is_valid())
        return 0;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->position_x : 0;
}

int32_t window_module_t::get_position_y() const {
    if (!m_window_entity.is_valid())
        return 0;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->position_y : 0;
}

void window_module_t::set_title(const char* title) {
    if (!m_window_entity.is_valid() || !title)
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        char* title_copy = new char[strlen(title) + 1];
        strcpy(title_copy, title);
        component->title = title_copy;
        if (IsWindowReady())
            SetWindowTitle(title);
    }
}

void window_module_t::set_vsync(bool enabled) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->vsync = enabled;
        if (IsWindowReady()) {
            if (enabled)
                SetWindowState(FLAG_VSYNC_HINT);
            else
                ClearWindowState(FLAG_VSYNC_HINT);
        }
    }
}

void window_module_t::set_fullscreen(bool fullscreen) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->fullscreen = fullscreen;
        if (IsWindowReady()) {
            if (fullscreen)
                SetWindowState(FLAG_FULLSCREEN_MODE);
            else
                ClearWindowState(FLAG_FULLSCREEN_MODE);
        }
    }
}

void window_module_t::set_borderless(bool borderless) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->borderless = borderless;
        if (IsWindowReady()) {
            if (borderless)
                SetWindowState(FLAG_WINDOW_UNDECORATED);
            else
                ClearWindowState(FLAG_WINDOW_UNDECORATED);
        }
    }
}

void window_module_t::set_resizable(bool resizable) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->resizable = resizable;
        if (IsWindowReady()) {
            if (resizable)
                SetWindowState(FLAG_WINDOW_RESIZABLE);
            else
                ClearWindowState(FLAG_WINDOW_RESIZABLE);
        }
    }
}

void window_module_t::set_always_on_top(bool always_on_top) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->always_on_top = always_on_top;
        if (IsWindowReady()) {
            if (always_on_top)
                SetWindowState(FLAG_WINDOW_TOPMOST);
            else
                ClearWindowState(FLAG_WINDOW_TOPMOST);
        }
    }
}

const char* window_module_t::get_title() const {
    if (!m_window_entity.is_valid())
        return nullptr;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->title : nullptr;
}

bool window_module_t::is_vsync() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->vsync : false;
}

bool window_module_t::is_fullscreen() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->fullscreen : false;
}

bool window_module_t::is_borderless() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->borderless : false;
}

bool window_module_t::is_resizable() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->resizable : false;
}

void window_module_t::minimize() {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->minimized = true;
        if (IsWindowReady())
            MinimizeWindow();
    }
}

void window_module_t::maximize() {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->maximized = true;
        if (IsWindowReady())
            MaximizeWindow();
    }
}

void window_module_t::restore() {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->minimized = false;
        component->maximized = false;
        if (IsWindowReady())
            RestoreWindow();
    }
}

void window_module_t::show() {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->visible = true;
        if (IsWindowReady())
            ClearWindowState(FLAG_WINDOW_HIDDEN);
    }
}

void window_module_t::hide() {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->visible = false;
        if (IsWindowReady())
            SetWindowState(FLAG_WINDOW_HIDDEN);
    }
}

void window_module_t::request_attention() {
    sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world),
                                 "[Window Module] request_attention is not supported natively in Raylib.");
}

bool window_module_t::is_minimized() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->minimized : false;
}

bool window_module_t::is_maximized() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->maximized : false;
}

bool window_module_t::is_visible() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->visible : false;
}

bool window_module_t::is_focused() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->focused : false;
}

bool window_module_t::is_hovered() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->hovered : false;
}

void window_module_t::set_cursor_visible(bool visible) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->cursor_visible = visible;
        if (IsWindowReady()) {
            if (visible)
                ShowCursor();
            else
                HideCursor();
        }
    }
}

void window_module_t::set_cursor_locked(bool locked) {
    if (!m_window_entity.is_valid())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        component->cursor_locked = locked;
        if (IsWindowReady()) {
            if (locked)
                DisableCursor();
            else
                EnableCursor();
        }
    }
}

bool window_module_t::is_cursor_visible() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->cursor_visible : false;
}

bool window_module_t::is_cursor_locked() const {
    if (!m_window_entity.is_valid())
        return false;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->cursor_locked : false;
}

void* window_module_t::get_native_handle() const {
    if (!m_window_entity.is_valid())
        return nullptr;
    const auto* component = m_window_entity.try_get<spectre_window_component_t>();
    return component ? component->native_handle : nullptr;
}

bool window_module_t::is_key_down(int keycode) const {
    if (!m_window_entity.is_valid() || keycode < 0 || keycode >= 512)
        return false;
    const auto* input_state = m_window_entity.try_get<spectre_input_state_t>();
    if (input_state) {
        return (input_state->keys_down[keycode / 64] & (1ULL << (keycode % 64))) != 0;
    }
    return false;
}

bool window_module_t::is_key_pressed(int keycode) const {
    if (!m_window_entity.is_valid() || keycode < 0 || keycode >= 512)
        return false;
    const auto* input_state = m_window_entity.try_get<spectre_input_state_t>();
    if (input_state) {
        return (input_state->keys_pressed[keycode / 64] & (1ULL << (keycode % 64))) != 0;
    }
    return false;
}

bool window_module_t::is_key_released(int keycode) const {
    if (!m_window_entity.is_valid() || keycode < 0 || keycode >= 512)
        return false;
    const auto* input_state = m_window_entity.try_get<spectre_input_state_t>();
    if (input_state) {
        return (input_state->keys_released[keycode / 64] & (1ULL << (keycode % 64))) != 0;
    }
    return false;
}

float window_module_t::get_mouse_x() const {
    if (!m_window_entity.is_valid())
        return 0.0f;
    const auto* input_state = m_window_entity.try_get<spectre_input_state_t>();
    return input_state ? input_state->mouse_position[0] : 0.0f;
}

float window_module_t::get_mouse_y() const {
    if (!m_window_entity.is_valid())
        return 0.0f;
    const auto* input_state = m_window_entity.try_get<spectre_input_state_t>();
    return input_state ? input_state->mouse_position[1] : 0.0f;
}

float window_module_t::get_mouse_delta_x() const {
    if (!m_window_entity.is_valid())
        return 0.0f;
    const auto* input_state = m_window_entity.try_get<spectre_input_state_t>();
    return input_state ? input_state->mouse_delta[0] : 0.0f;
}

float window_module_t::get_mouse_delta_y() const {
    if (!m_window_entity.is_valid())
        return 0.0f;
    const auto* input_state = m_window_entity.try_get<spectre_input_state_t>();
    return input_state ? input_state->mouse_delta[1] : 0.0f;
}

void window_module_t::apply_window_change() {
    if (!m_window_entity.is_valid() || !IsWindowReady())
        return;
    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (!component)
        return;

    // Apply all settings from component to raylib window instance
    SetWindowSize(component->width, component->height);
    SetWindowPosition(component->position_x, component->position_y);
    SetWindowMinSize(component->min_width, component->min_height);
    SetWindowMaxSize(component->max_width, component->max_height);
    if (component->title)
        SetWindowTitle(component->title);

    if (component->vsync)
        SetWindowState(FLAG_VSYNC_HINT);
    else
        ClearWindowState(FLAG_VSYNC_HINT);
    if (component->fullscreen)
        SetWindowState(FLAG_FULLSCREEN_MODE);
    else
        ClearWindowState(FLAG_FULLSCREEN_MODE);
    if (component->borderless)
        SetWindowState(FLAG_WINDOW_UNDECORATED);
    else
        ClearWindowState(FLAG_WINDOW_UNDECORATED);
    if (component->resizable)
        SetWindowState(FLAG_WINDOW_RESIZABLE);
    else
        ClearWindowState(FLAG_WINDOW_RESIZABLE);
    if (component->always_on_top)
        SetWindowState(FLAG_WINDOW_TOPMOST);
    else
        ClearWindowState(FLAG_WINDOW_TOPMOST);
    if (component->minimized)
        MinimizeWindow();
    else if (component->maximized)
        MaximizeWindow();
    else
        RestoreWindow();
    if (component->visible)
        ClearWindowState(FLAG_WINDOW_HIDDEN);
    else
        SetWindowState(FLAG_WINDOW_HIDDEN);

    if (component->cursor_visible)
        ShowCursor();
    else
        HideCursor();
    if (component->cursor_locked)
        DisableCursor();
    else
        EnableCursor();
}

void window_module_t::begin_input_frame() {
    if (!m_window_entity.is_valid() || !IsWindowReady())
        return;

    // Update input state component
    auto* input_state = m_window_entity.try_get_mut<spectre_input_state_t>();
    if (input_state) {
        std::memset(input_state->keys_down, 0, sizeof(input_state->keys_down));
        std::memset(input_state->keys_pressed, 0, sizeof(input_state->keys_pressed));
        std::memset(input_state->keys_released, 0, sizeof(input_state->keys_released));

        for (int keycode = 0; keycode < 512; ++keycode) {
            if (IsKeyDown(keycode)) {
                input_state->keys_down[keycode / 64] |= (1ULL << (keycode % 64));
            }
            if (IsKeyPressed(keycode)) {
                input_state->keys_pressed[keycode / 64] |= (1ULL << (keycode % 64));
            }
            if (IsKeyReleased(keycode)) {
                input_state->keys_released[keycode / 64] |= (1ULL << (keycode % 64));
            }
        }

        input_state->mouse_position[0] = GetMouseX();
        input_state->mouse_position[1] = GetMouseY();
        input_state->mouse_delta[0] = GetMouseDelta().x;
        input_state->mouse_delta[1] = GetMouseDelta().y;
    }

    auto* component = m_window_entity.try_get_mut<spectre_window_component_t>();
    if (component) {
        // Update states that can be changed externally by the user or OS
        component->minimized = IsWindowState(FLAG_WINDOW_MINIMIZED);
        component->maximized = IsWindowState(FLAG_WINDOW_MAXIMIZED);
        component->visible = !IsWindowState(FLAG_WINDOW_HIDDEN);
        component->focused = IsWindowFocused();
        // component->hovered is not natively updated by Raylib here
        component->width = GetScreenWidth();
        component->height = GetScreenHeight();
        component->position_x = GetWindowPosition().x;
        component->position_y = GetWindowPosition().y;
    }
}

void window_module_t::set_key_down(int keycode) {
    sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Window Module] set_key_down is not supported "
                                                                     "natively. Input is managed by Raylib.");
}

void window_module_t::set_key_up(int keycode) {
    sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world), "[Window Module] set_key_up is not supported "
                                                                     "natively. Input is managed by Raylib.");
}

void window_module_t::set_mouse_position(float position_x, float position_y) {
    if (IsWindowReady())
        SetMousePosition(static_cast<int>(position_x), static_cast<int>(position_y));
}

void window_module_t::set_mouse_delta(float delta_x, float delta_y) {
    sandbox::modules::logs::warn(const_cast<flecs::world&>(m_world),
                                 "[Window Module] set_mouse_delta is not supported natively. Input is "
                                 "managed by Raylib.");
}

} // namespace spectre::modules
