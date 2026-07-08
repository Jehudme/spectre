import re
import os

header = open('spectre/include/spectre/spectre.h').read()

modules = {
    'scenes': 'spectre_scenes_',
    'components': 'spectre_components_',
    'serializer': 'spectre_serializer_',
    'renderer': 'spectre_renderer_',
    'resource': 'spectre_resource_',
    'script': 'spectre_script_',
    'window': 'spectre_window_'
}

funcs = []
pattern = re.compile(r'^([a-zA-Z_][a-zA-Z0-9_*\s]*?)\s+([a-zA-Z0-9_]+)\((.*?)\);', re.MULTILINE)
for match in pattern.finditer(header):
    ret, name, args = match.groups()
    if name.startswith('spectre_'):
        funcs.append((ret.strip(), name.strip(), args.strip()))

# Generate services.h
services_h = """#pragma once
#include <flecs.h>
#include "components.h"

#ifdef __cplusplus
extern "C" {
#endif

"""

# Generate spectre.cpp
spectre_cpp = """#include "spectre/spectre.h"
#include "spectre/services.h"

"""

for mod, prefix in modules.items():
    services_h += f"// --- {mod} ---\ntypedef struct {prefix}service_t {{\n"
    
    mod_funcs = [f for f in funcs if f[1].startswith(prefix)]
    for ret, name, args in mod_funcs:
        short_name = name[len(prefix):]
        services_h += f"    {ret} (*{short_name})({args});\n"
        
        # generate spectre.cpp
        spectre_cpp += f"{ret} {name}({args}) {{\n"
        spectre_cpp += f"    flecs::world w(world);\n"
        spectre_cpp += f"    ecs_entity_t comp_id = w.component<{prefix}service_t>().id();\n"
        spectre_cpp += f"    const {prefix}service_t* svc = (const {prefix}service_t*)ecs_get_id(world, comp_id, comp_id);\n"
        
        arg_names = []
        for arg in args.split(','):
            arg = arg.strip()
            # extract name (last word after removing pointer asterisks)
            arg = re.sub(r'\[.*?\]', '', arg) # remove arrays
            parts = arg.split()
            name_part = parts[-1].replace('*', '')
            arg_names.append(name_part)
            
        arg_list = ', '.join(arg_names)
        
        if ret != 'void':
            spectre_cpp += f"    if (svc && svc->{short_name}) return svc->{short_name}({arg_list});\n"
            if '*' in ret or 'entity_t' in ret:
                spectre_cpp += f"    return 0;\n"
            elif 'handle_t' in ret:
                spectre_cpp += f"    return {{0}};\n"
            elif ret == 'bool':
                spectre_cpp += f"    return false;\n"
            elif 'float' in ret or 'int' in ret or 'double' in ret:
                spectre_cpp += f"    return 0;\n"
            else:
                spectre_cpp += f"    return {{0}};\n"
        else:
            spectre_cpp += f"    if (svc && svc->{short_name}) svc->{short_name}({arg_list});\n"
            
        spectre_cpp += "}\n\n"
        
    services_h += f"}} {prefix}service_t;\n\n"

services_h += """#ifdef __cplusplus
}
#endif
"""

with open('spectre/include/spectre/services.h', 'w') as f:
    f.write(services_h)

with open('spectre/source/spectre.cpp', 'w') as f:
    f.write(spectre_cpp)

# Also generate boilerplate for _module.cpp
for mod, prefix in modules.items():
    mod_name = mod
    if mod == 'script':
        mod_name = 'scripts'
    elif mod == 'components':
        mod_name = 'prefabs'
    
    filename = f"spectre/source/modules/{mod_name}/{mod_name}_module.cpp"
    if os.path.exists(filename):
        content = open(filename).read()
        if "world.component<" not in content:
            # Let's inject world.module and world.component
            # It's tricky to inject precisely, we can just leave a comment or rewrite simple boilerplate.
            pass

print("Generated services.h and spectre.cpp")
