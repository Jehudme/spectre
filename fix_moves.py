import re
import glob

# 1. Fix patch_modules.py and re-run
filepath = "/home/jehud/CLionProjects/spectre/patch_modules.py"
with open(filepath, 'r') as f:
    content = f.read()

content = content.replace(', schema);', ', std::move(schema));')
with open(filepath, 'w') as f:
    f.write(content)

# 2. Fix test_components.cpp
test_file = "/home/jehud/CLionProjects/spectre/tests/unit/source/modules/test_components.cpp"
with open(test_file, 'r') as f:
    tc = f.read()
tc = tc.replace(', dummy_props);', ', std::move(dummy_props));')
with open(test_file, 'w') as f:
    f.write(tc)

# 3. Fix components_service.cpp
srv_file = "/home/jehud/CLionProjects/spectre/spectre/source/modules/components/components_service.cpp"
with open(srv_file, 'r') as f:
    sc = f.read()
sc = sc.replace('sandbox::properties(schema_properties, false)', 'std::move(sandbox::properties(schema_properties, false))')
sc = sc.replace('schema_properties.get_raw()', 'std::move(schema_properties).get_raw()')
with open(srv_file, 'w') as f:
    f.write(sc)

# 4. Fix components_module.cpp
mod_file = "/home/jehud/CLionProjects/spectre/spectre/source/modules/components/components_module.cpp"
with open(mod_file, 'r') as f:
    mc = f.read()
mc = mc.replace('schema_comp.schema = sandbox::properties(schema_properties.get_raw(), true);', 'schema_comp.schema = sandbox::properties(schema_properties.get_raw(), true);')
with open(mod_file, 'w') as f:
    f.write(mc)

