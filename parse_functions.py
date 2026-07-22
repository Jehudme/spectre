import re

def parse_funcs(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    funcs = []
    for line in lines:
        line = line.strip()
        if not line: continue
        # Must contain '(' and ')'
        if '(' not in line or ')' not in line: continue
        # Must not be a typedef or struct or #define
        if line.startswith("typedef") or line.startswith("#define") or line.startswith("struct"): continue
        
        # Try to match: [return_type] [name]([args]);
        # We can just split by '(' to get name
        first_part = line.split('(')[0].strip()
        parts = first_part.split()
        if len(parts) < 2: continue
        name = parts[-1]
        
        # Must start with sandbox_ or spectre_
        if name.startswith("sandbox_") or name.startswith("spectre_"):
            # Ensure it's not a function pointer declaration inside a struct
            if '(*' in line: continue
            
            funcs.append((name, line))
    
    return funcs

funcs = parse_funcs("extracted_apis.txt")

# Generate ffi.cdef
cdef = "ffi.cdef[[\n"
for name, decl in funcs:
    if decl.endswith(";"):
        cdef += decl + "\n"
    else:
        cdef += decl + ";\n"
cdef += "]]\n\n"

# Generate lua wrappers
sandbox_modules = {}
spectre_modules = {}

for name, decl in funcs:
    # e.g., sandbox_logs_info
    if name.startswith("sandbox_"):
        parts = name.split('_')
        if len(parts) >= 3:
            mod = parts[1]
            func = "_".join(parts[2:])
            if mod not in sandbox_modules:
                sandbox_modules[mod] = []
            sandbox_modules[mod].append((func, name, decl))
    elif name.startswith("spectre_"):
        parts = name.split('_')
        if len(parts) >= 3:
            mod = parts[1]
            func = "_".join(parts[2:])
            if mod not in spectre_modules:
                spectre_modules[mod] = []
            spectre_modules[mod].append((func, name, decl))

def build_wrappers(namespace, modules, out_file):
    out = "local ffi = require('ffi')\n\n"
    out += "local {} = {{}}\n\n".format(namespace)
    for mod, fns in modules.items():
        out += "{}.{} = {{}}\n".format(namespace, mod)
        for func, c_name, decl in fns:
            # Extract arguments from decl
            args_part = decl.split('(')[1].split(')')[0]
            args = []
            if args_part.strip() and args_part.strip() != "void":
                arg_decls = args_part.split(',')
                for arg_decl in arg_decls:
                    arg_name = arg_decl.strip().split()[-1]
                    # strip pointer asterisks
                    arg_name = arg_name.replace('*', '')
                    args.append(arg_name)
            
            # Map 'ecs_world_t* ecs' to just 'world' implicitly?
            # User wants: find_component(world, name)
            # So we keep world in the signature.
            args_str = ", ".join(args)
            
            out += "function {}.{}.{}({})\n".format(namespace, mod, func, args_str)
            out += "    return ffi.C.{}({})\n".format(c_name, args_str)
            out += "end\n"
        out += "\n"
    out += "return {}\n".format(namespace)
    
    with open(out_file, "w") as f:
        f.write(out)

build_wrappers("sandbox", sandbox_modules, "sandbox_auto.lua")
build_wrappers("spectre", spectre_modules, "spectre_auto.lua")

print("Generated wrappers.")
