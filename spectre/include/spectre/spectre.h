#pragma once

#ifdef SWIG
%include <spectre/components.h>

%include <spectre/services/components_service.h>
%include <spectre/services/prefabs_service.h>
%include <spectre/services/renderer_service.h>
%include <spectre/services/resources_service.h>
%include <spectre/services/scenes_service.h>
%include <spectre/services/scripts_service.h>
%include <spectre/services/serializer_service.h>
%include <spectre/services/window_service.h>
#else
#include <spectre/components.h>

#include <spectre/services/components_service.h>
#include <spectre/services/prefabs_service.h>
#include <spectre/services/renderer_service.h>
#include <spectre/services/resources_service.h>
#include <spectre/services/scenes_service.h>
#include <spectre/services/scripts_service.h>
#include <spectre/services/serializer_service.h>
#include <spectre/services/window_service.h>
#endif
