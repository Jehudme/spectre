import os
import re

def fix_file(path):
    with open(path, 'r') as f:
        content = f.read()

    # Update lambdas
    content = re.sub(
        r'\[\]\(\s*ecs_world_t\*\s*([^,]*),\s*ecs_entity_t\s*([^,]+)\)\s*->\s*sandbox_properties_handle_t\s*\{',
        r'[](ecs_world_t* \1, ecs_entity_t serializer_entity, ecs_entity_t \2) -> sandbox_properties_handle_t {',
        content
    )
    
    content = re.sub(
        r'\[\]\(\s*ecs_world_t\*\s*([^,]*),\s*ecs_entity_t\s*([^,]+),\s*sandbox_properties_handle_t\s*([^)]+)\)\s*\{',
        r'[](ecs_world_t* \1, ecs_entity_t serializer_entity, ecs_entity_t \2, sandbox_properties_handle_t \3) {',
        content
    )

    with open(path, 'w') as f:
        f.write(content)

for root, _, files in os.walk('tests/unit'):
    for file in files:
        if file.endswith('.cpp') or file.endswith('.h'):
            fix_file(os.path.join(root, file))
