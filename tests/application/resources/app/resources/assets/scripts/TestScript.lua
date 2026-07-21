local ffi = require("ffi")
local flecs = require("ecs")
local spectre = require("spectre")

-- `g_world` is injected by scripts_module.cpp
local world = flecs.from_ptr(g_world)

local player = world:new("Hero")
local spectre_player = spectre.Entity(world, player)

---@param entity entity
function on_update(entity)
	-- Use spectre module function
	local transform = spectre_player:get_spectre_2D_transform_component_t()
	if transform ~= nil then
		transform.position_x = 0.0
		transform.position_y = 0.0
		spectre_player:mark_spectre_2D_transform_component_t_modified()
	end
	
	-- Test getting it again
	local get_transform = spectre_player:get_spectre_2D_transform_component_t()
	if get_transform ~= nil then
		print("Player Position: X=" .. get_transform.position_x .. ", Y=" .. get_transform.position_y)
	end
end
