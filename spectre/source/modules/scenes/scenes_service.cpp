#include "spectre/abi/scenes_service.h"
#include "scenes_module.h"
#include <iostream>


static spectre_scenes_api_t g_scenes_api = {
};

SANDBOX_DEFINE_SERVICE(spectre_scenes_service_t, spectre_scenes_api_t, &g_scenes_api)

