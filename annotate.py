import re
import os

def annotate_file(path, module_name):
    with open(path, 'r') as f:
        lines = f.readlines()
        
    out = []
    
    # Prepend the module class itself
    out.append(f"---@class {module_name}\n")
    out.append(f"{module_name} = {{}}\n\n")

    for i, line in enumerate(lines):
        # We don't want to duplicate `module = {}` if it exists, so we just skip it
        if re.match(rf'^{module_name}\s*=\s*{{}}', line):
            continue

        # Look for table definitions
        m_table = re.match(rf'^({module_name}\.[\w\.]+)\s*=\s*{{}}', line)
        if m_table:
            table_path = m_table.group(1)
            out.append(f"---@class {table_path}\n")
            out.append(line)
            continue
            
        m_func = re.match(r'^function ([\w\.\:]+)\((.*?)\)', line)
        if m_func:
            func_name = m_func.group(1)
            args_str = m_func.group(2)
            
            if func_name.startswith(module_name) or func_name.startswith("ecs."):
                args = [a.strip() for a in args_str.split(',')] if args_str else []
                for arg in args:
                    if arg == '' or arg == '...': continue
                    t = "any"
                    if arg in ("world", "ecs"): t = "any"
                    elif arg in ("msg", "message", "path", "name", "virtual_path", "physical_path", "virtual_mount_point", "source_virtual_path", "dest_virtual_path", "mount_point", "path_str", "key"): t = "string"
                    elif arg in ("entity", "entity_id", "scene_id", "state_id", "self_id", "serializer", "target", "bytes_to_read", "bytes_to_write", "position", "count", "idx"): t = "integer"
                    elif "is_" in arg or arg in ("read_only", "append", "force_path", "overwrite", "recursive", "value"): t = "boolean"
                    elif arg in ("props", "handle", "other"): t = "any"
                    elif arg == "buffer": t = "any"
                    out.append(f"---@param {arg} {t}\n")
                
                # Add return type if there's a return statement inside
                j = i + 1
                returns = False
                while j < len(lines) and not lines[j].startswith("end"):
                    if "return " in lines[j]:
                        returns = True
                        break
                    j += 1
                if returns:
                    out.append("---@return any\n")
                    
        out.append(line)
        
    with open(path, 'w') as f:
        f.writelines(out)

annotate_file('/home/jehud/CLionProjects/spectre/tests/application/resources/app/resources/assets/scripts/externals/sandbox.lua', 'sandbox')
annotate_file('/home/jehud/CLionProjects/spectre/tests/application/resources/app/resources/assets/scripts/externals/spectre.lua', 'spectre')
