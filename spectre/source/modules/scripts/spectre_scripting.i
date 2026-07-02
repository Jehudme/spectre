%module spectre

%{
#include "flecs.h"
#include <spectre/abi/spectre_window_input_abi.h>
#include <spectre/abi/renderer_components.h>
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
#define SANDBOX_DECLARE_SERVICE(...)
%ignore ecs_id;

// Use SWIG's robust ignore rules if flecs internal macros conflict, but we try parsing directly first.
%include "flecs.h"
%include "spectre/abi/spectre_window_input_abi.h"
%include "spectre/abi/renderer_components.h"
