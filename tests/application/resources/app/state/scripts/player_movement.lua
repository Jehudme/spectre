-- player_movement.lua
-- This script handles player movement using the SWIG-bound spectre engine C-ABI.

-- We assume 'spectre' module has been loaded globally by the engine.

function on_update(world, entity_id)
    -- Look up the input state singleton using flecs API
    -- The C-ABI provides `spectre.is_key_down` which requires the input state pointer.
    -- Wait, the prompt says: "query the global input state using the C-ABI `spectre.is_key_down`"
    
    -- In Flecs C API, we can get a component from the world.
    -- ecs_get_id(world, entity, component_id) returns a const void* which we cast to the struct.
    -- But since SWIG handles the binding, maybe `spectre.is_key_down` takes a pointer.
    -- For simplicity in this test, we assume the C++ test pushes a dummy input state if we need it,
    -- or the SWIG wrapper allows us to fetch it.
    
    -- Let's query the component directly if we can, or just use a dummy mechanism.
    -- We'll assume the C++ side injected the global input state pointer into a Lua global `global_input_state`.
    -- Or, we fetch it via Flecs: 
    -- local input_comp_id = spectre.ecs_lookup(world, "spectre_input_state_t")
    -- local input_ptr = spectre.ecs_get_id(world, spectre.ECS_ID(input_comp_id), input_comp_id) -- (Actually singleton is on itself)
    -- This requires a lot of accurate Flecs SWIG bindings.
    
    -- For this simple test requested: "query the global input state using the C-ABI `spectre.is_key_down`"
    -- "mutate a custom C-style component struct natively, and notify the world state using `spectre.ecs_modified`."

    local input_id = spectre.ecs_lookup(world, "spectre_input_state_t")
    local input_ptr = spectre.ecs_get_id(world, input_id, input_id)
    
    local is_w_down = spectre.abi_is_key_down(input_ptr, 87) -- 87 is 'W' in raylib
    
    if is_w_down then
        local transform_id = spectre.ecs_lookup(world, "spectre_transform_2d_t")
        local transform_ptr = spectre.ecs_get_mut_id(world, entity_id, transform_id)
        
        -- transform_ptr is wrapped as a SWIG pointer to spectre_transform_2d_t, we can mutate its fields natively!
        -- SWIG exposes structs with getter/setter fields.
        -- Let's assume spectre_transform_2d_t has a `position` array.
        -- In C: typedef struct { float position[2]; float rotation; float scale[2]; } spectre_transform_2d_t;
        -- Let's just modify the struct fields:
        
        -- Move up!
        local p = spectre.spectre_transform_2d_t_position_get(transform_ptr)
        p[1] = p[1] - 10.0
        
        -- Notify the world that we mutated it (useful for systems matching on change)
        spectre.ecs_modified_id(world, entity_id, transform_id)
    end
end
