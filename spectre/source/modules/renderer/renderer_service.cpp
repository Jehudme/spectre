#include "spectre/abi/renderer_service.h"
#include "renderer_module.h"
#include <iostream>


static spectre_renderer_api_t g_renderer_api = {
};

SANDBOX_DEFINE_SERVICE(spectre_renderer_service_t, spectre_renderer_api_t, &g_renderer_api)

