import re

path = 'demo/resources/asteroid/resources/scripts/externals/spectre.lua'
with open(path, 'r') as f:
    content = f.read()

content = content.replace('function spectre.serializer.find_serializer(world, type)', 'function spectre.serializer.find_serializer(world, type_name)')
content = content.replace('ffi.C.spectre_serializer_find_serializer((type(world) == "table" and world.ptr) and world.ptr or world, type)', 'ffi.C.spectre_serializer_find_serializer((type(world) == "table" and world.ptr) and world.ptr or world, type_name)')

content = content.replace('function spectre.serializer.register_serializer(world, type, serializer)', 'function spectre.serializer.register_serializer(world, type_name, serializer)')
content = content.replace('ffi.C.spectre_serializer_register_serializer((type(world) == "table" and world.ptr) and world.ptr or world, type, serializer)', 'ffi.C.spectre_serializer_register_serializer((type(world) == "table" and world.ptr) and world.ptr or world, type_name, serializer)')

with open(path, 'w') as f:
    f.write(content)
