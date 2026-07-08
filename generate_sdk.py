import os
import re

def process_spectre_services():
    services_dir = '/home/jehud/CLionProjects/spectre/spectre/include/spectre/services'
    sdk_dir = '/home/jehud/CLionProjects/spectre/spectre/include/spectre/sdk'
    cpp_dir = '/home/jehud/CLionProjects/spectre/spectre/source/modules'
    
    if not os.path.exists(sdk_dir):
        os.makedirs(sdk_dir)
        
    for filename in os.listdir(services_dir):
        if not filename.endswith('_service.h'):
            continue
            
        module_name = filename.replace('_service.h', '')
        service_path = os.path.join(services_dir, filename)
        
        with open(service_path, 'r') as f:
            content = f.read()
            
        # Parse out the function pointers from the api_t struct
        struct_pattern = r'typedef\s+struct\s+spectre_' + module_name + r'_api_t\s*\{([^}]*)\}\s*spectre_' + module_name + r'_api_t;'
        struct_match = re.search(struct_pattern, content)
        
        sdk_functions = []
        if struct_match:
            lines = struct_match.group(1).strip().split('\n')
            for line in lines:
                line = line.strip()
                if not line or line.startswith('//'):
                    continue
                # e.g., ecs_entity_t (*deserialize_window)(ecs_world_t* world, sandbox_properties_handle_t props);
                # e.g., void (*set_size)(ecs_world_t* world, uint32_t width, uint32_t height);
                fn_match = re.search(r'(.*?)\s*\(\*([a-zA-Z0-9_]+)\)\((.*?)\);', line)
                if fn_match:
                    ret_type = fn_match.group(1).strip()
                    fn_name = fn_match.group(2).strip()
                    args_str = fn_match.group(3).strip()
                    
                    # Split args
                    args = [a.strip() for a in args_str.split(',')]
                    
                    # First arg is ecs_world_t* world
                    # So we construct C++ args
                    cpp_args = ["const flecs::world& entity_world"]
                    c_call_args = ["entity_world.c_ptr()"]
                    
                    for arg in args[1:]:
                        if arg:
                            cpp_args.append(arg)
                            # Get the variable name (last word)
                            var_name_match = re.search(r'([a-zA-Z0-9_]+)$', arg)
                            if var_name_match:
                                c_call_args.append(var_name_match.group(1))
                            else:
                                c_call_args.append(arg)
                                
                    cpp_args_str = ", ".join(cpp_args)
                    c_call_args_str = ", ".join(c_call_args)
                    
                    return_statement = "return " if ret_type != "void" else ""
                    default_return = ""
                    if ret_type != "void":
                        if ret_type.endswith('*'):
                            default_return = " return nullptr;"
                        elif ret_type == "bool":
                            default_return = " return false;"
                        elif ret_type == "ecs_entity_t" or ret_type == "uint32_t" or ret_type == "int32_t" or ret_type == "float" or ret_type == "double" or ret_type == "size_t":
                            default_return = " return 0;"
                        else:
                            default_return = " return {0};"
                    
                    func_body = f"""        static {ret_type} {fn_name}({cpp_args_str}) {{
            if (const auto* service = SANDBOX_GET_SERVICE(entity_world, spectre_{module_name}_service_t)) {{
                if (service->api && service->api->{fn_name}) {{
                    {return_statement}service->api->{fn_name}({c_call_args_str});
                }}
            }}{default_return}
        }}"""
                    sdk_functions.append(func_body)
        
        if not sdk_functions:
            print(f"Skipping {module_name} (no API struct found)")
            continue
            
        sdk_class = "\n\n".join(sdk_functions)
        
        sdk_content = f"""#pragma once
#include <spectre/services/{filename}>

#ifdef __cplusplus
namespace spectre::modules {{
    class {module_name} {{
    public:
{sdk_class}
    }};
}}
#endif
"""
        sdk_path = os.path.join(sdk_dir, f"{module_name}.hpp")
        with open(sdk_path, 'w') as f:
            f.write(sdk_content)
        
        print(f"Generated {sdk_path}")
        
        # Remove SANDBOX_API lines from _service.h
        new_header_lines = []
        for line in content.split('\n'):
            if line.strip().startswith('SANDBOX_API'):
                continue
            if line.strip() == '// --- Public C API ---':
                continue
            new_header_lines.append(line)
        
        with open(service_path, 'w') as f:
            f.write('\n'.join(new_header_lines))
        print(f"Cleaned {service_path}")
        
        # Remove implementations from _service.cpp
        cpp_file_path = os.path.join(cpp_dir, module_name if module_name != 'scenes' else 'scenes', f"{module_name}_service.cpp")
        if not os.path.exists(cpp_file_path):
            cpp_file_path = os.path.join(cpp_dir, module_name + 's' if not module_name.endswith('s') else module_name, f"{module_name}_service.cpp")
            
        # Search recursively for the correct _service.cpp path just in case
        actual_cpp_path = None
        for root, dirs, files in os.walk(cpp_dir):
            if f"{module_name}_service.cpp" in files:
                actual_cpp_path = os.path.join(root, f"{module_name}_service.cpp")
                break
                
        if actual_cpp_path:
            with open(actual_cpp_path, 'r') as f:
                cpp_content = f.read()
            
            # Remove everything from "// --- Public C API Implementations ---" to the end
            api_impl_marker = "// --- Public C API Implementations ---"
            if api_impl_marker in cpp_content:
                cpp_content = cpp_content.split(api_impl_marker)[0].strip() + '\n'
                with open(actual_cpp_path, 'w') as f:
                    f.write(cpp_content)
                print(f"Cleaned {actual_cpp_path}")

process_spectre_services()
