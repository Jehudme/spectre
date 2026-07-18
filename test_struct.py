import re

def c_type_to_lua_type(ctype):
    return 'number'

fields_text = "float position_x, position_y, position_z; int x; float a, b;"
for line in fields_text.split(';'):
    line = line.strip()
    if not line: continue
    # example line: `float position_x, position_y, position_z`
    # or `struct ecs_vec_t *vec`
    # The type is everything before the first comma, minus the last word.
    
    parts = line.split(',')
    # parts: ['float position_x', ' position_y', ' position_z']
    first_part = parts[0].strip().split()
    if len(first_part) < 2: continue
    
    fname = first_part[-1].lstrip('*')
    ftype = ' '.join(first_part[:-1]) + '*' * (len(first_part[-1]) - len(fname))
    
    if '[' in fname: fname = fname.split('[')[0]
    print(f"---@field {fname} {c_type_to_lua_type(ftype)}")
    
    for p in parts[1:]:
        p = p.strip()
        fname2 = p.lstrip('*')
        if '[' in fname2: fname2 = fname2.split('[')[0]
        # the type is the same base type, plus pointers if any
        print(f"---@field {fname2} {c_type_to_lua_type(ftype)}")
