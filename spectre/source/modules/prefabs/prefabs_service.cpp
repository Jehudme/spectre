#include "spectre/abi/prefabs_service.h"
#include "prefabs_module.h"
#include <iostream>


static spectre_prefabs_api_t g_prefabs_api = {
};

SANDBOX_DEFINE_SERVICE(spectre_prefabs_service_t, spectre_prefabs_api_t, &g_prefabs_api)

