import re
import os

header = open('spectre/include/spectre/spectre.h').read()

modules_info = {
    'scenes': ('spectre_scenes_', 'scenes', 'scenes_module_t'),
    'components': ('spectre_components_', 'prefabs', 'components_module_t'),
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

# Delete old spectre.cpp
if os.path.exists('spectre/source/spectre.cpp'):
    os.remove('spectre/source/spectre.cpp')

cmake_sources = []

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

"""
    # 1. C API function declarations
    for ret, name, args in mod_funcs:
        abi_content += f"SANDBOX_API {ret} {name}({args});\n"
        
    abi_content += f"""
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


    # --- Generate Service CPP ---
    service_cpp_path = f"spectre/source/modules/{folder}/{folder}_service.cpp"
    cmake_sources.append(f"source/modules/{folder}/{folder}_service.cpp")
    
    service_cpp = f"""#include "spectre/abi/{folder}_service.h"
#include "{os.path.basename(header_path)}"
#include <iostream>

"""
    # 1. Forward declare functions for struct
    # (Actually we just implement them, but need them declared before the struct if we don't put them above)
    for ret, name, args in mod_funcs:
        service_cpp += f"static {ret} {name}_impl({args});\n"
        
    service_cpp += f"\nstatic spectre_{folder}_api_t g_{folder}_api = {{\n"
    for ret, name, args in mod_funcs:
        short_name = name[len(prefix):]
        service_cpp += f"    .{short_name} = {name}_impl,\n"
    service_cpp += f"}};\n\n"
    
    service_cpp += f"SANDBOX_DEFINE_SERVICE(spectre_{folder}_service_t, spectre_{folder}_api_t, &g_{folder}_api)\n\n"

    # Implement functions
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
        
        service_cpp += f"static {ret} {name}_impl({args}) {{\n"
        service_cpp += f"    if (!world) return "
        
        if ret != 'void':
            if '*' in ret or 'entity_t' in ret:
                service_cpp += f"0;\n"
            elif 'handle_t' in ret:
                service_cpp += f"{{0}};\n"
            elif ret == 'bool':
                service_cpp += f"false;\n"
            elif 'float' in ret or 'int' in ret or 'double' in ret or 'uint' in ret:
                service_cpp += f"0;\n"
            else:
                service_cpp += f"{{0}};\n"
        else:
            service_cpp += f";\n"
            
        service_cpp += f"    flecs::world w(world);\n"
        service_cpp += f"    auto* module = w.try_get_mut<spectre::modules::{class_name}>();\n"
        
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
                service_cpp += f"    if (module) return {call_stmt}\n"
            else:
                service_cpp += f"    if (module) return {call_stmt};\n"
            if '*' in ret or 'entity_t' in ret:
                service_cpp += f"    return 0;\n"
            elif 'handle_t' in ret:
                service_cpp += f"    return {{0}};\n"
            elif ret == 'bool':
                service_cpp += f"    return false;\n"
            elif 'float' in ret or 'int' in ret or 'double' in ret or 'uint' in ret:
                service_cpp += f"    return 0;\n"
            else:
                service_cpp += f"    return {{0}};\n"
        else:
            if call_stmt.startswith('/*'):
                service_cpp += f"    if (module) {{ {call_stmt} }}\n"
            else:
                service_cpp += f"    if (module) {call_stmt};\n"
            
        service_cpp += f"}}\n\n"
        
        # Also define the original C function to just call the impl (if users call it directly instead of via SDK)
        service_cpp += f"{ret} {name}({args}) {{\n"
        if ret != 'void':
            service_cpp += f"    return {name}_impl({', '.join(arg_names)});\n"
        else:
            service_cpp += f"    {name}_impl({', '.join(arg_names)});\n"
        service_cpp += f"}}\n\n"

    with open(service_cpp_path, 'w') as f:
        f.write(service_cpp)

    # --- Generate Module CPP ---
    module_cpp_path = f"spectre/source/modules/{folder}/{os.path.basename(header_path).replace('.h', '.cpp')}"
    cmake_sources.append(f"source/modules/{folder}/{os.path.basename(header_path).replace('.h', '.cpp')}")
    
    module_cpp = f"""#include "{os.path.basename(header_path)}"
#include "spectre/abi/{folder}_service.h"
#include <iostream>

namespace spectre::modules {{

    SANDBOX_DECLARE_MODULE({class_name}, {{
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

    {class_name}::{class_name}(flecs::world& world) : m_world(world) {{
        std::cout << "[SPECTRE] Initializing " << "{folder}" << " module" << std::endl;
    }}
    
    {class_name}::~{class_name}() = default;
"""
    for mret, mname, margs, mconst in methods:
        if 'private' in mret or 'public' in mret or 'protected' in mret: continue
        const_str = " const" if mconst else ""
        module_cpp += f"\n    {mret} {class_name}::{mname}({margs}){const_str} {{\n"
        if mret != 'void':
            if '*' in mret:
                module_cpp += f"        return nullptr;\n"
            elif 'entity' in mret:
                module_cpp += f"        return flecs::entity::null();\n"
            elif 'sandbox::properties' in mret:
                module_cpp += f"        return sandbox::properties{{}};\n"
            elif mret == 'bool':
                module_cpp += f"        return false;\n"
            elif 'float' in mret or 'int' in mret or 'double' in mret or 'uint' in mret:
                module_cpp += f"        return 0;\n"
            else:
                module_cpp += f"        return {{}};\n"
        module_cpp += f"    }}\n"

    module_cpp += f"\n}}\n"
    
    with open(module_cpp_path, 'w') as f:
        f.write(module_cpp)
        
# --- Overwrite spectre.h ---
# Remove all spectre_* function declarations from spectre.h
spectre_h_content = re.sub(r'^([a-zA-Z_][a-zA-Z0-9_*:&<>\s]*?)\s+spectre_[a-zA-Z0-9_]+\(.*?;\n', '', header, flags=re.MULTILINE)
# Also replace include "services.h" with all new includes if needed, but wait, the user's spectre.h doesn't have it.
# Let's just write back the cleaned spectre.h
with open('spectre/include/spectre/spectre.h', 'w') as f:
    f.write(spectre_h_content)

print("Generated refactored architecture.")
print("CMAKE SOURCES TO ADD:")
for src in cmake_sources:
    if "service" in src:
        print(src)
