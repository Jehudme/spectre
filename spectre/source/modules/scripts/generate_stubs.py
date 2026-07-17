import os
import re

def find_components(search_dirs):
    components = []
    # Match structs ending with _component_t, e.g., spectre_2D_transform_component_t or sandbox_transform_component_t
    # or input state e.g., spectre_input_state_t
    struct_pattern = re.compile(r'typedef\s+struct\s+([a-zA-Z0-9_]+)\s*\{')
    
    for d in search_dirs:
        for root, _, files in os.walk(d):
            for file in files:
                if file.endswith('.h') or file.endswith('.hpp'):
                    path = os.path.join(root, file)
                    with open(path, 'r', encoding='utf-8') as f:
                        content = f.read()
                        matches = struct_pattern.findall(content)
                        for match in matches:
                            if match.endswith('_component_t') or match == 'spectre_input_state_t':
                                fields = []
                                # extract fields for EmmyLua
                                struct_body_match = re.search(r'typedef\s+struct\s+' + match + r'\s*\{([^}]*)\}', content)
                                if struct_body_match:
                                    body = struct_body_match.group(1)
                                    # Very naive field parsing
                                    for line in body.split('\n'):
                                        line = line.strip()
                                        if not line or line.startswith('//') or line.startswith('#'): continue
                                        if ';' in line:
                                            # e.g., "float x;" or "uint32_t width;"
                                            parts = line.replace(';', '').split()
                                            if len(parts) >= 2:
                                                type_name = parts[0]
                                                field_name = parts[-1].replace('[2]', '') # hack for arrays
                                                # map C types to Lua types
                                                lua_type = 'number'
                                                if 'bool' in type_name: lua_type = 'boolean'
                                                elif 'char*' in type_name: lua_type = 'string'
                                                fields.append((field_name, lua_type))
                                components.append((match, fields))
    return components

def generate_files(components, output_dir):
    swig_i = os.path.join(output_dir, 'generated_components.i')
    lua_stub = os.path.join(output_dir, 'spectre_api_stubs.lua')
    
    with open(swig_i, 'w') as f:
        f.write('// Auto-generated SWIG templates for components\n\n')
        f.write('%extend spectre_world {\n')
        f.write('    ecs_entity_t lookup(const char* name) {\n')
        f.write('        return ecs_lookup(self->world, name);\n')
        f.write('    }\n')
        for comp, _ in components:
            short_name = comp.replace('spectre_', '').replace('sandbox_', '').replace('_component_t', '').replace('_state_t', '')
            f.write(f'    {comp}* get_mut_{short_name}(ecs_entity_t e) {{\n')
            f.write(f'        if (!e) return nullptr;\n')
            f.write(f'        ecs_entity_t comp_id = ecs_lookup(self->world, "{comp}");\n')
            f.write(f'        if (!comp_id) comp_id = flecs::world(self->world).id<{comp}>();\n')
            f.write(f'        return ({comp}*)ecs_get_mut_id(self->world, e, comp_id);\n')
            f.write(f'    }}\n')
            
            f.write(f'    {comp}* get_mut_singleton_{short_name}() {{\n')
            f.write(f'        ecs_entity_t comp_id = ecs_lookup(self->world, "{comp}");\n')
            f.write(f'        if (!comp_id) comp_id = flecs::world(self->world).id<{comp}>();\n')
            f.write(f'        return ({comp}*)ecs_get_mut_id(self->world, comp_id, comp_id);\n')
            f.write(f'    }}\n')

            f.write(f'    const {comp}* get_{short_name}(ecs_entity_t e) {{\n')
            f.write(f'        if (!e) return nullptr;\n')
            f.write(f'        ecs_entity_t comp_id = ecs_lookup(self->world, "{comp}");\n')
            f.write(f'        if (!comp_id) comp_id = flecs::world(self->world).id<{comp}>();\n')
            f.write(f'        return (const {comp}*)ecs_get_id(self->world, e, comp_id);\n')
            f.write(f'    }}\n')
            
            f.write(f'    const {comp}* get_singleton_{short_name}() {{\n')
            f.write(f'        ecs_entity_t comp_id = ecs_lookup(self->world, "{comp}");\n')
            f.write(f'        if (!comp_id) comp_id = flecs::world(self->world).id<{comp}>();\n')
            f.write(f'        return (const {comp}*)ecs_get_id(self->world, comp_id, comp_id);\n')
            f.write(f'    }}\n')

            f.write(f'    void set_{short_name}(ecs_entity_t e, const {comp}& value) {{\n')
            f.write(f'        if (!e) return;\n')
            f.write(f'        ecs_entity_t comp_id = ecs_lookup(self->world, "{comp}");\n')
            f.write(f'        if (!comp_id) comp_id = flecs::world(self->world).id<{comp}>();\n')
            f.write(f'        ecs_set_id(self->world, e, comp_id, sizeof({comp}), &value);\n')
            f.write(f'    }}\n')
            
            f.write(f'    bool has_{short_name}(ecs_entity_t e) {{\n')
            f.write(f'        if (!e) return false;\n')
            f.write(f'        ecs_entity_t comp_id = ecs_lookup(self->world, "{comp}");\n')
            f.write(f'        if (!comp_id) comp_id = flecs::world(self->world).id<{comp}>();\n')
            f.write(f'        return ecs_has_id(self->world, e, comp_id);\n')
            f.write(f'    }}\n\n')
        f.write('}\n')

    with open(lua_stub, 'w') as f:
        f.write('---@meta\n\n')
        for comp, fields in components:
            f.write(f'---@class {comp}\n')
            for field_name, lua_type in fields:
                f.write(f'---@field {field_name} {lua_type}\n')
            f.write('\n')
        
        f.write('---@class spectre_world\n')
        f.write('local spectre_world = {}\n\n')
        
        for comp, _ in components:
            short_name = comp.replace('spectre_', '').replace('sandbox_', '').replace('_component_t', '').replace('_state_t', '')
            f.write(f'---@param entity number\n')
            f.write(f'---@return {comp}\n')
            f.write(f'function spectre_world:get_mut_{short_name}(entity) end\n\n')
            
            f.write(f'---@return {comp}\n')
            f.write(f'function spectre_world:get_mut_singleton_{short_name}() end\n\n')

            f.write(f'---@param entity number\n')
            f.write(f'---@return {comp}\n')
            f.write(f'function spectre_world:get_{short_name}(entity) end\n\n')
            
            f.write(f'---@return {comp}\n')
            f.write(f'function spectre_world:get_singleton_{short_name}() end\n\n')

            f.write(f'---@param entity number\n')
            f.write(f'---@param value {comp}\n')
            f.write(f'function spectre_world:set_{short_name}(entity, value) end\n\n')
            
            f.write(f'---@param entity number\n')
            f.write(f'---@return boolean\n')
            f.write(f'function spectre_world:has_{short_name}(entity) end\n\n')

if __name__ == '__main__':
    search_dirs = [
        '../../../include',
        '../../../../../sandbox/include'
    ]
    components = find_components(search_dirs)
    generate_files(components, '.')
    print(f"Generated bindings for {len(components)} components.")
