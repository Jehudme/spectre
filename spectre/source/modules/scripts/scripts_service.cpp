#include "spectre/abi/scripts_service.h"
#include "scripts_module.h"
#include <iostream>


static spectre_scripts_api_t g_scripts_api = {
};

SANDBOX_DEFINE_SERVICE(spectre_scripts_service_t, spectre_scripts_api_t, &g_scripts_api)

