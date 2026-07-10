#pragma once

#ifdef SWIG
%include <spectre/components/prefabs_component.h>
%include <spectre/components/serializer_component.h>
%include <spectre/components/resources_component.h>
%include <spectre/components/scenes_component.h>
%include <spectre/components/renderer_component.h>
%include <spectre/components/scripts_component.h>
%include <spectre/components/window_component.h>
%include <spectre/components/core_component.h>

%include <spectre/services/components_service.h>
%include <spectre/services/prefabs_service.h>
%include <spectre/services/renderer_service.h>
%include <spectre/services/resources_service.h>
%include <spectre/services/scenes_service.h>
%include <spectre/services/scripts_service.h>
%include <spectre/services/serializer_service.h>
%include <spectre/services/window_service.h>
#else
#include <spectre/components/serializer_component.h>
#include <spectre/components/resources_component.h>
#include <spectre/components/scenes_component.h>
#include <spectre/components/renderer_component.h>
#include <spectre/components/scripts_component.h>
#include <spectre/components/window_component.h>
#include <spectre/components/core_component.h>
#include <spectre/components/prefabs_component.h>

#include <spectre/services/components_service.h>
#include <spectre/services/prefabs_service.h>
#include <spectre/services/renderer_service.h>
#include <spectre/services/resources_service.h>
#include <spectre/services/scenes_service.h>
#include <spectre/services/scripts_service.h>
#include <spectre/services/serializer_service.h>
#include <spectre/services/window_service.h>
#endif
