#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {
#endif


typedef struct spectre_scripts_api_t {
} spectre_scripts_api_t;

SANDBOX_DECLARE_SERVICE(spectre_scripts_service_t, spectre_scripts_api_t, {
    .struct_size = 0,
    .name = "scripts",
    .description = "Scripts Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
});

#ifdef __cplusplus
}
#endif
