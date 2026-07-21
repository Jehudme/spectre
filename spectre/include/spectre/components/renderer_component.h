#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_color_t {
    float r, g, b, a;
} spectre_color_t;

typedef struct spectre_renderable_t {
    char dummy;
} spectre_renderable_t;

typedef struct spectre_2D_transform_component_t {
    float position_x, position_y, position_z;
    float scale_x, scale_y;
    float origin_x, origin_y;
    float rotation;
} spectre_2D_transform_component_t;

typedef struct spectre_rectange_renderable_t {
    float width;
    float height;
    spectre_color_t fill_color;
    spectre_color_t outline_color;
    float outline_thickness;
} spectre_rectange_renderable_t;

typedef struct spectre_circle_renderable_t {
    float radius;
    spectre_color_t fill_color;
    spectre_color_t outline_color;
    float outline_thickness;
} spectre_circle_renderable_t;

typedef struct spectre_polygone_renderable_t {
    float radius;
    uint32_t point_count;
    spectre_color_t fill_color;
    spectre_color_t outline_color;
    float outline_thickness;
} spectre_polygone_renderable_t;

typedef struct spectre_custom_polygone_renderable_t {
    float* vertices;
    uint32_t vertex_count;
    spectre_color_t fill_color;
    spectre_color_t outline_color;
    float outline_thickness;
} spectre_custom_polygone_renderable_t;

typedef struct spectre_ligne_renderable_t {
    double position_x1, position_y1, position_x2, position_y2;
    spectre_color_t color;
    float thickness;
} spectre_ligne_renderable_t;

#ifdef __cplusplus
}
#endif
