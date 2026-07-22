local ecs = require("ecs")
local spectre = require("externals.spectre")
local sandbox = require("externals.sandbox")

local world = ecs.from_ptr(g_world)

-- Initialize spectre components
spectre.init(world)

local Velocity = world:lookup("TestVelocity")
if not Velocity then
	Velocity = world:new_struct("TestVelocity", "{ float x; float y; }")
end

local TestScript = {}

function TestScript.on_update(self_id, scene_id, state_id)
	if not world:has(self_id, Velocity) then
		world:set(self_id, Velocity, { x = 1.5, y = -0.5 })
	end

	local transform = world:get(self_id, spectre.Transform2D)
	local v = world:get(self_id, Velocity)

	if transform and v then
		sandbox.logs.info("[TestScript] Player Position: X=" .. transform.position_x .. ", Y=" .. transform.position_y)
		sandbox.logs.info("[TestScript] Player Velocity: X=" .. v.x .. ", Y=" .. v.y)

		-- Update position
		world:set(self_id, spectre.Transform2D, {
			position_x = transform.position_x + v.x,
			position_y = transform.position_y + v.y,
			position_z = transform.position_z,
			scale_x = transform.scale_x,
			scale_y = transform.scale_y,
			origin_x = transform.origin_x,
			origin_y = transform.origin_y,
			rotation = transform.rotation,
		})
	end
end

return {
	on_update = ecs.Script.define(TestScript.on_update, "self_id:integer", "scene_id:integer", "state_id:integer"),
}
