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
pattern = re.compile(r'^([a-zA-Z_][a-zA-Z0-9_*:&<>\s]*?)\s+([a-zA-Z0-9_]+)\((.*?)\);', re.MULTILINE)
for match in pattern.finditer(header):
    ret, name, args = match.groups()
    if name.startswith('spectre_'):
        funcs.append((ret.strip(), name.strip(), args.strip()))

os.makedirs('spectre/include/spectre/abi', exist_ok=True)

for mod, (prefix, folder, class_name) in modules_info.items():
    mod_funcs = [f for f in funcs if f[1].startswith(prefix)]
    
    header_path = f"spectre/source/modules/{folder}/{folder}_module.h"
    if not os.path.exists(header_path):
        if folder == 'prefabs':
            header_path = "spectre/source/modules/prefabs/prefabs_module.h"
            
    header_content = ""
    try:
        header_content = open(header_path).read()
    except Exception:
        pass
        
    class_pattern = re.compile(r'class\s+([a-zA-Z0-9_]+)[\s\S]*?\{([\s\S]*?)\};')
    class_match = class_pattern.search(header_content)
    methods = []
    if class_match:
        class_name = class_match.group(1)
        class_body = class_match.group(2)
        method_pattern = re.compile(r'^\s*([a-zA-Z_][a-zA-Z0-9_*:&<>\s]*?)\s+([a-zA-Z0-9_]+)\((.*?)\)\s*(const)?\s*;', re.MULTILINE)
        for mmatch in method_pattern.finditer(class_body):
            mret, mname, margs, mconst = mmatch.groups()
            if mname == class_name or mname.startswith('operator') or mname.startswith('~'):
                continue
            methods.append((mret.strip(), mname.strip(), margs.strip(), mconst))

    # --- Generate ABI Header ---
    abi_header_path = f"spectre/include/spectre/abi/{folder}_service.h"
    abi_content = f"""#pragma once
#include <sandbox/abi/bootstrapper.h>
#include <sandbox/abi/properties.h>
#include "spectre/components.h" // for custom types

#ifdef __cplusplus
extern "C" {{
#endif

typedef struct spectre_{folder}_api_t {{
"""
    for ret, name, args in mod_funcs:
        short_name = name[len(prefix):]
        abi_content += f"    {ret} (*{short_name})({args});\n"
        
    abi_content += f"""}} spectre_{folder}_api_t;

SANDBOX_DECLARE_SERVICE(spectre_{folder}_service_t, spectre_{folder}_api_t, {{
    .struct_size = 0,
    .name = "{folder}",
    .description = "{folder.capitalize()} Service",
    .architecture = "spectre",
    .version_major = 1,
    .version_minor = 0,
    .init_fn = NULL
}});

#ifdef __cplusplus
}}
#endif
"""
    with open(abi_header_path, 'w') as f:
        f.write(abi_content)


    # --- Generate Module CPP ---
    cpp_path = f"spectre/source/modules/{folder}/{folder}_module.cpp"
    if not os.path.exists(cpp_path):
        continue

    cpp_content = f"""#include "{os.path.basename(header_path)}"
#include "spectre/abi/{folder}_service.h"
#include <iostream>

namespace spectre::modules {{

"""
    # 1. Forwarding static functions
    for ret, name, args in mod_funcs:
        short_name = name[len(prefix):]
        
        arg_names = []
        for arg in args.split(','):
            arg = arg.strip()
            if not arg: continue
            arg = re.sub(r'\[.*?\]', '', arg)
            parts = arg.split()
            name_part = parts[-1].replace('*', '')
            arg_names.append(name_part)
            
        forward_args = []
        for arg, t in zip(arg_names[1:], args.split(',')[1:]):
            if 'sandbox_properties_handle_t' in t:
                forward_args.append(f"sandbox::properties({arg})")
            elif 'ecs_entity_t' in t:
                forward_args.append(f"w.entity({arg})")
            else:
                forward_args.append(arg)
                
        forward_list = ', '.join(forward_args)
        
        cpp_content += f"    static {ret} {name}_fwd({args}) {{\n"
        cpp_content += f"        flecs::world w(world);\n"
        cpp_content += f"        ecs_entity_t id = w.component<{class_name}>().id();\n"
        cpp_content += f"        {class_name}* module = ({class_name}*)ecs_get_mut_id(world, id, id);\n"
        
        call_stmt = f"module->{short_name}({forward_list})"
        if 'sandbox_properties_handle_t' in ret:
             call_stmt = f"{call_stmt}.get_raw()"
             
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
                cpp_content += f"        if (module) return {call_stmt}\n"
            else:
                cpp_content += f"        if (module) return {call_stmt};\n"
            if '*' in ret or 'entity_t' in ret:
                cpp_content += f"        return 0;\n"
            elif 'handle_t' in ret:
                cpp_content += f"        return {{0}};\n"
            elif ret == 'bool':
                cpp_content += f"        return false;\n"
            elif 'float' in ret or 'int' in ret or 'double' in ret or 'uint' in ret:
                cpp_content += f"        return 0;\n"
            else:
                cpp_content += f"        return {{0}};\n"
        else:
            if call_stmt.startswith('/*'):
                cpp_content += f"        if (module) {{ {call_stmt} }}\n"
            else:
                cpp_content += f"        if (module) {call_stmt};\n"
            
        cpp_content += f"    }}\n\n"

    # 2. Define API struct
    cpp_content += f"    static spectre_{folder}_api_t {folder}_api = {{\n"
    for ret, name, args in mod_funcs:
        short_name = name[len(prefix):]
        cpp_content += f"        .{short_name} = {name}_fwd,\n"
    cpp_content += f"    }};\n\n"

    # 3. Define Service
    cpp_content += f"    SANDBOX_DEFINE_SERVICE(spectre_{folder}_service_t, spectre_{folder}_api_t, &{folder}_api);\n\n"

    # 4. Declare Module
    cpp_content += f"""    SANDBOX_DECLARE_MODULE({class_name}, {{
        .name = "{folder}",
        .description = "{folder.capitalize()} Module",
        .architecture = "spectre",
        .version_major = 1,
        .version_minor = 0,
        .version_patch = 0,
        .service = &spectre_{folder}_service_t_info_decl,
        .requirements = nullptr,
        .requirement_count = 0
    }})
"""

    # 5. Class Constructor and stubs
    cpp_content += f"\n    {class_name}::{class_name}(flecs::world& world) : m_world(world) {{\n"
    cpp_content += f"        // Optional: module initialization\n"
    cpp_content += f"    }}\n"
    cpp_content += f"\n    {class_name}::~{class_name}() = default;\n"
    
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
        
print("Generated sandbox-style modules and ABI headers.")
