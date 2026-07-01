#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <sandbox/abi/bootstrapper.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- Resource Handles ---
// Using opaque 64-bit integers to safely pass GPU resource IDs across the ABI.
// The internal C++ renderer maps these IDs to actual OpenGL/Vulkan objects.
typedef uint64_t spectre_mesh_handle_t;
typedef uint64_t spectre_texture_handle_t;
typedef uint64_t spectre_material_handle_t;

#define SPECTRE_INVALID_HANDLE 0

// --- Service API ---

/**
 * @brief Pure C virtual table mapping the Renderer subsystem boundary.
 * Exposes GPU resource uploading, texture streaming, and global pipeline state.
 */
typedef struct spectre_renderer_api_t {
    
} spectre_renderer_api_t;

// --- Service Registration ---

SANDBOX_DECLARE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t, {
    .name = "renderer",
    .description = "The core rendering pipeline and GPU resource manager",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0
})

#ifdef __cplusplus
}
#endif