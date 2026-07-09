%module spectre_api
%{
#include <flecs.h>
#include <sandbox/sandbox.h>
#include <spectre/spectre.h>
%}

#define SANDBOX_API
#define SPECTRE_API
#define SANDBOX_DECLARE_SERVICE(...)
#define SANDBOX_DEFINE_SERVICE(...)
#define SANDBOX_DEFINE_HANDLE(name) typedef struct { uintptr_t token; } name;

%include <sandbox/sandbox.h>
%include <spectre/spectre.h>
