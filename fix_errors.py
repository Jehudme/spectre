import re
import glob
import sys

base_dir = "/home/jehud/CLionProjects/spectre"

# Fix modules using std::move(schema)
module_files = glob.glob(f"{base_dir}/spectre/source/modules/**/*_module.cpp", recursive=True)
for filepath in module_files:
    with open(filepath, 'r') as f:
        content = f.read()
    
    # We may have replaced it already with std::move(schema) but let's make sure it's correct
    content = content.replace(', schema);', ', std::move(schema));')
    with open(filepath, 'w') as f:
        f.write(content)

# Fix components_module.cpp
mod_file = f"{base_dir}/spectre/source/modules/components/components_module.cpp"
with open(mod_file, 'r') as f:
    mc = f.read()

mc = re.sub(r'serializer_entity\.set<spectre_component_schema_t>\(\{schema_properties\}\);', 
            r'spectre_component_schema_t schema_comp; schema_comp.schema = sandbox::properties(schema_properties.get_raw(), true); serializer_entity.set<spectre_component_schema_t>(std::move(schema_comp));', 
            mc)

mc = re.sub(r'serializer_entity\.set<spectre_component_schema_t>\(\{properties\}\);',
            r'spectre_component_schema_t dyn_schema_comp; dyn_schema_comp.schema = sandbox::properties(properties.get_raw(), true); serializer_entity.set<spectre_component_schema_t>(std::move(dyn_schema_comp));',
            mc)

with open(mod_file, 'w') as f:
    f.write(mc)
