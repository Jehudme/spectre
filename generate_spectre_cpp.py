import re
import os

header = open('spectre/include/spectre/spectre.h').read()

modules_info = {
    'scenes': ('spectre_scenes_', 'scenes'),
    'components': ('spectre_components_', 'prefabs'),
    'serializer': ('spectre_serializer_', 'serializer'),
    'renderer': ('spectre_renderer_', 'renderer'),
    'resource': ('spectre_resource_', 'resources'),
    'script': ('spectre_script_', 'scripts'),
    'window': ('spectre_window_', 'window')
}

funcs = []
pattern = re.compile(r'^([a-zA-Z_][a-zA-Z0-9_*:&<>\s]*?)\s+([a-zA-Z0-9_]+)\((.*?)\);', re.MULTILINE)
for match in pattern.finditer(header):
    ret, name, args = match.groups()
    if name.startswith('spectre_'):
        funcs.append((ret.strip(), name.strip(), args.strip()))

spectre_cpp = """#include "spectre/spectre.h"
#include <sandbox/abi/bootstrapper.h>

// Include ABI headers
"""
for folder in set(f for _, f in modules_info.values()):
    spectre_cpp += f'#include "spectre/abi/{folder}_service.h"\n'

spectre_cpp += "\n"

for mod, (prefix, folder) in modules_info.items():
    mod_funcs = [f for f in funcs if f[1].startswith(prefix)]
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
            
        arg_list = ', '.join(arg_names)
        
        spectre_cpp += f"{ret} {name}({args}) {{\n"
        spectre_cpp += f"    flecs::world w(world);\n"
        spectre_cpp += f"    if (const auto* service = SANDBOX_GET_SERVICE(w, spectre_{folder}_service_t)) {{\n"
        spectre_cpp += f"        if (service->api && service->api->{short_name}) {{\n"
        if ret != 'void':
            spectre_cpp += f"            return service->api->{short_name}({arg_list});\n"
        else:
            spectre_cpp += f"            service->api->{short_name}({arg_list});\n"
            spectre_cpp += f"            return;\n"
        spectre_cpp += f"        }}\n"
        spectre_cpp += f"    }}\n"
        
        if ret != 'void':
            if '*' in ret or 'entity_t' in ret:
                spectre_cpp += f"    return 0;\n"
            elif 'handle_t' in ret:
                spectre_cpp += f"    return {{0}};\n"
            elif ret == 'bool':
                spectre_cpp += f"    return false;\n"
            elif 'float' in ret or 'int' in ret or 'double' in ret or 'uint' in ret:
                spectre_cpp += f"    return 0;\n"
            else:
                spectre_cpp += f"    return {{0}};\n"
        
        spectre_cpp += "}\n\n"

with open('spectre/source/spectre.cpp', 'w') as f:
    f.write(spectre_cpp)
    
print("Generated spectre.cpp")
