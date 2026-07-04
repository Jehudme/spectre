print("Player movement script loaded")

local frame_count = 0

function player_movement(world, entity_id)
    -- Move the player slightly to the right every frame
    spectre.move_entity(world, entity_id, 1.0, 0.0)
    
    frame_count = frame_count + 1
    if frame_count % 60 == 0 then
        print("[LUA] player_movement executed 60 frames for entity " .. tostring(entity_id))
    end
end
