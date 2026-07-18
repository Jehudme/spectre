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
    stubs.append("local ffi = {}")
    stubs.append("")
    stubs.append("---@class ffi.C")
    stubs.append("ffi.C = {}")
    stubs.append("")
    
    stubs.append("function ffi.cdef(def) end")
    stubs.append("function ffi.new(ct, ...) end")
    stubs.append("function ffi.typeof(ct) end")
    stubs.append("function ffi.cast(ct, init) end")
    stubs.append("function ffi.metatype(ct, metatable) end")
    stubs.append("function ffi.gc(cdata, finalizer) end")
    stubs.append("function ffi.sizeof(ct, ...) end")
    stubs.append("function ffi.alignof(ct) end")
    stubs.append("function ffi.offsetof(ct, field) end")
    stubs.append("function ffi.istype(ct, obj) end")
    stubs.append("function ffi.errno(newerr) end")
    stubs.append("function ffi.string(ptr, len) end")
    stubs.append("function ffi.copy(dst, src, len) end")
    stubs.append("function ffi.fill(dst, len, c) end")
    stubs.append("function ffi.abi(param) end")
    stubs.append("function ffi.os() end")
    stubs.append("function ffi.arch() end")
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
            
            parts = line.split(',')
            first_part = parts[0].strip().split()
            if len(first_part) < 2: continue
            
            fname = first_part[-1].lstrip('*')
            ftype = ' '.join(first_part[:-1]) + '*' * (len(first_part[-1]) - len(fname))
            if '[' in fname: fname = fname.split('[')[0]
            
            ltype = c_type_to_lua_type(ftype)
            if re.match(r'^[a-zA-Z_][a-zA-Z0-9_]*$', fname):
                stubs.append(f"---@field {fname} {ltype}")
            
            for p in parts[1:]:
                p = p.strip()
                fname2 = p.lstrip('*')
                if '[' in fname2: fname2 = fname2.split('[')[0]
                if re.match(r'^[a-zA-Z_][a-zA-Z0-9_]*$', fname2):
                    stubs.append(f"---@field {fname2} {ltype}")
        
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
            
        USEFUL_FLECS_FUNCTIONS = {
            "ecs_get_mut_id",
            "ecs_lookup_symbol",
            "ecs_new_id",
            "ecs_add_id",
            "ecs_remove_id",
            "ecs_delete",
            "ecs_has_id",
            "ecs_modified_id",
            "ecs_get_id",
            "ecs_set_id",
            "ecs_enable",
            "ecs_disable",
            "ecs_enable_id",
            "ecs_disable_id",
            "ecs_is_alive",
            "ecs_is_valid",
            "ecs_clear",
        }
        
        if func_name.startswith("ecs_"):
            if func_name not in USEFUL_FLECS_FUNCTIONS:
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

    return "\n".join(stubs) + "\n\nreturn ffi\n"

def generate_wrapper(structs):
    lines = []
    lines.append("local ffi = require('ffi')")
    lines.append("local spectre = {}")
    lines.append("spectre.ComponentIDs = {}")
    lines.append("")
    lines.append("---@class spectre.Entity")
    lines.append("---@field world any")
    lines.append("---@field id number")
    lines.append("local Entity = {}")
    lines.append("Entity.__index = Entity")
    lines.append("")
    lines.append("function spectre.get_component_id(world, name)")
    lines.append("    local id = spectre.ComponentIDs[name]")
    lines.append("    if not id then")
    lines.append("        id = ffi.C.ecs_lookup_symbol(world, name, true, true)")
    lines.append("        spectre.ComponentIDs[name] = id")
    lines.append("    end")
    lines.append("    return id")
    lines.append("end")
    lines.append("")
    lines.append("---@param world any")
    lines.append("---@param id number")
    lines.append("---@return spectre.Entity")
    lines.append("function spectre.Entity(world, id)")
    lines.append("    return setmetatable({ world = world, id = id }, Entity)")
    lines.append("end")
    lines.append("")
    
    for s in structs:
        lines.append(f"---@return {s}|nil")
        lines.append(f"function Entity:get_{s}()")
        lines.append(f"    local comp_id = spectre.get_component_id(self.world, '{s}')")
        lines.append(f"    if comp_id == 0 then return nil end")
        lines.append(f"    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)")
        lines.append(f"    if ptr ~= nil then return ffi.cast('{s}*', ptr) end")
        lines.append(f"    return nil")
        lines.append(f"end")
        lines.append("")
        
        lines.append(f"function Entity:mark_{s}_modified()")
        lines.append(f"    local comp_id = spectre.get_component_id(self.world, '{s}')")
        lines.append(f"    if comp_id ~= 0 then")
        lines.append(f"        ffi.C.ecs_modified_id(self.world, self.id, comp_id)")
        lines.append(f"    end")
        lines.append(f"end")
        lines.append("")

    lines.append("return spectre")
    return "\n".join(lines)

if __name__ == '__main__':
    if len(sys.argv) < 3:
        print("Usage: python3 generate_ffi_stubs.py <input.h> <output.lua> [wrapper.lua]")
        sys.exit(1)
        
    with open(sys.argv[1], 'r') as f:
        content = f.read()
        
    struct_names = []
    struct_pattern = re.compile(r'typedef\s+struct\s+([a-zA-Z0-9_]+)\s*\{([^}]*)\}\s*\1\s*;')
    for match in struct_pattern.finditer(content):
        struct_names.append(match.group(1))

    stubs = parse_header(content)
    
    os.makedirs(os.path.dirname(os.path.abspath(sys.argv[2])), exist_ok=True)
    with open(sys.argv[2], 'w') as f:
        f.write(stubs)
        
    if len(sys.argv) >= 4:
        wrapper = generate_wrapper(struct_names)
        os.makedirs(os.path.dirname(os.path.abspath(sys.argv[3])), exist_ok=True)
        with open(sys.argv[3], 'w') as f:
            f.write(wrapper)
