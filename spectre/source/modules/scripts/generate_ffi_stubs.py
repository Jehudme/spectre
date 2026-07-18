import re
import sys
import os

def c_type_to_lua_type(ctype):
    ctype = ctype.replace('const', '').replace('struct', '').replace('enum', '').strip()
    if '*' in ctype:
        if 'char' in ctype:
            return 'string'
        return 'any'
    if ctype in ['int', 'float', 'double', 'int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t', 'int64_t', 'uint64_t', 'size_t', 'ecs_size_t', 'ecs_entity_t', 'ecs_id_t']:
        return 'number'
    if ctype in ['bool', '_Bool']:
        return 'boolean'
    if ctype in ['char', 'unsigned char', 'signed char']:
        return 'number'
    if ctype == 'void':
        return 'nil'
    return 'any'

def parse_header(content):
    stubs = []
    stubs.append("---@meta")
    stubs.append("")
    stubs.append("ffi = ffi or {}")
    stubs.append("ffi.C = ffi.C or {}")
    stubs.append("")

    # Find structs
    struct_pattern = re.compile(r'typedef\s+struct\s+([a-zA-Z0-9_]+)\s*\{([^}]*)\}\s*\1\s*;')
    for match in struct_pattern.finditer(content):
        name = match.group(1)
        fields_text = match.group(2)
        stubs.append(f"---@class {name}")
        
        for line in fields_text.split(';'):
            line = line.strip()
            if not line: continue
            parts = line.split()
            if len(parts) >= 2:
                fname = parts[-1].lstrip('*')
                ftype = ' '.join(parts[:-1]) + '*' * (len(parts[-1]) - len(fname))
                
                if '[' in fname:
                    fname = fname.split('[')[0]
                
                ltype = c_type_to_lua_type(ftype)
                stubs.append(f"---@field {fname} {ltype}")
        
        # We shouldn't declare `local StructName = {}` for structs, because C structs don't exist as global Lua tables!
        # Just the ---@class annotation is enough for EmmyLua/Sumneko!
        # But we can add a dummy global if they want to access it, though they usually use ffi.new("StructName").
        # Better to just leave it as an annotation.

    # Find enums
    enum_pattern = re.compile(r'typedef\s+enum\s+([a-zA-Z0-9_]+)\s*\{([^}]*)\}\s*\1\s*;')
    for match in enum_pattern.finditer(content):
        name = match.group(1)
        fields_text = match.group(2)
        stubs.append(f"---@enum {name}")
        stubs.append(f"{name} = {{")
        for line in fields_text.split(','):
            line = line.strip()
            if not line: continue
            enum_key = line.split('=')[0].strip()
            stubs.append(f"    {enum_key} = 0,")
        stubs.append("}\n")
        
    flat_content = content.replace('\n', ' ')
    flat_content = re.sub(r'\s+', ' ', flat_content)
    
    # functions
    func_pattern = re.compile(r'([a-zA-Z_][a-zA-Z0-9_\s\*]*?)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*;')
    for match in func_pattern.finditer(flat_content):
        ret_type = match.group(1).strip()
        func_name = match.group(2).strip()
        args_text = match.group(3).strip()
        
        # Filter out typedefs and control structures
        if any(kw in ret_type for kw in ['typedef', 'return', 'else', 'if', 'struct ']) and not ret_type.startswith('extern ') and not ret_type.startswith('void '):
            if not (ret_type.startswith('struct') and '*' in ret_type):
                continue
                
        if func_name in ['if', 'while', 'for', 'switch', 'return', 'else', 'typedef']:
            continue
            
        l_ret = c_type_to_lua_type(ret_type)
        
        args = []
        param_docs = []
        if args_text and args_text != 'void':
            for arg in args_text.split(','):
                arg = arg.strip()
                if not arg or arg == '...':
                    args.append('...')
                    continue
                parts = arg.split()
                if len(parts) >= 2:
                    aname = parts[-1].lstrip('*')
                    atype = ' '.join(parts[:-1]) + '*' * (len(parts[-1]) - len(aname))
                    
                    if '[' in aname:
                        aname = aname.split('[')[0]
                    if aname == '':
                        aname = f'arg{len(args)}'
                        
                    lua_keywords = ['and', 'break', 'do', 'else', 'elseif', 'end', 'false', 'for', 'function', 'if', 'in', 'local', 'nil', 'not', 'or', 'repeat', 'return', 'then', 'true', 'until', 'while', 'type']
                    if aname in lua_keywords:
                        aname = aname + '_'
                        
                    args.append(aname)
                    l_type = c_type_to_lua_type(atype)
                    param_docs.append(f"---@param {aname} {l_type}")
                else:
                    args.append(f'arg{len(args)}')
                    
        for p in param_docs:
            stubs.append(p)
        stubs.append(f"---@return {l_ret}")
        stubs.append(f"function ffi.C.{func_name}({', '.join(args)}) end\n")

    return "\n".join(stubs)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python3 generate_ffi_stubs.py <input.h> <output.lua>")
        sys.exit(1)
        
    with open(sys.argv[1], 'r') as f:
        content = f.read()
        
    stubs = parse_header(content)
    
    os.makedirs(os.path.dirname(os.path.abspath(sys.argv[2])), exist_ok=True)
    with open(sys.argv[2], 'w') as f:
        f.write(stubs)
