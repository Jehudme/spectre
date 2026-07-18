import sys
import os
import re

def c_type_to_lua_type(c_type):
    c_type = c_type.strip()
    c_type = c_type.replace('const ', '').replace('volatile ', '')
    if c_type.endswith('*'):
        if 'char' in c_type:
            return 'string'
        return 'any'
    if c_type in ['void']:
        return 'nil'
    if c_type in ['bool', '_Bool']:
        return 'boolean'
    if c_type in ['float', 'double', 'f32', 'f64']:
        return 'number'
    if 'int' in c_type or c_type in ['u8', 'u16', 'u32', 'u64', 'i8', 'i16', 'i32', 'i64', 'size_t']:
        return 'number'
    return 'any'

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 generate_wrappers.py <module_name> <header_file> <output_ffi> <output_wrapper>")
        sys.exit(1)
        
    module_name = sys.argv[1]
    input_file = sys.argv[2]
    output_ffi = sys.argv[3]
    output_wrapper = sys.argv[4]
    
    with open(input_file, 'r') as f:
        flat_content = f.read()
        
    flat_content = re.sub(r'//.*', '', flat_content)
    flat_content = re.sub(r'\s+', ' ', flat_content)
    
    stubs = []
    stubs.append("---@meta")
    stubs.append("")
    stubs.append("local ffi = {}")
    stubs.append("ffi.C = {}")
    stubs.append("function ffi.cdef(def) end")
    stubs.append("function ffi.new(ct, ...) end")
    stubs.append("function ffi.cast(ct, val) end")
    stubs.append("function ffi.string(ptr, len) end")
    stubs.append("")
    
    wrapper = []
    wrapper.append(f"local {module_name} = {{}}")
    wrapper.append("local ffi = require('ffi')")
    wrapper.append(f"local flecs = require('wrappers.flecs')")
    wrapper.append("")
    
    struct_pattern = re.compile(r'typedef\s+struct\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\{([^}]*)\}\s*\1\s*;')
    components = []
    
    for match in struct_pattern.finditer(flat_content):
        struct_name = match.group(1).strip()
        fields_text = match.group(2).strip()
        
        stubs.append(f"---@class {struct_name}")
        
        for field in fields_text.split(';'):
            field = field.strip()
            if not field: continue
            
            parts = field.split()
            if len(parts) >= 2:
                fname = parts[-1].lstrip('*')
                if '[' in fname:
                    fname = fname.split('[')[0]
                ftype = ' '.join(parts[:-1]) + '*' * (len(parts[-1]) - len(fname))
                l_type = c_type_to_lua_type(ftype)
                stubs.append(f"---@field {fname} {l_type}")
                
        stubs.append(f"local {struct_name} = {{}}")
        stubs.append("")
        
        if struct_name.endswith("_component_t") or struct_name.endswith("_relation_t") or struct_name.endswith("_t"):
            components.append(struct_name)
            
    # functions
    func_pattern = re.compile(r'([a-zA-Z_][a-zA-Z0-9_\s\*]*?)\s+([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\)\s*;')
    for match in func_pattern.finditer(flat_content):
        ret_type = match.group(1).strip()
        func_name = match.group(2).strip()
        args_text = match.group(3).strip()
        
        if any(kw in ret_type for kw in ['typedef', 'return', 'else', 'if', 'struct ']):
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
                        
                    l_type = c_type_to_lua_type(atype)
                    args.append(aname)
                    param_docs.append(f"---@param {aname} {l_type}")
                    
        for doc in param_docs:
            stubs.append(doc)
            
        if l_ret != 'nil':
            stubs.append(f"---@return {l_ret}")
            
        stubs.append(f"function ffi.C.{func_name}({', '.join(args)}) end")
        stubs.append("")
        
        wrapper.append(f"{module_name}.{func_name} = ffi.C.{func_name}")

    if components:
        for comp in components:
            wrapper.append(f"---@return {comp}|nil")
            wrapper.append(f"function flecs.EntityMeta:get_{comp}()")
            wrapper.append(f"    local comp_id = flecs.get_component_id(self.world, '{comp}')")
            wrapper.append(f"    if comp_id == 0 then return nil end")
            wrapper.append(f"    local ptr = ffi.C.ecs_get_mut_id(self.world, self.id, comp_id)")
            wrapper.append(f"    if ptr ~= nil then return ffi.cast('{comp}*', ptr) end")
            wrapper.append(f"    return nil")
            wrapper.append(f"end")
            wrapper.append("")
            
            wrapper.append(f"function flecs.EntityMeta:mark_{comp}_modified()")
            wrapper.append(f"    local comp_id = flecs.get_component_id(self.world, '{comp}')")
            wrapper.append(f"    if comp_id ~= 0 then")
            wrapper.append(f"        ffi.C.ecs_modified_id(self.world, self.id, comp_id)")
            wrapper.append(f"    end")
            wrapper.append(f"end")
            wrapper.append("")
            
            wrapper.append(f"function flecs.EntityMeta:add_{comp}()")
            wrapper.append(f"    local comp_id = flecs.get_component_id(self.world, '{comp}')")
            wrapper.append(f"    if comp_id ~= 0 then")
            wrapper.append(f"        ffi.C.ecs_add_id(self.world, self.id, comp_id)")
            wrapper.append(f"    end")
            wrapper.append(f"end")
            wrapper.append("")
            
            wrapper.append(f"function flecs.EntityMeta:remove_{comp}()")
            wrapper.append(f"    local comp_id = flecs.get_component_id(self.world, '{comp}')")
            wrapper.append(f"    if comp_id ~= 0 then")
            wrapper.append(f"        ffi.C.ecs_remove_id(self.world, self.id, comp_id)")
            wrapper.append(f"    end")
            wrapper.append(f"end")
            wrapper.append("")
            
            wrapper.append(f"---@return boolean")
            wrapper.append(f"function flecs.EntityMeta:has_{comp}()")
            wrapper.append(f"    local comp_id = flecs.get_component_id(self.world, '{comp}')")
            wrapper.append(f"    if comp_id ~= 0 then")
            wrapper.append(f"        return ffi.C.ecs_has_id(self.world, self.id, comp_id) == true")
            wrapper.append(f"    end")
            wrapper.append(f"    return false")
            wrapper.append(f"end")
            wrapper.append("")
            
    wrapper.append(f"function {module_name}.Entity(world, id)")
    wrapper.append(f"    return flecs.Entity(world, id)")
    wrapper.append(f"end")
    wrapper.append("")
    wrapper.append(f"return {module_name}")
    
    stubs.append("return ffi")
    
    os.makedirs(os.path.dirname(output_ffi), exist_ok=True)
    with open(output_ffi, "w") as f:
        f.write("\n".join(stubs))
        
    os.makedirs(os.path.dirname(output_wrapper), exist_ok=True)
    with open(output_wrapper, "w") as f:
        f.write("\n".join(wrapper))

if __name__ == "__main__":
    main()
