import os
import re

def parse_header(file_path):
    with open(file_path, "r") as f:
        content = f.read()

    # Find extern "C" { ... }
    extern_c = re.search(r'extern "C"\s*\{([\s\S]*?)\}', content)
    if not extern_c:
        return ""
    
    code = extern_c.group(1)
    
    # Remove comments
    code = re.sub(r'/\*[\s\S]*?\*/', '', code)
    code = re.sub(r'//.*', '', code)
    
    # Remove SANDBOX_DECLARE_SERVICE
    code = re.sub(r'SANDBOX_DECLARE_SERVICE\([\s\S]*?\);', '', code)
    
    # Remove empty lines
    lines = [l.strip() for l in code.split('\n') if l.strip()]
    return '\n'.join(lines)

def process_headers(dirs):
    cdef = ""
    for d in dirs:
        for root, _, files in os.walk(d):
            for file in files:
                if file.endswith(".h"):
                    path = os.path.join(root, file)
                    code = parse_header(path)
                    if code:
                        cdef += code + "\n"
    return cdef

cdef = process_headers(["../sandbox/modules/core/include/sandbox/services", "spectre/include/spectre/services", "../sandbox/sandbox/include/sandbox/abi"])

with open("cdef.txt", "w") as f:
    f.write(cdef)
