local ffi = require("ffi")
local ecs = require("ecs")

local world = ecs.from_ptr(g_world)

-- We can declare the C struct so LuaJIT FFI knows its memory layout.
-- We use a pcall to gracefully ignore if it's already defined (e.g. if script is evaluated twice).
pcall(function()
	ffi.cdef([[
        typedef struct spectre_2D_transform_component_t {
            float position_x;
            float position_y;
            float position_z;
            float scale_x;
            float scale_y;
            float rotation;
        } spectre_2D_transform_component_t;
    ]])
end)

-- Lookup the C++ registered Transform2D component and bind it to our FFI definition
local Transform2D = world:lookup("spectre.modules.renderer_module_t.Transform2D")
if Transform2D then
	world:bind_struct(Transform2D, "spectre_2D_transform_component_t")
end

local Velocity = world:lookup("TestVelocity")
if not Velocity then
	Velocity = world:new_struct("TestVelocity", "{ float x; float y; }")
end

local TestScript = {}

function TestScript.on_update(self_id, scene_id, state_id)
	if not world:has(self_id, Velocity) then
		world:set(self_id, Velocity, { x = 1.5, y = -0.5 })
	end

	local transform = world:get(self_id, Transform2D)
	local v = world:get(self_id, Velocity)

	if transform and v then
		print("[TestScript] Player Position: X=" .. transform.position_x .. ", Y=" .. transform.position_y)
		print("[TestScript] Player Velocity: X=" .. v.x .. ", Y=" .. v.y)

		-- Update position
		world:set(self_id, Transform2D, {
			position_x = transform.position_x + v.x,
			position_y = transform.position_y + v.y,
			position_z = transform.position_z,
			scale_x = transform.scale_x,
			scale_y = transform.scale_y,
			rotation = transform.rotation,
		})
	end
end

return {
	on_update = ecs.Script.define(TestScript.on_update, "self_id:integer", "scene_id:integer", "state_id:integer"),
}
