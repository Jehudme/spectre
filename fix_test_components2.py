import re

path = 'tests/unit/source/modules/test_components.cpp'
with open(path, 'r') as f:
    content = f.read()

content = content.replace(
    'spectre::modules::serializer::deserialize_entity(world, serializer_entity.id(), target.id(), std::move(input_props));',
    'spectre::modules::serializer::deserialize_entity(world, serializer_entity.id(), target.id(), input_props.get_handle());'
)
content = content.replace(
    'sandbox::properties out_props = spectre::modules::serializer::serialize_entity(world, serializer_entity.id(), target.id());',
    'sandbox::properties out_props(spectre::modules::serializer::serialize_entity(world, serializer_entity.id(), target.id()), true);'
)

with open(path, 'w') as f:
    f.write(content)
