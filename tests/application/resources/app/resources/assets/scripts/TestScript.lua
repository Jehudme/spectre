local ecs = require("ecs")
local spectre = require("externals.spectre")
local sandbox = require("externals.sandbox")

local world = ecs.from_ptr(g_world)

local TestScript = {}

function TestScript.on_update(self_id, scene_id, state_id)
	local transform = world:get(self_id, spectre.Transform2D)
end

return {
	on_update = ecs.Script.define(TestScript.on_update, "self_id:integer", "scene_id:integer", "state_id:integer"),
}
