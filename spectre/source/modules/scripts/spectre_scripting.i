%module spectre

%{
#include "flecs.h"
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/engine.h>
#include <sandbox/abi/handle.h>
#include <sandbox/abi/library_loader.h>
#include <sandbox/abi/platform.h>
#include <sandbox/abi/properties.h>

#include <spectre/abi/prefabs_service.h>
#include <spectre/abi/renderer_components.h>
#include <spectre/abi/renderer_service.h>
#include <spectre/abi/resources_service.h>
#include <spectre/abi/scripts_components.h>
#include <spectre/abi/scripts_service.h>
#include <spectre/abi/window_service.h>
%}

// Handle opaque pointers gracefully across the boundary wrapper.
typedef struct ecs_world_t ecs_world_t;
typedef struct ecs_entity_t ecs_entity_t;
typedef struct lua_State lua_State;

%include "stdint.i"

#define FLECS_API
#define ECS_API
#define FLECS_EXPORT
#define FLECS_ALWAYS_INLINE
#define __linux__
#define SANDBOX_API
%ignore SANDBOX_DECLARE_SERVICE;
%ignore ecs_id;

// Use SWIG's robust ignore rules if flecs internal macros conflict, but we try parsing directly first.
%include "flecs.h"
%include "sandbox/abi/bootstrapper.h"
%include "sandbox/abi/engine.h"
%include "sandbox/abi/handle.h"
%include "sandbox/abi/library_loader.h"
%include "sandbox/abi/platform.h"
%include "sandbox/abi/properties.h"

%include "spectre/abi/prefabs_service.h"
%include "spectre/abi/renderer_components.h"
%include "spectre/abi/renderer_service.h"
%include "spectre/abi/resources_service.h"
%include "spectre/abi/scripts_components.h"
%include "spectre/abi/scripts_service.h"
%include "spectre/abi/window_service.h"

%inline %{
void move_entity(ecs_world_t* world, uint64_t entity, float dx, float dy) {
    if (!world || !entity) return;
    ecs_entity_t comp_id = ecs_lookup(world, "Transform2D");
    if (!comp_id) return;
    spectre_transform_2d_t* t = (spectre_transform_2d_t*)ecs_get_mut_id(world, entity, comp_id);
    if (t) {
        t->position[0] += dx;
        t->position[1] += dy;
    }
}
%}
