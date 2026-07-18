import re

def c_type_to_lua_type(ctype):
    ctype = ctype.replace('const', '').replace('struct', '').replace('enum', '').strip()
    if '*' in ctype:
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

    flat_content = content.replace('\n', ' ')
    flat_content = re.sub(r'\s+', ' ', flat_content)
    
    # functions
    func_pattern = re.compile(r'([a-zA-Z_][a-zA-Z0-9_\s\*]*?)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*;')
    for match in func_pattern.finditer(flat_content):
        ret_type = match.group(1).strip()
        func_name = match.group(2).strip()
        args_text = match.group(3).strip()
        
        if any(kw in ret_type for kw in ['typedef', 'return', 'else', 'if', 'struct ']) and not ret_type.startswith('extern ') and not ret_type.startswith('void '):
            if not (ret_type.startswith('struct') and '*' in ret_type):
                continue
                
        if func_name in ['if', 'while', 'for', 'switch']:
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

with open("/home/jehud/CLionProjects/spectre/tests/application/resources/app/resources/assets/scripts/declarations/spectre_api.h", "r") as f:
    res = parse_header(f.read())
print(len(res.splitlines()))
