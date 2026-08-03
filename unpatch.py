import os
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # We want to match:
    # {
    #     sandbox::properties schema;
    #     schema.load(R"({ ... })", sandbox::properties::Format::JSON);
    #     spectre::modules::components::register_component(m_world, "Name", register_fn, serializer, std::move(schema));
    # }
    
    # Regex to find these blocks and extract the register_component part without the std::move(schema)
    pattern = re.compile(
        r'\{\s*sandbox::properties schema;\s*schema\.load\(R"\([^)]*\)", sandbox::properties::Format::JSON\);\s*'
        r'(spectre::modules::components::register_component\([^,]+,\s*"[^"]+",\s*[^,]+,\s*[^,]+),\s*std::move\(schema\)\);\s*\}',
        re.MULTILINE
    )
    
    new_content = pattern.sub(r'\1);', content)
    
    # Wait, renderer_module.cpp has different indentation or multiple lines?
    # Let's also do a more generic replacement if the above doesn't catch everything.
    # We can match `schema` creation and `std::move(schema)`
    
    if new_content != content:
        with open(filepath, 'w') as f:
            f.write(new_content)
        print(f"Patched {filepath}")

for root, _, files in os.walk('/home/jehud/CLionProjects/spectre/spectre/source/modules'):
    for file in files:
        if file.endswith('.cpp'):
            process_file(os.path.join(root, file))

for root, _, files in os.walk('/home/jehud/CLionProjects/spectre/tests'):
    for file in files:
        if file.endswith('.cpp'):
            process_file(os.path.join(root, file))
