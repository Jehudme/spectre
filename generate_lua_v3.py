import os

def parse_funcs(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    funcs = []
    seen = set()
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
            if name not in seen:
                seen.add(name)
                funcs.append((name, line))
    
    return funcs

funcs = parse_funcs("extracted_apis.txt")

def generate_lua(namespace, funcs, cdef_types):
    cdef = "local ffi = require('ffi')\n\nffi.cdef[[\n"
    for t in cdef_types:
        cdef += "    " + t + "\n"
        
    for name, decl in funcs:
        if decl.endswith(";"):
            cdef += "    " + decl + "\n"
        else:
            cdef += "    " + decl + ";\n"
    cdef += "]]\n\n"

    out = cdef
    out += f"local {namespace} = {{}}\n\n"
    
    modules = {}
    for name, decl in funcs:
        parts = name.split('_')
        if len(parts) >= 3:
            mod = parts[1]
            func = "_".join(parts[2:])
            if mod not in modules:
                modules[mod] = []
            modules[mod].append((func, name, decl))
    
    for mod, fns in modules.items():
        out += f"-- {'='*40}\n"
        out += f"-- {mod.capitalize()} API\n"
        out += f"-- {'='*40}\n"
        out += f"{namespace}.{mod} = {{}}\n\n"
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
            
            # Special case for sandbox logs where we tostring() msg
            if namespace == "sandbox" and mod == "logs" and func in ["trace", "debug", "info", "warn", "error"]:
                out += f"function {namespace}.{mod}.{func}(world, msg)\n"
                out += f"    return ffi.C.{c_name}((world and world.ptr) and world.ptr or world, tostring(msg))\n"
                out += "end\n\n"
            else:
                out += f"function {namespace}.{mod}.{func}({args_str})\n"
                call_args = []
                for a in args:
                    if a == 'world' or a == 'ecs':
                        call_args.append('(world and world.ptr) and world.ptr or world')
                    else:
                        call_args.append(a)
                out += f"    return ffi.C.{c_name}({', '.join(call_args)})\n"
                out += "end\n\n"
        
    return out

sandbox_funcs = [(name, decl) for name, decl in funcs if name.startswith("sandbox_")]
sandbox_types = [
    "typedef struct { uintptr_t token; } sandbox_properties_handle_t;",
    "typedef struct { uintptr_t token; } sandbox_file_handle_t;",
    "typedef enum { SANDBOX_FORMAT_JSON = 0, SANDBOX_FORMAT_BEVE, SANDBOX_FORMAT_TOML, SANDBOX_FORMAT_YAML } sandbox_properties_format_t;"
]

spectre_funcs = [(name, decl) for name, decl in funcs if name.startswith("spectre_")]
spectre_types = [
    "typedef struct { uintptr_t token; } sandbox_properties_handle_t;",
    "typedef struct spectre_serializer_component { void* deserialize; void* serialize; } spectre_serializer_component;",
    "typedef uint64_t ecs_entity_t;",
    "typedef struct ecs_world_t ecs_world_t;",
    "typedef struct ecs_query_t ecs_query_t;",
    "typedef ecs_entity_t (*spectre_component_registration_fn_t)(ecs_world_t* world);",
    "typedef struct spectre_resource_loader_component_t { void* load_fn; void* free_fn; } spectre_resource_loader_component_t;",
    "typedef struct spectre_script_argument_t { int dummy; } spectre_script_argument_t;",
    "typedef enum spectre_script_argument_type_t { DUMMY_ARG = 0 } spectre_script_argument_type_t;",
    "typedef void (*spectre_recursive_callback_t)(ecs_world_t*, ecs_entity_t, void*);"
]

sandbox_out = generate_lua("sandbox", sandbox_funcs, sandbox_types)

# Properties wrapper
sandbox_out += """
-- ========================================
-- Properties Wrapper Class
-- ========================================
sandbox.Properties = {}
sandbox.Properties.__index = sandbox.Properties

function sandbox.Properties.new()
    local instance = { handle = ffi.C.sandbox_properties_create() }
    setmetatable(instance, sandbox.Properties)
    return instance
end

function sandbox.Properties:destroy()
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

function sandbox.Properties:sub(path)
    local sub_handle = ffi.C.sandbox_properties_sub(self.handle, path)
    local instance = { handle = sub_handle }
    setmetatable(instance, sandbox.Properties)
    return instance
end

function sandbox.Properties:get_int64(path)
    local out = ffi.new("int64_t[1]")
    if ffi.C.sandbox_properties_get_int64(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

function sandbox.Properties:get_uint64(path)
    local out = ffi.new("uint64_t[1]")
    if ffi.C.sandbox_properties_get_uint64(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

function sandbox.Properties:get_double(path)
    local out = ffi.new("double[1]")
    if ffi.C.sandbox_properties_get_double(self.handle, path, out) then
        return tonumber(out[0])
    end
    return nil
end

function sandbox.Properties:get_bool(path)
    local out = ffi.new("bool[1]")
    if ffi.C.sandbox_properties_get_bool(self.handle, path, out) then
        return out[0]
    end
    return nil
end

function sandbox.Properties:merge(path, other_props)
    ffi.C.sandbox_properties_merge(self.handle, path, other_props.handle)
end

function sandbox.Properties:get_handle()
    return self.handle
end

return sandbox
"""

with open("tests/application/resources/app/resources/assets/scripts/externals/sandbox.lua", "w") as f:
    f.write(sandbox_out)

spectre_out = generate_lua("spectre", spectre_funcs, spectre_types)
spectre_out += "return spectre\n"

with open("tests/application/resources/app/resources/assets/scripts/externals/spectre.lua", "w") as f:
    f.write(spectre_out)
