local Main = {}

function Main:init(world)
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
end

return Main
