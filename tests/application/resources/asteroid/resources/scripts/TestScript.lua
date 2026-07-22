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
    world:set(self_id, Velocity, { x = 0.001, y = 0 })
  end

  local transform = world:get(self_id, spectre.Transform2D)
  local v = world:get(self_id, Velocity)

  if transform and v then
    sandbox.logs.info(
      world,
      "[TestScript] Player Position: X=" .. transform.position_x .. ", Y=" .. transform.position_y
    )
    sandbox.logs.info(world, "[TestScript] Player Velocity: X=" .. v.x .. ", Y=" .. v.y)

    transform.position_x = transform.position_x + v.x
    transform.position_y = transform.position_y + v.y

    world:set(self_id, spectre.Transform2D, transform)
  end
end

return {
  on_update = ecs.Script.define(TestScript.on_update, "self_id:integer", "scene_id:integer", "state_id:integer"),
}
