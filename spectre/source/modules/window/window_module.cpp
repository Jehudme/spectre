#include "window_module.h"
#include "spectre/services/window_service.h"
#include <iostream>

namespace spectre::modules {

    SANDBOX_DECLARE_MODULE(window_module_t, {
        .name = "window",
        .description = "Window Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_window_service_t_info_decl,
        .requirements = nullptr,
        .requirement_count = 0
    })

    window_module_t::window_module_t(flecs::world& world) : m_world(world) {
        std::cout << "[SPECTRE] Initializing " << "window" << " module" << std::endl;
    }
    
    window_module_t::~window_module_t() = default;

    flecs::entity window_module_t::deserialize_window(sandbox::properties props) {
        return flecs::entity::null();
    }

    sandbox::properties window_module_t::serialize_window(flecs::entity window) {
        return sandbox::properties{};
    }

    void window_module_t::register_window(sandbox::properties props) {
    }

    bool window_module_t::should_close() const {
        return false;
    }

    void window_module_t::set_should_close(bool close) {
    }

    void window_module_t::set_size(uint32_t width, uint32_t height) {
    }

    void window_module_t::set_position(int32_t x, int32_t y) {
    }

    void window_module_t::set_size_limits(uint32_t min_width, uint32_t min_height, uint32_t max_width, uint32_t max_height) {
    }

    uint32_t window_module_t::get_width() const {
        return 0;
    }

    uint32_t window_module_t::get_height() const {
        return 0;
    }

    float window_module_t::get_aspect_ratio() const {
        return 0;
    }

    int32_t window_module_t::get_position_x() const {
        return 0;
    }

    int32_t window_module_t::get_position_y() const {
        return 0;
    }

    void window_module_t::set_title(const char* title) {
    }

    void window_module_t::set_vsync(bool enabled) {
    }

    void window_module_t::set_fullscreen(bool fullscreen) {
    }

    void window_module_t::set_borderless(bool borderless) {
    }

    void window_module_t::set_resizable(bool resizable) {
    }

    void window_module_t::set_always_on_top(bool always_on_top) {
    }

    const char* window_module_t::get_title() const {
        return nullptr;
    }

    bool window_module_t::is_vsync() const {
        return false;
    }

    bool window_module_t::is_fullscreen() const {
        return false;
    }

    bool window_module_t::is_borderless() const {
        return false;
    }

    bool window_module_t::is_resizable() const {
        return false;
    }

    void window_module_t::minimize() {
    }

    void window_module_t::maximize() {
    }

    void window_module_t::restore() {
    }

    void window_module_t::show() {
    }

    void window_module_t::hide() {
    }

    void window_module_t::request_attention() {
    }

    bool window_module_t::is_minimized() const {
        return false;
    }

    bool window_module_t::is_maximized() const {
        return false;
    }

    bool window_module_t::is_visible() const {
        return false;
    }

    bool window_module_t::is_focused() const {
        return false;
    }

    bool window_module_t::is_hovered() const {
        return false;
    }

    void window_module_t::set_cursor_visible(bool visible) {
    }

    void window_module_t::set_cursor_locked(bool locked) {
    }

    bool window_module_t::is_cursor_visible() const {
        return false;
    }

    bool window_module_t::is_cursor_locked() const {
        return false;
    }

    void* window_module_t::get_native_handle() const {
        return nullptr;
    }

    bool window_module_t::is_key_down(int keycode) const {
        return false;
    }

    bool window_module_t::is_key_pressed(int keycode) const {
        return false;
    }

    bool window_module_t::is_key_released(int keycode) const {
        return false;
    }

    float window_module_t::get_mouse_x() const {
        return 0;
    }

    float window_module_t::get_mouse_y() const {
        return 0;
    }

    float window_module_t::get_mouse_delta_x() const {
        return 0;
    }

    float window_module_t::get_mouse_delta_y() const {
        return 0;
    }

    void window_module_t::set_key_down(int keycode) {
    }

    void window_module_t::set_key_up(int keycode) {
    }

    void window_module_t::set_mouse_position(float x, float y) {
    }

    void window_module_t::set_mouse_delta(float dx, float dy) {
    }

}
