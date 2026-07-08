#pragma once

#include <flecs.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "sandbox/abi/properties.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- prefabs ---
typedef struct spectre_prefab_t {
    char dummy;
} spectre_prefab_t;

// --- serializer ---
typedef struct spectre_serializer_t {
    ecs_entity_t (*deserialize)(ecs_world_t* world, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize)(ecs_world_t* world, ecs_entity_t entity);
} spectre_serializer_t;

// --- resources ---
typedef struct spectre_resource_flag            {} spectre_resource_flag;
typedef struct spectre_resource_loader_flag     {} spectre_resource_type;
typedef struct spectre_resource_loader_relation {} spectre_resource_loader_relation;
typedef struct spectre_use_resource_relation    {} spectre_use_resource_relation;

typedef struct spectre_resource_t {
    const char* path;
    void* instance;
} spectre_resource_t;

typedef struct spectre_resource_loader_t {
    void (*load_fn)(ecs_world_t* world, spectre_resource_t* resource);
    void (*free_fn)(ecs_world_t* world, spectre_resource_t* resource);
} spectre_resource_loader_t;

// --- scenes ---
typedef struct spectre_scene_t {
    char dummy;
} spectre_scene_t;

typedef struct spectre_state_t {
    char dummy;
} spectre_state_t;

typedef struct spectre_state_use_scene {
    uint32_t layer_index;
} spectre_state_use_scene_t;

// --- renderer ---
typedef struct spectre_renderable_t {
    char dummy;
} spectre_renderable_flag;

typedef struct specter_disable_rendering_flag {
    char dummy;
} specter_disable_rendering_flag;

// --- scripts ---
typedef struct spectre_script_on_enter_t   { char dummy; } spectre_script_on_enter_t;
typedef struct spectre_script_on_exit_t    { char dummy; } spectre_script_on_exit_t;
typedef struct spectre_script_on_create_t  { char dummy; } spectre_script_on_create_t;
typedef struct spectre_script_on_destroy_t { char dummy; } spectre_script_on_destroy_t;
typedef struct spectre_script_on_update_t  { char dummy; } spectre_script_on_update_t;
typedef struct spectre_script_on_render_t  { char dummy; } spectre_script_on_render_t;

typedef enum spectre_script_argument_type_t {
    SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA
} spectre_script_argument_type_t;

typedef struct spectre_script_argument_t {
    spectre_script_argument_type_t type;
    union {
        bool        boolean_value;
        double      number_value;
        long long   integer_value;
        const char* string_value;
        void*       table_pointer;
        void*       userdata_pointer;
    } value;
} spectre_script_argument_t;

typedef struct spectre_script_t {
    const char* function_name;
} spectre_script_t;

// --- window ---
typedef struct spectre_input_state_t {
    uint64_t keys_down[8];
    uint64_t keys_pressed[8];
    uint64_t keys_released[8];
    float mouse_position[2];
    float mouse_delta[2];
} spectre_input_state_t;

#ifdef __cplusplus
}
#endif