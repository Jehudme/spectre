#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

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
        bool boolean_value;
        double number_value;
        long long integer_value;
        const char* string_value;
        void* table_pointer;
        void* userdata_pointer;
        const char* entity;
    } value;
} spectre_script_argument_t;

typedef struct spectre_script_t {
    const char* function_name;
    const char** arguments_name;
    const spectre_script_argument_type_t* argument_types;
    uint32_t argument_count;
} spectre_script_t;

#define SPECTRE_SCRIPT_RELATION_FIELDS    \
    spectre_script_argument_t* arguments; \
    int argument_count;

typedef struct spectre_use_script_on_enter_relation_t {
    SPECTRE_SCRIPT_RELATION_FIELDS
} spectre_use_script_on_enter_relation_t;
typedef struct spectre_use_script_on_exit_relation_t {
    SPECTRE_SCRIPT_RELATION_FIELDS
} spectre_use_script_on_exit_relation_t;
typedef struct spectre_use_script_on_create_relation_t {
    SPECTRE_SCRIPT_RELATION_FIELDS
} spectre_use_script_on_create_relation_t;
typedef struct spectre_use_script_on_destroy_relation_t {
    SPECTRE_SCRIPT_RELATION_FIELDS
} spectre_use_script_on_destroy_relation_t;
typedef struct spectre_use_script_on_update_relation_t {
    SPECTRE_SCRIPT_RELATION_FIELDS
} spectre_use_script_on_update_relation_t;
typedef struct spectre_use_script_on_render_relation_t {
    SPECTRE_SCRIPT_RELATION_FIELDS
} spectre_use_script_on_render_relation_t;

#ifdef __cplusplus
}
#endif
