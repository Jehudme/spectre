import re

def fix_sandbox_lua():
    with open("/home/jehud/CLionProjects/sandbox/sandbox/include/sandbox/abi/properties.h", "r") as f:
        properties_h = f.read()

    # Extract all functions
    funcs = []
    for line in properties_h.split('\n'):
        line = line.strip()
        if not line or line.startswith("//") or line.startswith("/*") or line.startswith("*") or line.startswith("#"): continue
        if line.startswith("SANDBOX_DEFINE_HANDLE"): continue
        if "(" in line and ")" in line and ";" in line:
            funcs.append(line)

    with open("tests/application/resources/app/resources/assets/scripts/externals/sandbox.lua", "r") as f:
        content = f.read()

    # Insert properties functions into ffi.cdef
    cdef_end = content.find("]]")
    
    new_cdef_lines = "\n    -- Properties API\n"
    for func in funcs:
        new_cdef_lines += "    " + func + "\n"
        
    content = content[:cdef_end] + new_cdef_lines + content[cdef_end:]
    
    with open("tests/application/resources/app/resources/assets/scripts/externals/sandbox.lua", "w") as f:
        f.write(content)

fix_sandbox_lua()
