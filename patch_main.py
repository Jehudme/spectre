import re
import os

filepath = "/home/jehud/CLionProjects/spectre/demo/resources/asteroid/resources/scripts/main.lua"
with open(filepath, 'r') as f:
    content = f.read()

test_code = r'''
    -- Test components API for schemas
    local static_schema = spectre.components.find_schema(world, "Transform2D")
    sandbox.logs.info(world, "Transform2D schema: " .. (static_schema and sandbox.properties.to_json(static_schema) or "nil"))
    
    local is_static = spectre.components.is_static(world, "Transform2D")
    sandbox.logs.info(world, "Transform2D is_static: " .. tostring(is_static))

    local dyn_props = {
        members = {
            { name = "speed", type = "float" }
        }
    }
    spectre.components.register_component(world, "AsteroidDynamicComp", nil, nil, dyn_props)
    local dyn_schema = spectre.components.find_schema(world, "AsteroidDynamicComp")
    sandbox.logs.info(world, "AsteroidDynamicComp schema: " .. (dyn_schema and sandbox.properties.to_json(dyn_schema) or "nil"))
    local dyn_is_static = spectre.components.is_static(world, "AsteroidDynamicComp")
    sandbox.logs.info(world, "AsteroidDynamicComp is_static: " .. tostring(dyn_is_static))
'''

if 'AsteroidDynamicComp' not in content:
    # insert inside App:init
    content = content.replace('function App:init(world)', 'function App:init(world)\n' + test_code)

with open(filepath, 'w') as f:
    f.write(content)
print("Patched main.lua")
