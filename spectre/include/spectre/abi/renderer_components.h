#pragma once

#include <stdint.h>
#include <stdbool.h>

/* * The extern "C" block ensures that if a C++ compiler reads this file,
 * it disables C++ name mangling, allowing dynamic libraries to link safely.
 */
#ifdef __cplusplus
extern "C" {
#endif

// ============================================================================
// 1. CORE MATH & UTILITY TYPES
// ============================================================================

/**
 * @brief Standard RGBA color representation (0.0f to 1.0f).
 */
typedef struct spectre_color_t {
    float r, g, b, a;
} spectre_color_t;

/**
 * @brief 2D Spatial transform.
 * Kept strictly separate from visual data so physics/gameplay logic
 * can move entities without pulling render data into the CPU L1/L2 cache.
 */
typedef struct spectre_transform_2d_t {
    float position[2]; // X, Y (World Space)
    float rotation;    // Rotation in radians
    float scale[2];    // Scale X, Scale Y (Crucial for scaling squares into rectangles)
} spectre_transform_2d_t;


// ============================================================================
// 2. THE RENDER FLAG / TAG
// ============================================================================

typedef enum spectre_render_type_2d_t {
    SPECTRE_RENDER_2D_POLYGON = 0, // Handles Triangles, Squares, Rectangles, Hexagons, and Circles
    SPECTRE_RENDER_2D_LINE
} spectre_render_type_2d_t;

/**
 * @brief The base tag for all 2D renderable entities.
 * The renderer queries this to filter entities and dictate draw order.
 */
typedef struct spectre_renderable_2d_t {
    spectre_render_type_2d_t type;
    bool     is_visible;
    uint32_t z_order; // Used to sort which shape draws on top of another
} spectre_renderable_2d_t;


// ============================================================================
// 3. SPECIFIC SHAPE DATA COMPONENTS
// ============================================================================

/**
 * @brief Represents any closed 2D shape.
 * - 3 points = Triangle
 * - 4 points = Square/Diamond (Use transform.scale for Rectangles)
 * - 64 points = Smooth Circle
 */
typedef struct spectre_polygon_renderer_t {
    float radius;
    uint32_t point_count;

    spectre_color_t fill_color;
    spectre_color_t outline_color;
    float outline_thickness;
} spectre_polygon_renderer_t;

/**
 * @brief Represents a line segment drawn relative to the entity's transform.
 */
typedef struct spectre_line_renderer_t {
    // The line starts at the entity's transform.position.
    // This is the local offset of the end point.
    float end_point_local[2];

    spectre_color_t color;
    float thickness;
} spectre_line_renderer_t;

#ifdef __cplusplus
}
#endif