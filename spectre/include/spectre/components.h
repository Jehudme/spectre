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
typedef struct spectre_prefab_flag_t {
    char dummy;
} spectre_prefab_flag_t;

// --- serializer ---
typedef struct spectre_serializer_component {
    ecs_entity_t (*deserialize)(ecs_world_t* world, sandbox_properties_handle_t props);
    sandbox_properties_handle_t (*serialize)(ecs_world_t* world, ecs_entity_t entity);
} spectre_serializer_component;

typedef struct spectre_serializer_relation_t {
    char dummy;
} spectre_serializer_relation_t;

// --- resources ---
typedef struct spectre_use_loader_relation_t { char dummy; } spectre_resource_loader_relation;
typedef struct spectre_use_resource_relation_t { char dummy; } spectre_use_resource_relation;
typedef struct spectre_resource_loaded_flag_t { char dummy; } spectre_resource_flag_t;

typedef struct spectre_resource_component_t {
    const char* path;
    void* instance;
} spectre_resource_component_t;

typedef struct spectre_resource_loader_component_t {
    void (*load_fn)(ecs_world_t* world, spectre_resource_component_t* resource);
    void (*free_fn)(ecs_world_t* world, spectre_resource_component_t* resource);
} spectre_resource_loader_component_t;

// --- scenes ---
typedef struct spectre_disable_rendering_t {
    char dummy;
} spectre_disable_rendering_t;

typedef struct spectre_state_t { char dummy; } spectre_state_t;
typedef struct spectre_scene_t { char dummy; } spectre_scene_t;
typedef struct spectre_state_use_scene_relation_t { int layer_index; } spectre_state_use_scene_relation_t;

typedef void (*spectre_recursive_callback_t)(ecs_world_t* world, ecs_entity_t entity, void* payload);

struct spectre_scene_context_t {
    flecs::entity scene_entity;
};

struct spectre_state_context_t {
    flecs::entity state_entity;
};

// --- renderer ---
typedef struct spectre_renderable_t {
    char dummy;
} spectre_renderable_t;

typedef struct spectre_2D_transform_component_t {
    float position_x, position_y, position_z;
    float scale_x, scale_y;
    float origin_x, origin_y;
    float rotation;
} spectre_2D_transform_component_t;


// --- scripts ---

typedef enum spectre_script_argument_type_t {
    SPECTRE_SCRIPT_ARGUMENT_TYPE_NIL,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_BOOLEAN,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_NUMBER,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_INTEGER,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_STRING,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_TABLE,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_USERDATA,
    SPECTRE_SCRIPT_ARGUMENT_TYPE_ENTITY
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
        const char* entity;
    } value;
} spectre_script_argument_t;

typedef struct spectre_script_t {
    const char* function_name;
    const char** arguments_name;
    const spectre_script_argument_type_t* argument_types;
    uint32_t argument_count;
} spectre_script_t;

#define SPECTRE_SCRIPT_RELATION_FIELDS spectre_script_argument_t* arguments; int argument_count;

typedef struct spectre_use_script_on_enter_relation_t { SPECTRE_SCRIPT_RELATION_FIELDS } spectre_use_script_on_enter_relation_t;
typedef struct spectre_use_script_on_exit_relation_t { SPECTRE_SCRIPT_RELATION_FIELDS } spectre_use_script_on_exit_relation_t;
typedef struct spectre_use_script_on_create_relation_t { SPECTRE_SCRIPT_RELATION_FIELDS} spectre_use_script_on_create_relation_t;
typedef struct spectre_use_script_on_destroy_relation_t { SPECTRE_SCRIPT_RELATION_FIELDS } spectre_use_script_on_destroy_relation_t;
typedef struct spectre_use_script_on_update_relation_t { SPECTRE_SCRIPT_RELATION_FIELDS } spectre_use_script_on_update_relation_t;
typedef struct spectre_use_script_on_render_relation_t { SPECTRE_SCRIPT_RELATION_FIELDS } spectre_use_script_on_render_relation_t;

// --- window ---
typedef struct spectre_input_state_t {
    uint64_t keys_down[8];
    uint64_t keys_pressed[8];
    uint64_t keys_released[8];
    float mouse_position[2];
    float mouse_delta[2];
} spectre_input_state_t;

// --- Components ---
typedef struct spectre_component_t { char dummy; } spectre_component_serializer_t;

#ifdef __cplusplus
}
#endif