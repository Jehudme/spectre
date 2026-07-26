import os
import re

def fix_file(path):
    with open(path, 'r') as f:
        content = f.read()

    # deserialize
    content = re.sub(
        r'(void\s+deserialize_[a-zA-Z0-9_]+\s*\(\s*ecs_world_t\*\s*[^,]+,\s*ecs_entity_t\s+)([^,]+)(\s*,\s*sandbox_properties_handle_t\s+[^)]+\))',
        r'\1serializer_entity, ecs_entity_t \2\3',
        content
    )
    
    # serialize
    content = re.sub(
        r'(sandbox_properties_handle_t\s+serialize_[a-zA-Z0-9_]+\s*\(\s*ecs_world_t\*\s*[^,]+,\s*ecs_entity_t\s+)([^)]+)(\))',
        r'\1serializer_entity, ecs_entity_t \2\3',
        content
    )
    
    # empty handlers
    content = re.sub(
        r'\[\]\(\s*ecs_world_t\*\s*([^,]+),\s*ecs_entity_t\s*([^,]+),\s*sandbox_properties_handle_t\s*([^)]+)\)\s*\{',
        r'[](ecs_world_t* \1, ecs_entity_t serializer_entity, ecs_entity_t \2, sandbox_properties_handle_t \3) {',
        content
    )
    content = re.sub(
        r'\[\]\(\s*ecs_world_t\*\s*([^,]+),\s*ecs_entity_t\s*([^)]+)\)\s*->\s*sandbox_properties_handle_t\s*\{\s*return\s*\{0\};\s*\}',
        r'[](ecs_world_t* \1, ecs_entity_t serializer_entity, ecs_entity_t \2) -> sandbox_properties_handle_t { return {0}; }',
        content
    )

    with open(path, 'w') as f:
        f.write(content)

for root, _, files in os.walk('spectre/source/modules'):
    for file in files:
        if file.endswith('.cpp') or file.endswith('.h'):
            fix_file(os.path.join(root, file))
