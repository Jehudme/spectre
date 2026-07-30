import re

filepath = "/home/jehud/CLionProjects/spectre/demo/resources/asteroid/resources/scripts/TestScript.lua"
with open(filepath, 'r') as f:
    content = f.read()

test_code = r'''
function TestScript:init(world)
    -- Test schemas
    local static_schema = spectre.components.find_schema(world, "Transform2D")
    if static_schema then
        -- We will just print if it exists. Since sandbox properties isn't easily JSON printed in standard lua without helper, we can just print success
        sandbox.logs.info(world, "Transform2D schema found and is_static: " .. tostring(spectre.components.is_static(world, "Transform2D")))
    end

    local dyn_props = {
        members = {
            { name = "speed", type = "float" }
        }
    }
    spectre.components.register_component(world, "AsteroidDynamicComp", nil, nil, dyn_props)
    local dyn_schema = spectre.components.find_schema(world, "AsteroidDynamicComp")
    if dyn_schema then
        sandbox.logs.info(world, "AsteroidDynamicComp schema found and is_static: " .. tostring(spectre.components.is_static(world, "AsteroidDynamicComp")))
    end
'''

if 'AsteroidDynamicComp' not in content:
    content = content.replace('function TestScript:init(world)', test_code)
    
with open(filepath, 'w') as f:
    f.write(content)
print("Patched TestScript.lua")

# also create main.lua just in case
main_content = test_code.replace("TestScript:", "Main:")
main_content = "local Main = {}\n" + main_content + "\nreturn Main\n"
with open("/home/jehud/CLionProjects/spectre/demo/resources/asteroid/resources/scripts/main.lua", 'w') as f:
    f.write(main_content)
