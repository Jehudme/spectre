import re
import os

header = open('spectre/include/spectre/spectre.h').read()

modules_info = {
    'scenes': ('spectre_scenes_', 'scenes', 'scenes_module_t'),
    'components': ('spectre_components_', 'prefabs', 'prefab_module_t'),
    'serializer': ('spectre_serializer_', 'serializer', 'serializer_module'),
    'renderer': ('spectre_renderer_', 'renderer', 'renderer_module_t'),
    'resource': ('spectre_resource_', 'resources', 'resource_module_t'),
    'script': ('spectre_script_', 'scripts', 'script_module_t'),
    'window': ('spectre_window_', 'window', 'window_module_t')
}

funcs = []
pattern = re.compile(r'^([a-zA-Z_][a-zA-Z0-9_*\s]*?)\s+([a-zA-Z0-9_]+)\((.*?)\);', re.MULTILINE)
for match in pattern.finditer(header):
    ret, name, args = match.groups()
    if name.startswith('spectre_'):
        funcs.append((ret.strip(), name.strip(), args.strip()))

for mod, (prefix, folder, class_name) in modules_info.items():
    mod_funcs = [f for f in funcs if f[1].startswith(prefix)]
    
    # Check if the class is actually serializer_module_t
    if class_name == 'serializer_module':
        class_name = 'serializer_module' # let's just use it
        
    # Read the header to get the class methods
    header_path = f"spectre/source/modules/{folder}/{folder}_module.h"
    if not os.path.exists(header_path):
        # some folders might be named differently?
        if folder == 'prefabs':
            header_path = "spectre/source/modules/prefabs/prefabs_module.h"
            if not os.path.exists(header_path):
                continue
    
    header_content = ""
    try:
        header_content = open(header_path).read()
    except Exception:
        pass
        
    cpp_path = f"spectre/source/modules/{folder}/{folder}_module.cpp"
    if not os.path.exists(cpp_path):
        continue

    # We will generate the cpp file completely.
    # First, parse the methods from the header so we can generate stubs for them!
    methods = []
    # simple heuristic: look for return_type method_name(args) inside the class
    class_pattern = re.compile(r'class\s+([a-zA-Z0-9_]+)[\s\S]*?\{([\s\S]*?)\};')
    class_match = class_pattern.search(header_content)
    if class_match:
        class_name = class_match.group(1) # override with actual class name!
        class_body = class_match.group(2)
        # find methods
        method_pattern = re.compile(r'^\s*([a-zA-Z_][a-zA-Z0-9_*:&<>\s]*?)\s+([a-zA-Z0-9_]+)\((.*?)\)\s*(const)?\s*;', re.MULTILINE)
        for mmatch in method_pattern.finditer(class_body):
            mret, mname, margs, mconst = mmatch.groups()
            if mname == class_name or mname.startswith('operator') or mname.startswith('~'):
                continue
            methods.append((mret.strip(), mname.strip(), margs.strip(), mconst))

    cpp_content = f"""#include "{os.path.basename(header_path)}"
#include "spectre/services.h"
#include <iostream>

namespace spectre::modules {{

    {class_name}::{class_name}(flecs::world& world) : m_world(world) {{
        world.module<{class_name}>("spectre::modules::{folder}_module");
        
        {prefix}service_t svc = {{}};
"""

    for ret, name, args in mod_funcs:
        short_name = name[len(prefix):]
        
        # generate lambda
        arg_names = []
        for arg in args.split(','):
            arg = arg.strip()
            if not arg: continue
            # extract name (last word after removing pointer asterisks)
            arg = re.sub(r'\[.*?\]', '', arg) # remove arrays
            parts = arg.split()
            name_part = parts[-1].replace('*', '')
            arg_names.append(name_part)
            
        forward_args = []
        for arg, t in zip(arg_names[1:], args.split(',')[1:]): # skip ecs_world_t* world
            # if the C api takes sandbox_properties_handle_t, the C++ api takes sandbox::properties
            if 'sandbox_properties_handle_t' in t:
                forward_args.append(f"sandbox::properties({arg})")
            elif 'ecs_entity_t' in t:
                forward_args.append(f"w.entity({arg})")
            else:
                forward_args.append(arg)
                
        forward_list = ', '.join(forward_args)
        
        cpp_content += f"        svc.{short_name} = []({args}) -> {ret} {{\n"
        cpp_content += f"            flecs::world w(world);\n"
        cpp_content += f"            ecs_entity_t id = w.component<{class_name}>().id();\n"
        cpp_content += f"            {class_name}* module = ({class_name}*)ecs_get_mut_id(world, id, id);\n"
        
        call_stmt = f"module->{short_name}({forward_list})"
        if 'sandbox_properties_handle_t' in ret:
             call_stmt = f"{call_stmt}.get_raw()"
             
        # handle special cases
        if 'ecs_query_t*' in ret:
             call_stmt = f"0; /* TODO: map flecs::query to ecs_query_t* */"
        if 'register_serializer' in short_name:
             call_stmt = f"/* TODO: map register_serializer types */"
        if 'execute_script' in short_name:
             call_stmt = f"/* TODO: map execute_script types */"
        if 'create_entity' in short_name:
             call_stmt = f"0; /* TODO: implement create_entity */"
             
        if ret != 'void':
            if call_stmt.startswith('0;'):
                cpp_content += f"            if (module) return {call_stmt}\n"
            else:
                cpp_content += f"            if (module) return {call_stmt};\n"
            if '*' in ret or 'entity_t' in ret:
                cpp_content += f"            return 0;\n"
            elif 'handle_t' in ret:
                cpp_content += f"            return {{0}};\n"
            elif ret == 'bool':
                cpp_content += f"            return false;\n"
            elif 'float' in ret or 'int' in ret or 'double' in ret or 'uint' in ret:
                cpp_content += f"            return 0;\n"
            else:
                cpp_content += f"            return {{0}};\n"
        else:
            if call_stmt.startswith('/*'):
                cpp_content += f"            if (module) {{ {call_stmt} }}\n"
            else:
                cpp_content += f"            if (module) {call_stmt};\n"
            
        cpp_content += f"        }};\n"

    cpp_content += f"""
        world.component<{prefix}service_t>().set(svc);
    }}
"""

    # Generate stubs for the class methods
    for mret, mname, margs, mconst in methods:
        if 'private' in mret or 'public' in mret or 'protected' in mret: continue
        const_str = " const" if mconst else ""
        cpp_content += f"\n    {mret} {class_name}::{mname}({margs}){const_str} {{\n"
        if mret != 'void':
            if '*' in mret:
                cpp_content += f"        return nullptr;\n"
            elif 'entity' in mret:
                cpp_content += f"        return flecs::entity::null();\n"
            elif 'sandbox::properties' in mret:
                cpp_content += f"        return sandbox::properties{{}};\n"
            elif mret == 'bool':
                cpp_content += f"        return false;\n"
            elif 'float' in mret or 'int' in mret or 'double' in mret or 'uint' in mret:
                cpp_content += f"        return 0;\n"
            else:
                cpp_content += f"        return {{}};\n"
        cpp_content += f"    }}\n"

    cpp_content += f"\n}}\n"
    
    with open(cpp_path, 'w') as f:
        f.write(cpp_content)
        
print("Generated _module.cpp files with module setup and stubs.")
