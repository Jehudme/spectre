#include <spectre/abi/spectre_window_input_abi.h>
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/sdk/logs.hpp>
#include <flecs.h>
#include <raylib.h>
#include <cstring>

namespace spectre::module {

class WindowModule {
public:
    explicit WindowModule(flecs::world& ecs);
    ~WindowModule();

private:
    flecs::world m_world;
    flecs::system m_input_system;
};

WindowModule::WindowModule(flecs::world& ecs) : m_world(ecs) {
    sandbox::modules::logs::trace(m_world, "WindowModule: Initializing raylib window...");
    
    // Default window initialization
    InitWindow(1280, 720, "Spectre Engine");
    SetTargetFPS(60);

    // Register singleton input state component
    m_world.component<spectre_input_state_t>("spectre_input_state_t");
    m_world.set<spectre_input_state_t>({});

    // Create PreUpdate system for unified poll step
    m_input_system = m_world.system<spectre_input_state_t>()
        .kind(flecs::PreUpdate)
        .each([this](flecs::entity e, spectre_input_state_t& state) {
            // Zero-out transient state frames to prevent sticky keys
            std::memset(state.keys_pressed, 0, sizeof(state.keys_pressed));
            std::memset(state.keys_released, 0, sizeof(state.keys_released));
            std::memset(state.keys_down, 0, sizeof(state.keys_down));

            // Sample Raylib API for up to 512 keys.
            // We divide by 64 to find the 64-bit integer bucket,
            // and modulo 64 to find the bit index.
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

    static bool abi_should_window_close(void) {
        return WindowShouldClose();
    }
}

static spectre_window_input_api_t g_window_input_api = {
    .is_key_down = abi_is_key_down,
    .is_key_pressed = abi_is_key_pressed,
    .is_key_released = abi_is_key_released,
    .get_mouse_position = abi_get_mouse_position,
    .get_mouse_delta = abi_get_mouse_delta,
    .should_window_close = abi_should_window_close
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
