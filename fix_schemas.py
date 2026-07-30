import os
import re
import json

directory = '/home/jehud/CLionProjects/spectre/spectre/source/modules'

pattern = re.compile(r'R"\(\{"members":\s*\[(.*?)\]\}\)"', re.DOTALL)

def replace_array(match):
    inner = match.group(1)
    if not inner.strip():
        return r'R"({"members":{}})"'
    
    json_str = f"[{inner}]"
    try:
        arr = json.loads(json_str)
        obj = {str(i): item for i, item in enumerate(arr)}
        new_inner = json.dumps({"members": obj}, separators=(',', ':'))
        return f'R"({new_inner})"'
    except Exception as e:
        print(f"Failed to parse {json_str}: {e}")
        return match.group(0)

for root, _, files in os.walk(directory):
    for f in files:
        if f.endswith('.cpp'):
            path = os.path.join(root, f)
            with open(path, 'r') as file:
                content = file.read()
            
            new_content = pattern.sub(replace_array, content)
            
            if new_content != content:
                print(f"Updated {path}")
                with open(path, 'w') as file:
                    file.write(new_content)
