%module spectre_api
%include "stdint.i"
%include "carrays.i"
%array_functions(float, floatArray)
typedef uint64_t ecs_entity_t;
%{
#include <flecs.h>
#include <sandbox/sandbox.h>
#include <spectre/spectre.h>
#include <typeinfo>
%}

#define SANDBOX_API
#define SPECTRE_API
#define SANDBOX_DECLARE_SERVICE(...)
#define SANDBOX_DEFINE_SERVICE(...)
#define SANDBOX_DEFINE_HANDLE(name) typedef struct { uintptr_t token; } name;

%include <sandbox/sandbox.h>
%include <spectre/spectre.h>

%{
static ecs_world_t* g_current_world = nullptr;

struct spectre_world {
    ecs_world_t* world;
    spectre_world(ecs_world_t* w) : world(w) {}
};
%}

%inline %{
#ifdef __cplusplus
extern "C" {
#endif
    void set_global_world(ecs_world_t* world) {
        g_current_world = world;
    }
#ifdef __cplusplus
}
#endif

spectre_world get_world() {
    return spectre_world(g_current_world);
}
%}

struct spectre_world {
    spectre_world(ecs_world_t* w);
};

%extend spectre_world {
    float get_mouse_x() {
        return spectre_window_get_mouse_x(self->world);
    }
    float get_mouse_y() {
        return spectre_window_get_mouse_y(self->world);
    }
}

%include "generated_components.i"
