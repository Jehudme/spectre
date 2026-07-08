import os
import re

modules = ['prefabs', 'renderer', 'resources', 'scenes', 'scripts', 'serializer', 'window']

for mod in modules:
    # 1. Update _service.h
    h_path = f"spectre/include/spectre/services/{mod}_service.h"
    with open(h_path, 'r') as f:
        h_content = f.read()

    # Extract function pointers from struct
    struct_pattern = re.compile(rf'typedef struct spectre_{mod}_api_t \{{(.*?)\}} spectre_{mod}_api_t;', re.DOTALL)
    match = struct_pattern.search(h_content)
    if not match:
        print(f"Error parsing {h_path}")
        continue
    
    struct_body = match.group(1)
    
    # Parse each function pointer
    # ecs_entity_t (*deserialize_window)(ecs_world_t* world, sandbox_properties_handle_t props);
    func_pattern = re.compile(r'^\s*(.*?)\s*\(\*(.*?)\)\((.*?)\);', re.MULTILINE)
    
    funcs = []
    for fmatch in func_pattern.finditer(struct_body):
        ret = fmatch.group(1).strip()
        name = fmatch.group(2).strip()
        args = fmatch.group(3).strip()
        funcs.append((ret, name, args))

    # Generate public API declarations
    public_decls = "\n// --- Public C API ---\n"
    for ret, name, args in funcs:
        # replace world with entity_world in args
        args = re.sub(r'\becs_world_t\*\s*\w+', 'ecs_world_t* entity_world', args)
        public_decls += f"SANDBOX_API {ret} spectre_{mod}_{name}({args});\n"
    public_decls += "\n"

    # Insert before typedef struct
    if "// --- Public C API ---" not in h_content:
        h_content = h_content.replace(f'typedef struct spectre_{mod}_api_t {{', public_decls + f'typedef struct spectre_{mod}_api_t {{')
        with open(h_path, 'w') as f:
            f.write(h_content)

    # 2. Update _service.cpp
    cpp_path = f"spectre/source/modules/{mod}/{mod}_service.cpp"
    with open(cpp_path, 'r') as f:
        cpp_content = f.read()

    # Rename variables in static functions using a series of regexes
    # We want to replace `w` with `flecs_world`, `m` with `module`, `world` with `entity_world`
    
    # First, replace `world` with `entity_world` in the arguments and bodies of static functions.
    # To be safe, let's just do targeted replacements.
    
    # Find all static function definitions: static ret name(args) { ... }
    # Since we are renaming the `world` argument to `entity_world`, we can just replace `ecs_world_t* world` with `ecs_world_t* entity_world`.
    cpp_content = cpp_content.replace('ecs_world_t* world', 'ecs_world_t* entity_world')
    cpp_content = cpp_content.replace('if (!world)', 'if (!entity_world)')
    cpp_content = cpp_content.replace('flecs::world w(world);', 'flecs::world flecs_world(entity_world);')
    cpp_content = cpp_content.replace('auto* m = w.try_get_mut', 'auto* module = flecs_world.try_get_mut')
    cpp_content = cpp_content.replace('if (m)', 'if (module)')
    cpp_content = cpp_content.replace('m->', 'module->')
    cpp_content = cpp_content.replace('w.entity', 'flecs_world.entity')
    
    # Generate public API implementations
    public_impls = "\n// --- Public C API Implementations ---\n"
    for ret, name, args in funcs:
        # Get parameter names for forwarding
        args_replaced = re.sub(r'\becs_world_t\*\s*\w+', 'ecs_world_t* entity_world', args)
        param_list = []
        for arg in args_replaced.split(','):
            arg = arg.strip()
            if not arg: continue
            arg_name = arg.split()[-1].replace('*', '').replace('[]', '')
            param_list.append(arg_name)
            
        forward_args = ', '.join(param_list)
        
        default_ret = ""
        if ret != 'void':
            if '*' in ret or 'entity_t' in ret:
                default_ret = "0"
            elif 'handle_t' in ret:
                default_ret = "{0}"
            elif ret == 'bool':
                default_ret = "false"
            elif 'float' in ret or 'int' in ret or 'double' in ret or 'uint' in ret:
                default_ret = "0"
            else:
                default_ret = "{0}"
                
        impl = f"{ret} spectre_{mod}_{name}({args_replaced}) {{\n"
        if ret != 'void':
            impl += f"    if (!entity_world) return {default_ret};\n"
        else:
            impl += f"    if (!entity_world) return;\n"
            
        impl += f"    flecs::world flecs_world(entity_world);\n"
        impl += f"    if (const auto* service = SANDBOX_GET_SERVICE(flecs_world, spectre_{mod}_service_t)) {{\n"
        impl += f"        if (service->api && service->api->{name}) {{\n"
        if ret != 'void':
            impl += f"            return service->api->{name}({forward_args});\n"
        else:
            impl += f"            service->api->{name}({forward_args});\n"
        impl += f"        }}\n"
        impl += f"    }}\n"
        if ret != 'void':
            impl += f"    return {default_ret};\n"
        impl += f"}}\n\n"
        public_impls += impl

    # Append to the end of cpp file if not already there
    if "// --- Public C API Implementations ---" not in cpp_content:
        cpp_content += public_impls
        with open(cpp_path, 'w') as f:
            f.write(cpp_content)

print("Generated Pure C APIs successfully.")
