import re
import os

def parse_funcs(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    funcs = []
    for line in lines:
        line = line.strip()
        if not line: continue
        if '(' not in line or ')' not in line: continue
        if line.startswith("typedef") or line.startswith("#define") or line.startswith("struct"): continue
        if '(*' in line: continue
        
        first_part = line.split('(')[0].strip()
        parts = first_part.split()
        if len(parts) < 2: continue
        name = parts[-1]
        
        if name.startswith("sandbox_") or name.startswith("spectre_"):
            funcs.append((name, line))
    
    return funcs

funcs = parse_funcs("extracted_apis.txt")

cdef = "local ffi = require('ffi')\n\nffi.cdef[[\n"
cdef += "    typedef struct { uintptr_t token; } sandbox_properties_handle_t;\n"
cdef += "    typedef struct { uintptr_t token; } sandbox_file_handle_t;\n"
cdef += "    typedef enum { SANDBOX_FORMAT_JSON = 0, SANDBOX_FORMAT_BEVE, SANDBOX_FORMAT_TOML, SANDBOX_FORMAT_YAML } sandbox_properties_format_t;\n"
cdef += "    typedef struct spectre_serializer_component { void* deserialize; void* serialize; } spectre_serializer_component;\n"
cdef += "    typedef ecs_entity_t (*spectre_component_registration_fn_t)(ecs_world_t* world);\n"
cdef += "    typedef struct spectre_resource_loader_component_t { void* load_fn; void* free_fn; } spectre_resource_loader_component_t;\n"
cdef += "    typedef struct spectre_script_argument_t { int dummy; } spectre_script_argument_t;\n"
cdef += "    typedef enum spectre_script_argument_type_t { DUMMY_ARG = 0 } spectre_script_argument_type_t;\n"
cdef += "    typedef void (*spectre_recursive_callback_t)(ecs_world_t*, ecs_entity_t, void*);\n"

for name, decl in funcs:
    if decl.endswith(";"):
        cdef += "    " + decl + "\n"
    else:
        cdef += "    " + decl + ";\n"
cdef += "]]\n\n"

def build_sandbox():
    out = cdef
    out += "local sandbox = {}\n\n"
    
    sandbox_modules = {}
    for name, decl in funcs:
        if name.startswith("sandbox_"):
            parts = name.split('_')
            if len(parts) >= 3:
                mod = parts[1]
                func = "_".join(parts[2:])
                if mod not in sandbox_modules:
                    sandbox_modules[mod] = []
                sandbox_modules[mod].append((func, name, decl))
    
    for mod, fns in sandbox_modules.items():
        out += "sandbox.{} = {{}}\n".format(mod)
        for func, c_name, decl in fns:
            args_part = decl.split('(')[1].split(')')[0]
            args = []
            if args_part.strip() and args_part.strip() != "void":
                arg_decls = args_part.split(',')
                for arg_decl in arg_decls:
                    arg_name = arg_decl.strip().split()[-1]
                    arg_name = arg_name.replace('*', '')
                    if arg_name == 'ecs': arg_name = 'world'
                    if arg_name == 'format': arg_name = 'fmt'
                    args.append(arg_name)
            args_str = ", ".join(args)
            
            # Special case for sandbox logs where we tostring() msg
            if mod == "logs" and func in ["trace", "debug", "info", "warn", "error"]:
                out += "function sandbox.{}.{}(world, msg)\n".format(mod, func)
                out += "    return ffi.C.{}(world.ptr or world, tostring(msg))\n".format(c_name)
                out += "end\n"
            else:
                out += "function sandbox.{}.{}({})\n".format(mod, func, args_str)
                # Check if world argument is used and extract ptr
                call_args = []
                for a in args:
                    if a == 'world' or a == 'ecs':
                        call_args.append('(world and world.ptr) and world.ptr or world')
                    else:
                        call_args.append(a)
                out += "    return ffi.C.{}({})\n".format(c_name, ", ".join(call_args))
                out += "end\n"
        out += "\n"
        
    out += """
-- Properties Wrapper Class
sandbox.Properties = {}
sandbox.Properties.__index = sandbox.Properties

function sandbox.Properties.new()
    local instance = { handle = ffi.C.sandbox_properties_create() }
    setmetatable(instance, sandbox.Properties)
    return instance
end

function sandbox.Properties:destroy()
    -- Need pointer to handle to destroy it properly
    local ptr = ffi.new("sandbox_properties_handle_t[1]", self.handle)
    ffi.C.sandbox_properties_destroy(ptr)
end

function sandbox.Properties:load(data, fmt)
    return ffi.C.sandbox_properties_load(self.handle, data, #data, fmt or 0)
end

function sandbox.Properties:dump(fmt)
    local str = ffi.C.sandbox_properties_dump(self.handle, fmt or 0)
    if str ~= nil then
        local res = ffi.string(str)
        ffi.C.sandbox_properties_free_string(str)
        return res
    end
    return nil
end

function sandbox.Properties:clear(path)
    ffi.C.sandbox_properties_clear(self.handle, path)
end

function sandbox.Properties:has(path)
    return ffi.C.sandbox_properties_has(self.handle, path)
end

function sandbox.Properties:get_handle()
    return self.handle
end

return sandbox
"""
    with open("tests/application/resources/app/resources/assets/scripts/externals/sandbox.lua", "w") as f:
        f.write(out)

def build_spectre():
    # Only keep the FFI bindings needed for spectre in the cdef so it doesn't double-declare struct if both required
    # Actually LuaJIT allows multiple identical cdefs, but safely we can just omit or pcall it.
    out = "local ffi = require('ffi')\n\n"
    out += "pcall(function()\n"
    out += cdef.replace("local ffi = require('ffi')\n\n", "")
    out += "end)\n\n"
    out += "local spectre = {}\n\n"
    
    spectre_modules = {}
    for name, decl in funcs:
        if name.startswith("spectre_"):
            parts = name.split('_')
            if len(parts) >= 3:
                mod = parts[1]
                func = "_".join(parts[2:])
                if mod not in spectre_modules:
                    spectre_modules[mod] = []
                spectre_modules[mod].append((func, name, decl))
    
    for mod, fns in spectre_modules.items():
        out += "spectre.{} = {{}}\n".format(mod)
        for func, c_name, decl in fns:
            args_part = decl.split('(')[1].split(')')[0]
            args = []
            if args_part.strip() and args_part.strip() != "void":
                arg_decls = args_part.split(',')
                for arg_decl in arg_decls:
                    arg_name = arg_decl.strip().split()[-1]
                    arg_name = arg_name.replace('*', '')
                    if arg_name == 'ecs': arg_name = 'world'
                    if arg_name == 'format': arg_name = 'fmt'
                    if arg_name == 'end': arg_name = 'end_idx'
                    args.append(arg_name)
            args_str = ", ".join(args)
            
            out += "function spectre.{}.{}({})\n".format(mod, func, args_str)
            call_args = []
            for a in args:
                if a == 'world' or a == 'ecs':
                    call_args.append('(world and world.ptr) and world.ptr or world')
                else:
                    call_args.append(a)
            out += "    return ffi.C.{}({})\n".format(c_name, ", ".join(call_args))
            out += "end\n"
        out += "\n"
        
    out += "return spectre\n"
    with open("tests/application/resources/app/resources/assets/scripts/externals/spectre.lua", "w") as f:
        f.write(out)

build_sandbox()
build_spectre()
print("Wrote sandbox.lua and spectre.lua")
