#include "renderer_module.h"
#include <spectre/abi/renderer_service.h>
#include <sandbox/sdk/logs.hpp>
#include <raylib.h>

namespace spectre::module {
    RendererModule::RendererModule(flecs::world& world) : m_entity_world(world) {
        sandbox::modules::logs::trace(m_entity_world, "Renderer Module: Initializing...");

        m_render_system = m_entity_world.system("RenderSystem")
            .kind(flecs::OnUpdate)
            .run([](flecs::iter& it) {
                BeginDrawing();
                ClearBackground(BLACK);
                EndDrawing();
            });
            
        sandbox::modules::logs::info(m_entity_world, "Renderer Module: Initialized.");
    }

    RendererModule::~RendererModule() {
        if (m_render_system.is_valid()) {
            m_render_system.destruct();
        }
    }
}

// ----------------------------------------------------------------------------
// C-ABI Implementations (Bridge to C++)
// ----------------------------------------------------------------------------
using spectre::module::RendererModule;

static spectre_renderer_api_t g_renderer_api = {
};

SANDBOX_DEFINE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t, &g_renderer_api)

namespace spectre::module {

    static sandbox_requirement_info_t renderer_requirements[] = {
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
            .name = "window",
            .architecture = "spectre",
            .version_major = 1,
            .version_minor = 0,
            .version_patch = -1
        }
    };

    SANDBOX_DECLARE_MODULE(RendererModule, {
        .name = "renderer",
        .description = "Simple Raylib Renderer Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_renderer_service_t_info,
        .requirements = renderer_requirements,
        .requirement_count = 2
    })
}
