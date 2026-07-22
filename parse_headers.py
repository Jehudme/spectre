import os
import re
import glob

def find_headers(dirs):
    headers = []
    for d in dirs:
        for root, _, files in os.walk(d):
            for f in files:
                if f.endswith(".h"):
                    headers.append(os.path.join(root, f))
    return headers

def extract_apis(headers):
    funcs = []
    for h in headers:
        with open(h, "r") as f:
            content = f.read()
            # Match SANDBOX_API ...
            sandbox_apis = re.findall(r'SANDBOX_API\s+(.*?);', content)
            for api in sandbox_apis:
                funcs.append(api + ";")
            # Match spectre_ ... that are not inside structs or typedefs
            # Actually, just search for lines starting with return type and spectre_
            lines = content.split('\n')
            for i, line in enumerate(lines):
                if line.startswith("void spectre_") or line.startswith("bool spectre_") or line.startswith("ecs_entity_t spectre_") or line.startswith("sandbox_properties_handle_t spectre_") or line.startswith("void* spectre_"):
                    # Extract until ';'
                    decl = line
                    j = i
                    while ';' not in decl and j < len(lines) - 1:
                        j += 1
                        decl += " " + lines[j].strip()
                    funcs.append(decl)
    return funcs

dirs = ["../sandbox/modules/core/include/sandbox/services", "spectre/include/spectre/services", "../sandbox/sandbox/include/sandbox/abi"]
funcs = extract_apis(find_headers(dirs))
for f in funcs:
    print(f)
