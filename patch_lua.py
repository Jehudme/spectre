import os
import glob
import re

base_dir = "/home/jehud/CLionProjects/spectre"
lua_files = glob.glob(f"{base_dir}/demo/**/externals/spectre.lua", recursive=True)

def patch_lua(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # 1. C definitions
    cdef_old = r'void spectre_components_register_component\(ecs_world_t\* world, const char\* name, spectre_component_registration_fn_t registration_fn, spectre_serializer_component serializer\);'
    cdef_new = '''void spectre_components_register_component(ecs_world_t* world, const char* name, spectre_component_registration_fn_t registration_fn, spectre_serializer_component serializer, sandbox_properties_handle_t schema_properties);
    bool spectre_components_is_static(ecs_world_t* world, const char* name);
    sandbox_properties_handle_t spectre_components_find_schema(ecs_world_t* world, const char* name);'''
    content = re.sub(cdef_old, cdef_new, content)

    # 2. Wrapper
    wrapper_old = r'function spectre\.components\.register_component\(world, name, registration_fn, serializer\)\n\s*return ffi\.C\.spectre_components_register_component\(\(type\(world\) == "table" and world\.ptr\) and world\.ptr or world, name, registration_fn, serializer\)\nend'
    wrapper_new = '''function spectre.components.register_component(world, name, registration_fn, serializer, schema_properties)
    local schema_handle = 0
    if type(schema_properties) == "table" then
        schema_handle = sandbox.properties.from_table(schema_properties)
    elseif schema_properties ~= nil then
        schema_handle = schema_properties
    end
    return ffi.C.spectre_components_register_component((type(world) == "table" and world.ptr) and world.ptr or world, name, registration_fn, serializer, schema_handle)
end

function spectre.components.is_static(world, name)
    return ffi.C.spectre_components_is_static((type(world) == "table" and world.ptr) and world.ptr or world, name)
end

function spectre.components.find_schema(world, name)
    local handle = ffi.C.spectre_components_find_schema((type(world) == "table" and world.ptr) and world.ptr or world, name)
    if handle == 0 then return nil end
    return sandbox.properties.to_table(handle)
end'''
    content = re.sub(wrapper_old, wrapper_new, content)

    with open(filepath, 'w') as f:
        f.write(content)
    print(f"Patched {filepath}")

for f in lua_files:
    patch_lua(f)
