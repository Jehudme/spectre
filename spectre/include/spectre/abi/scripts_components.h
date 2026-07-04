#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spectre_script_t {
    const char* function_name;
    uint32_t target_entity;
} spectre_script_t;

typedef enum spectre_script_trigger_t {
    SPECTRE_SCRIPT_TRIGGER_ON_ENTER,
    SPECTRE_SCRIPT_TRIGGER_ON_EXIT,
    SPECTRE_SCRIPT_TRIGGER_ON_CREATE,
    SPECTRE_SCRIPT_TRIGGER_ON_DELETE,
    SPECTRE_SCRIPT_TRIGGER_ON_UPDATE,
    SPECTRE_SCRIPT_TRIGGER_UNDEFINED
} spectre_script_trigger_t;

// Empty tag components for triggering scripts
typedef struct spectre_script_on_enter_t { char dummy; } spectre_script_on_enter_t;
typedef struct spectre_script_on_exit_t { char dummy; } spectre_script_on_exit_t;
typedef struct spectre_script_on_create_t { char dummy; } spectre_script_on_create_t;
typedef struct spectre_script_on_delete_t { char dummy; } spectre_script_on_delete_t;
typedef struct spectre_script_on_update_t { char dummy; } spectre_script_on_update_t;

#ifdef __cplusplus
}
#endif