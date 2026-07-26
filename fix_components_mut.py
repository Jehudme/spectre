import re

path = 'spectre/source/modules/components/components_module.cpp'
with open(path, 'r') as f:
    content = f.read()

new_content = content.replace(
    'void* comp_data = ecs_get_mut_id(world, target_entity, comp_id.id());',
    'if (!ecs_is_valid(world, target_entity)) { sandbox::modules::logs::error(w, "target_entity is invalid"); return; }\necs_add_id(world, target_entity, comp_id.id());\nvoid* comp_data = ecs_get_mut_id(world, target_entity, comp_id.id());'
)

with open(path, 'w') as f:
    f.write(new_content)
