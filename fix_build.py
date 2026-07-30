import glob
import re
import os

base_dir = "/home/jehud/CLionProjects/spectre"

# Fix unpatched multi-line calls
module_files = glob.glob(f"{base_dir}/spectre/source/modules/**/*_module.cpp", recursive=True)
for filepath in module_files:
    with open(filepath, 'r') as f:
        content = f.read()

    # match register_component(...) ending with ); without schema
    # But wait, what if it's already patched? We can just do a regex replace for multi-line
    new_content = re.sub(
        r'(spectre::modules::components::register_component\([^,]+,\s*"[^"]+",\s*[^,]+,\s*[^,)]+)\);',
        r'\1, sandbox::properties());',
        content
    )
    
    # Check if there are ones with only 3 args
    new_content = re.sub(
        r'(spectre::modules::components::register_component\([^,]+,\s*"[^"]+",\s*[^,)]+)\);',
        r'\1, spectre_serializer_component{}, sandbox::properties());',
        new_content
    )

    if new_content != content:
        with open(filepath, 'w') as f:
            f.write(new_content)
        print(f"Patched {filepath}")

# Fix components_service.cpp return 0
srv_file = f"{base_dir}/spectre/source/modules/components/components_service.cpp"
with open(srv_file, 'r') as f:
    sc = f.read()

sc = sc.replace('return 0;', 'sandbox_properties_handle_t null_handle = {0}; return null_handle;')
sc = sc.replace('sandbox_properties_handle_t null_handle = {0}; sandbox_properties_handle_t null_handle = {0}; return null_handle;', 'sandbox_properties_handle_t null_handle = {0}; return null_handle;')

with open(srv_file, 'w') as f:
    f.write(sc)
