import os
import re

def process_file(filepath):
    with open(filepath, 'r') as f:
        content = f.read()

    # Update deserialize signature
    content = re.sub(
        r'void\s+deserialize_([a-zA-Z0-9_]+)\s*\(\s*ecs_world_t\*\s*world\s*,\s*ecs_entity_t\s+entity\s*,\s*sandbox_properties_handle_t\s+props\s*\)',
        r'void deserialize_\1(ecs_world_t* world, ecs_entity_t serializer_entity, ecs_entity_t entity, sandbox_properties_handle_t props)',
        content
    )
    
    # Update deserialize_empty signature
    content = re.sub(
        r'\[\]\(\s*ecs_world_t\*\s*[^,]+,\s*ecs_entity_t\s*[^,]+,\s*sandbox_properties_handle_t\s*[^)]+\)\s*\{',
        r'[](ecs_world_t*, ecs_entity_t, ecs_entity_t, sandbox_properties_handle_t) {',
        content
    )

    # Update serialize signature
    content = re.sub(
        r'sandbox_properties_handle_t\s+serialize_([a-zA-Z0-9_]+)\s*\(\s*ecs_world_t\*\s*world\s*,\s*ecs_entity_t\s+entity\s*\)',
        r'sandbox_properties_handle_t serialize_\1(ecs_world_t* world, ecs_entity_t serializer_entity, ecs_entity_t entity)',
        content
    )
    
    # Update serialize_empty signature
    content = re.sub(
        r'\[\]\(\s*ecs_world_t\*\s*[^,]+,\s*ecs_entity_t\s*[^)]+\)\s*->\s*sandbox_properties_handle_t\s*\{\s*return\s*\{0\};\s*\}',
        r'[](ecs_world_t*, ecs_entity_t, ecs_entity_t) -> sandbox_properties_handle_t { return {0}; }',
        content
    )

    with open(filepath, 'w') as f:
        f.write(content)

for root, _, files in os.walk('spectre'):
    for file in files:
        if file.endswith(('.cpp', '.h', '.hpp')):
            process_file(os.path.join(root, file))
