local ecs = require("ecs")
local spectre = require("spectre")
local sandbox = require("sandbox")
local imgui = require("imgui")
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

	local PlayerStats = spectre.components.find_component(world, "PlayerStats")
	sandbox.logs.info(world, "[TestScript] PlayerStats ID: " .. tostring(PlayerStats))
	if PlayerStats and PlayerStats ~= 0 then
		local PlayerStatsSerializer = spectre.serializer.find_serializer(world, "PlayerStats")
		sandbox.logs.info(world, "[TestScript] PlayerStatsSerializer ID: " .. tostring(PlayerStatsSerializer))

		if not world:has(self_id, PlayerStats) then
			local props = sandbox.Properties.new()
			props:set_int64("health", 100)
			props:set_double("speed", 5.5)
			props:set_string("name", "Hero")
			props:set_bool("is_alive", true)

			spectre.serializer.deserialize_entity(world, PlayerStatsSerializer, self_id, props:get_handle())
			props:destroy()
			sandbox.logs.info(world, "[TestScript] Assigned dynamic PlayerStats component to entity!")
		else
			local props_handle = spectre.serializer.serialize_entity(world, PlayerStatsSerializer, self_id)
			local props = { handle = props_handle }
			setmetatable(props, sandbox.Properties)

			local health = props:get_int64("health")
			local speed = props:get_double("speed")
			local name = props:read_string("name")
			local alive = props:get_bool("is_alive")

			sandbox.logs.info(
				world,
				"[TestScript] PlayerStats - Health: "
					.. tostring(health)
					.. ", Speed: "
					.. tostring(speed)
					.. ", Name: "
					.. tostring(name)
					.. ", Alive: "
					.. tostring(alive)
			)

			props:set_int64("health", health - 1)
			spectre.serializer.deserialize_entity(world, PlayerStatsSerializer, self_id, props.handle)
			props:destroy()
		end
	else
		sandbox.logs.warn(
			world,
			"[TestScript] Could not find dynamic component 'PlayerStats'! Type: "
				.. type(PlayerStats)
				.. " Value: "
				.. tostring(PlayerStats)
		)
	end

	imgui.Begin("Test ImGui Window from Lua", nil, 0)
	imgui.Text("Hello from LuaJIT and statically linked cimgui!")
	if imgui.Button("Click Me!", imgui.ImVec2(120, 30)) then
		sandbox.logs.info(world, "[TestScript] ImGui Button Clicked!")
	end
	imgui.End()
end

return {
	on_update = ecs.Script.define(TestScript.on_update, "self_id:integer", "scene_id:integer", "state_id:integer"),
}
