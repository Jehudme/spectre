#include "spectre/abi/serializer_service.h"
#include "serializer_module.h"
#include <iostream>


static spectre_serializer_api_t g_serializer_api = {
};

SANDBOX_DEFINE_SERVICE(spectre_serializer_service_t, spectre_serializer_api_t, &g_serializer_api)

