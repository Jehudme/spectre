local ffi = require("ffi")
local flecs = require("ecs")

local world = flecs.init()

local Position = world:new_struct("Position", "{ float x; float y; }")
local Velocity = world:new_struct("Velocity", "{ float x; float y; }")

local player = world:new("Hero")


---@param entity entity
function on_update(entity)
	world:set(player, Position, { x = 0.0, y = 0.0 })
	world:set(player, Velocity, { x = 5.0, y = -2.5 })

	local p = world:get(player, Position)
	local v = world:get(player, Velocity)
	if p then
		print("Player Position: X=" .. p.x .. ", Y=" .. p.y)
		print("Player Velocity: Y=" .. v.x .. ", Y=" .. v.y)
	end

end
