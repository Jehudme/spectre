#include "spectre/abi/resources_service.h"
#include "resources_module.h"
#include <iostream>


static spectre_resources_api_t g_resources_api = {
};

SANDBOX_DEFINE_SERVICE(spectre_resources_service_t, spectre_resources_api_t, &g_resources_api)

