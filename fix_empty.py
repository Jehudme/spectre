import os
import re

def fix_file(path):
    with open(path, 'r') as f:
        content = f.read()

    content = re.sub(
        r'static void deserialize_empty\(ecs_world_t\*, ecs_entity_t, sandbox_properties_handle_t\)\s*\{\}',
        r'static void deserialize_empty(ecs_world_t*, ecs_entity_t, ecs_entity_t, sandbox_properties_handle_t) {}',
        content
    )
    
    content = re.sub(
        r'static sandbox_properties_handle_t serialize_empty\(ecs_world_t\*, ecs_entity_t\)\s*\{\s*return\s*\{0\};\s*\}',
        r'static sandbox_properties_handle_t serialize_empty(ecs_world_t*, ecs_entity_t, ecs_entity_t) { return {0}; }',
        content
    )
    
    content = re.sub(
        r'\[\]\(\s*ecs_world_t\*,\s*ecs_entity_t,\s*sandbox_properties_handle_t\s*\)\s*\{\}',
        r'[](ecs_world_t*, ecs_entity_t, ecs_entity_t, sandbox_properties_handle_t) {}',
        content
    )
    
    content = re.sub(
        r'\[\]\(\s*ecs_world_t\*,\s*ecs_entity_t\s*\)\s*->\s*sandbox_properties_handle_t\s*\{\s*return\s*\{0\};\s*\}',
        r'[](ecs_world_t*, ecs_entity_t, ecs_entity_t) -> sandbox_properties_handle_t { return {0}; }',
        content
    )

    with open(path, 'w') as f:
        f.write(content)

for root, _, files in os.walk('spectre/source/modules'):
    for file in files:
        if file.endswith('.cpp') or file.endswith('.h'):
            fix_file(os.path.join(root, file))
            
for root, _, files in os.walk('tests'):
    for file in files:
        if file.endswith('.cpp') or file.endswith('.h'):
            fix_file(os.path.join(root, file))
