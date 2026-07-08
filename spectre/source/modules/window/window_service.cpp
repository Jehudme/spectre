#include "spectre/abi/window_service.h"
#include "window_module.h"
#include <iostream>


static spectre_window_api_t g_window_api = {
};

SANDBOX_DEFINE_SERVICE(spectre_window_service_t, spectre_window_api_t, &g_window_api)

